//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2025, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------

#include "tsAbstractDescrambler.h"
#include "tsMemory.h"

// Stack usage required by this module in the ECM deciphering thread.
#define ECM_THREAD_STACK_OVERHEAD (16  * 1024)


//----------------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------------

ts::AbstractDescrambler::AbstractDescrambler(TSP* tsp_, const UString& description, const UString& syntax, size_t stack_usage) :
    ProcessorPlugin(tsp_, description, syntax),
    _stack_usage(stack_usage)
{
    // We need to define character sets to specify service names.
    duck.defineArgsForCharset(*this);

    // Generic scrambling options.
    _scrambling.defineArgs(*this);

    option(u"", 0, STRING, 0, 1);
    help(u"",
         u"Specifies the optional service to descramble. If no fixed control word is "
         u"specified, ECM's from the service are used to extract control words.\n\n"
         u"If the argument is an integer value (either decimal or hexadecimal), it is interpreted as a service id. "
         u"If it is an empty string or \"-\", the first service in the PAT is descrambled. "
         u"Otherwise, it is interpreted as a service name, as specified in the SDT. "
         u"The name is not case sensitive and blanks are ignored. "
         u"If the input TS does not contain an SDT, use service ids only.\n\n"
         u"If the argument is omitted, --pid options shall be specified to list explicit "
         u"PID's to descramble and fixed control words shall be specified as well.");

    option(u"pid", 'p', PIDVAL, 0, UNLIMITED_COUNT);
    help(u"pid", u"pid1[-pid2]",
         u"Descramble packets with this PID value or range of PID values. "
         u"Several -p or --pid options may be specified. "
         u"By default, descramble the specified service.");

    option(u"synchronous");
    help(u"synchronous",
         u"Specify to synchronously decipher the ECM's. By default, in real-time "
         u"mode, the packet processing continues while processing ECM's. This option "
         u"is always on in offline mode.");

    option(u"swap-cw");
    help(u"swap-cw",
        u"Swap even and odd control words from the ECM's. "
        u"Useful when a crazy ECMG inadvertently swapped the CW before generating the ECM.");
}


//----------------------------------------------------------------------------
// Get options method
//----------------------------------------------------------------------------

bool ts::AbstractDescrambler::getOptions()
{
    // Load command line arguments.
    _use_service = present(u"");
    _service.set(value(u""));
    _synchronous = present(u"synchronous") || !tsp->realtime();
    _swap_cw = present(u"swap-cw");
    getIntValues(_pids, u"pid");
    if (!duck.loadArgs(*this) || !_scrambling.loadArgs(duck, *this)) {
        return false;
    }

    // Descramble either a service or a list of PID's, not a mixture of them.
    if ((_use_service + _pids.any()) != 1) {
        error(u"specify either a service or a list of PID's");
        return false;
    }

    // We need to decipher ECM's if we descramble a service without fixed control words.
    _need_ecm = _use_service && !_scrambling.hasFixedCW();

    // To descramble a fixed list of PID's, we need fixed control words.
    if (_pids.any() && !_scrambling.hasFixedCW()) {
        error(u"specify control words to descramble an explicit list of PID's");
        return false;
    }

    return true;
}


//----------------------------------------------------------------------------
// Get the ECM stream for a PID, create it if non existent
//----------------------------------------------------------------------------

ts::AbstractDescrambler::ECMStreamPtr ts::AbstractDescrambler::getOrCreateECMStream(PID ecm_pid)
{
    auto ecm_it = _ecm_streams.find(ecm_pid);
    if (ecm_it != _ecm_streams.end()) {
        return ecm_it->second;
    }
    else {
        ECMStreamPtr p(new ECMStream(this));
        _ecm_streams.insert(std::make_pair(ecm_pid, p));
        return p;
    }
}


//----------------------------------------------------------------------------
// Start method
//----------------------------------------------------------------------------

bool ts::AbstractDescrambler::start()
{
    // Reset descrambler state
    _abort = false;
    _ecm_streams.clear();
    _scrambled_streams.clear();
    _demux.reset();

    // Initialize the scrambling engine.
    if (!_scrambling.start()) {
        return false;
    }

    // In asynchronous mode, create a thread for ECM processing
    if (_need_ecm && !_synchronous) {
        _stop_thread = false;
        ThreadAttributes attr;
        _ecm_thread.getAttributes(attr);
        attr.setStackSize(ECM_THREAD_STACK_OVERHEAD + _stack_usage);
        _ecm_thread.setAttributes(attr);
        _ecm_thread.start();
    }

    return true;
}


//----------------------------------------------------------------------------
// Stop abstract descrambler.
//----------------------------------------------------------------------------

bool ts::AbstractDescrambler::stop()
{
    // In asynchronous mode, notify the ECM processing thread to terminate
    // and wait for its actual termination.
    if (_need_ecm && !_synchronous) {
        {
            std::lock_guard<std::mutex> lock(_mutex);
            _stop_thread = true;
            _ecm_to_do.notify_one();
        }
        _ecm_thread.waitForTermination();
    }

    _scrambling.stop();
    return true;
}


//----------------------------------------------------------------------------
//  This method is invoked when a PMT is available for the service.
//----------------------------------------------------------------------------

void ts::AbstractDescrambler::handlePMT(const PMT& pmt, PID)
{
    debug(u"PMT: service 0x%X, %d elementary streams", pmt.service_id, pmt.streams.size());

    // Default scrambling is DVB-CSA2.
    uint8_t scrambling_type = SCRAMBLING_DVB_CSA2;

    // Search ECM PID's at service level
    std::set<PID> service_ecm_pids;
    analyzeDescriptors(pmt.descs, service_ecm_pids, scrambling_type);

    // Loop on all elementary streams in this service.
    // Create an entry in _scrambled_streams for each of them.
    for (const auto& it : pmt.streams) {
        const PID pid = it.first;
        const PMT::Stream& pmt_stream(it.second);

        // Enforce an entry for this PID in _scrambled_streams, even no valid ECM PID is found
        // (maybe we don't need ECM at all). But the PID must be marked as potentially scrambled.
        ScrambledStream& scr_stream(_scrambled_streams[pid]);

        // Search ECM PIDs at elementary stream level.
        std::set<PID> component_ecm_pids;
        analyzeDescriptors(pmt_stream.descs, component_ecm_pids, scrambling_type);

        // If none found as stream level, use the ones from service level.
        if (!component_ecm_pids.empty()) {
            // Valid ECM PID's found at component level, use them.
            scr_stream.ecm_pids = std::move(component_ecm_pids);
        }
        else if (!service_ecm_pids.empty()) {
            // Valid ECM PID's found at service level, use them.
            scr_stream.ecm_pids = service_ecm_pids;
        }
    }

    // Set global scrambling type from scrambling descriptor, if not specified on the command line.
    _scrambling.setScramblingType(scrambling_type, false);
    verbose(u"using scrambling mode: %s", NameFromSection(u"dtv", u"ScramblingMode", _scrambling.scramblingType()));
    for (auto& it : _ecm_streams) {
        it.second->scrambling.setScramblingType(scrambling_type, false);
    }
}


//----------------------------------------------------------------------------
// Analyze a list of descriptors, looking for ECM PID's
//----------------------------------------------------------------------------

void ts::AbstractDescrambler::analyzeDescriptors(const DescriptorList& dlist, std::set<PID>& ecm_pids, uint8_t& scrambling)
{
    // Loop on all descriptors
    for (size_t index = 0; index < dlist.count(); ++index) {

        // Descriptor payload
        const uint8_t* desc = dlist[index].payload();
        size_t size = dlist[index].payloadSize();

        switch (dlist[index].tag()) {
            case DID_MPEG_CA: {
                    // The fixed part of a CA descriptor is 4 bytes long.
                    // Ignore CA descriptors if we do not need ECM's.
                    if (_need_ecm && size >= 4) {
                        const uint16_t sysid = GetUInt16(desc);
                        const uint16_t pid = GetUInt16(desc + 2) & 0x1FFF;

                        // Ask subclass if this PID is OK
                        if (checkCADescriptor(sysid, ByteBlock(desc + 4, size - 4))) {
                            verbose(u"using ECM PID %n", pid);
                            // Create context for this ECM stream.
                            ecm_pids.insert(pid);
                            getOrCreateECMStream(pid);
                            // Ask the demux to notify us of ECM's in this PID.
                            _demux.addPID(pid);
                        }
                    }
                    break;
                }
            case DID_DVB_SCRAMBLING: {
                    // A scrambling descriptor contains one byte.
                    if (size >= 1) {
                        scrambling = desc[0];
                    }
                    break;
                }
            default: {
                    break;
                }
        }
    }
}


//----------------------------------------------------------------------------
// Invoked by the demux when a section is available in an ECM PID.
//----------------------------------------------------------------------------

void ts::AbstractDescrambler::handleSection(SectionDemux& demux, const Section& sect)
{
    const PID ecm_pid = sect.sourcePID();
    log(2, u"got ECM (TID 0x%X) on PID %n", sect.tableId(), ecm_pid);

    // Get ECM stream context
    auto ecm_it = _ecm_streams.find(ecm_pid);
    if (ecm_it == _ecm_streams.end()) {
        warning(u"got ECM on non-ECM PID %n", ecm_pid);
        return;
    }
    ECMStreamPtr& estream(ecm_it->second);

    // If same TID as previous CMT on this PID, give up, this is the same ECM.
    if (sect.tableId() == estream->last_tid) {
        return;
    }

    // This is a new ECM on this PID.
    estream->last_tid = sect.tableId();

    // Check if the ECM can be deciphered (ask subclass)
    if (!checkECM(sect)) {
        log(2, u"ECM not handled by subclass");
        return;
    }
    debug(u"new ECM (TID 0x%X) on PID %n", sect.tableId(), ecm_pid);

    // In asynchronous mode, the CW are accessed under mutex protection.
    if (!_synchronous) {
        _mutex.lock();
    }

    // Copy the ECM into the PID context.
    estream->ecm.copy(sect);
    estream->new_ecm = true;

    // Decipher the ECM.
    if (_synchronous) {
        // Synchronous mode: directly decipher the ECM
        processECM(*estream);
    }
    else {
        // Asynchronous mode: signal the ECM to the ECM processing thread.
        _ecm_to_do.notify_one();
        _mutex.unlock();
    }
}


//----------------------------------------------------------------------------
// Process one ECM (the one in ECMStream::ecm).
// In asynchronous mode, this method must be invoked with the mutex held.
// Release the mutex while deciphering the ECM and relock it before exiting.
//----------------------------------------------------------------------------

void ts::AbstractDescrambler::processECM(ECMStream& estream)
{
    // Copy the ECM out of the protected area into local data
    Section ecm(estream.ecm, ShareMode::COPY);
    estream.new_ecm = false;

    // Local data for deciphered CW's from ECM.
    CWData cw_even(estream.scrambling.scramblingType());
    CWData cw_odd(estream.scrambling.scramblingType());

    // In asynchronous mode, release the mutex.
    if (!_synchronous) {
        _mutex.unlock();
    }

    // Here, we have an ECM to decipher.
    const size_t dumpSize = std::min<size_t>(8, ecm.payloadSize());
    if (debug()) {
        debug(u"packet %d, decipher ECM, %d bytes: %s%s",
              tsp->pluginPackets(),
              ecm.payloadSize(),
              UString::Dump(ecm.payload(), dumpSize, UString::SINGLE_LINE),
              dumpSize < ecm.payloadSize() ? u" ..." : u"");
    }

    // Submit the ECM to the CAS (subclass).
    // Exchange the control words if CW swapping was requested.
    bool ok = decipherECM(ecm, _swap_cw ? cw_odd : cw_even, _swap_cw ? cw_even : cw_odd);

    if (ok) {
        debug(u"even CW: %s", UString::Dump(cw_even.cw, UString::SINGLE_LINE));
        debug(u"odd CW:  %s", UString::Dump(cw_odd.cw, UString::SINGLE_LINE));
    }

    // In asynchronous mode, relock the mutex.
    if (!_synchronous) {
        _mutex.lock();
    }

    // Copy the control words in the protected area.
    // Normally, only one CW is modified for each new ECM.
    // Compare extracted CW with previous ones to avoid signaling a new
    // CW when it is actually unchanged.
    if (ok) {
        if (!estream.cw_valid || estream.cw_even.cw != cw_even.cw) {
            // Previous even CW was either invalid or different from new one
            estream.new_cw_even = true;
            estream.cw_even = std::move(cw_even);
        }
        if (!estream.cw_valid || estream.cw_odd.cw != cw_odd.cw) {
            // Previous odd CW was either invalid or different from new one
            estream.new_cw_odd = true;
            estream.cw_odd = std::move(cw_odd);
        }
        estream.cw_valid = ok;
    }
}


//----------------------------------------------------------------------------
// ECM deciphering thread
//----------------------------------------------------------------------------

void ts::AbstractDescrambler::ECMThread::main()
{
    _parent->debug(u"ECM processing thread started");

    // ECM processing loop.
    // The loop executes with the mutex held. The mutex is released
    // while deciphering an ECM and while waiting for the condition
    // variable 'ecm_to_do'.
    std::unique_lock<std::mutex> lock(_parent->_mutex);

    for (;;) {

        // Look for an ECM to decipher. Loop as long as ECM's are
        // found (or a terminate request is encountered). We loop
        // again if at least an ECM was found since the mutex was
        // released during the ECM processing and a new ECM may
        // have been added at the beginning of the list.
        bool got_ecm = false;
        bool terminate = false;

        do {
            got_ecm = false;
            terminate = _parent->_stop_thread;

            // Decipher ECM's on all ECM PID's.
            for (auto it = _parent->_ecm_streams.begin(); !terminate && it != _parent->_ecm_streams.end(); ++it) {
                ECMStreamPtr& estream(it->second);
                if (estream->new_ecm) {
                    // Found an ECM, decipher it. Note that the mutex is
                    // released while deciphering the ECM.
                    got_ecm = true;
                    _parent->processECM(*estream);

                    // Look for termination request while deciphering
                    terminate = _parent->_stop_thread;
                }
            }
        } while (!terminate && got_ecm);

        // Check if a terminate request is found
        if (terminate) {
            break;
        }

        // We have accomplished a full scan of all ECM PID's and found no ECM.
        // The mutex was consequently not released during the loop and we are
        // now sure that there is nothing to do. The mutex is implicitely
        // released and we wait for the condition 'ecm_to_do' and, once we
        // get it, implicitely relock the mutex.
        _parent->_ecm_to_do.wait(lock);
    }

    _parent->debug(u"ECM processing thread terminated");
}


//----------------------------------------------------------------------------
// Packet processing method
//----------------------------------------------------------------------------

ts::ProcessorPlugin::Status ts::AbstractDescrambler::processPacket(TSPacket& pkt, TSPacketMetadata& pkt_data)
{
    const PID pid = pkt.getPID();

    // Descramble packets from fixed PID's using fixed control words.
    // If there is a user-specified list of PID's, we don't manage a service
    // and there is nothing else to do.
    if (_pids.any()) {
        return !_pids.test(pid) || _scrambling.decrypt(pkt) ? TSP_OK : TSP_END;
    }

    // Filter sections to locate the service and grab ECM's.
    _service.feedPacket(pkt);
    _demux.feedPacket(pkt);

    // If the service is definitely unknown or a fatal error occured during table analysis, give up.
    if (_abort || _service.nonExistentService()) {
        return TSP_END;
    }

    // Get scrambling_control_value in packet.
    uint8_t scv = pkt.getScrambling();

    // If the packet has no payload or is clear, there is nothing to descramble.
    if (!pkt.hasPayload() || (scv != SC_EVEN_KEY && scv != SC_ODD_KEY)) {
        return TSP_OK;
    }

    // Without ECM's, we descramble using fixed control words.
    if (!_need_ecm) {
        return _scrambling.decrypt(pkt) ? TSP_OK : TSP_END;
    }

    // Get PID context. If the PID is not known as a scrambled PID,
    // with a corresponding ECM stream, we cannot descramble it.
    auto ssit = _scrambled_streams.find(pid);
    if (ssit == _scrambled_streams.end()) {
        return TSP_OK;
    }
    ScrambledStream& ss(ssit->second);

    // Locate an ECM stream with a currently valid pair of CW.
    ECMStreamPtr pecm;
    for (auto it = ss.ecm_pids.begin(); pecm == nullptr && it != ss.ecm_pids.end(); ++it) {
        pecm = getOrCreateECMStream(*it);
        // Flag cw_valid is "write-protected, read-volatile", no mutex needed.
        if (!pecm->cw_valid) {
            pecm.reset();
        }
    }
    if (pecm == nullptr) {
        // No ECM stream has valid Control Word now, cannot descramble
        return TSP_OK;
    }

    // We found a valid CW, check if new CW were deciphered and store them in the descrambler.
    // Flags new_cw_even/odd are "write-protected, read-volatile", no mutex needed.
    if ((scv == SC_EVEN_KEY && pecm->new_cw_even) || (scv == SC_ODD_KEY && pecm->new_cw_odd)) {

        // A new CW was deciphered.
        // In asynchronous mode, the CW are accessed under mutex protection.
        if (!_synchronous) {
            _mutex.lock();
        }

        // Store the new CW in the descrambler.
        if (scv == SC_EVEN_KEY) {
            pecm->scrambling.setScramblingType(pecm->cw_even.scrambling, false);
            pecm->scrambling.setCW(pecm->cw_even.cw, SC_EVEN_KEY);
            pecm->new_cw_even = false;
        }
        else {
            pecm->scrambling.setScramblingType(pecm->cw_odd.scrambling, false);
            pecm->scrambling.setCW(pecm->cw_odd.cw, SC_ODD_KEY);
            pecm->new_cw_odd = false;
        }

        if (!_synchronous) {
            _mutex.unlock();
        }
    }

    // Descramble the packet payload.
    return pecm->scrambling.decrypt(pkt) ? TSP_OK : TSP_END;
}
