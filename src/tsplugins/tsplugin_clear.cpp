//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2025, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------
//
//  Transport stream processor shared library:
//  Extract clear (non scrambled) sequence of a transport stream
//
//----------------------------------------------------------------------------

#include "tsPluginRepository.h"
#include "tsService.h"
#include "tsSectionDemux.h"
#include "tsBinaryTable.h"
#include "tsPAT.h"
#include "tsPMT.h"
#include "tsSDT.h"
#include "tsTOT.h"


//----------------------------------------------------------------------------
// Plugin definition
//----------------------------------------------------------------------------

namespace ts {
    class ClearPlugin: public ProcessorPlugin, private TableHandlerInterface
    {
        TS_PLUGIN_CONSTRUCTORS(ClearPlugin);
    public:
        // Implementation of plugin API
        virtual bool start() override;
        virtual Status processPacket(TSPacket&, TSPacketMetadata&) override;

    private:
        bool          _abort = false;         // Error (service not found, etc)
        Service       _service {};            // Service name & id
        bool          _pass_packets = false;  // Pass packets trigger
        Status        _drop_status = TSP_OK;  // Status for dropped packets
        bool          _video_only = false;    // Check video PIDs only
        bool          _audio_only = false;    // Check audio PIDs only
        TOT           _last_tot {};           // Last received TOT
        PacketCounter _drop_after = 0;        // Number of packets after last clear
        PacketCounter _last_clear_pkt = 0;    // Last clear packet number
        PIDSet        _clear_pids {};         // List of PIDs to check for clear packets
        SectionDemux  _demux {duck, this};    // Section demux

        // Invoked by the demux when a complete table is available.
        virtual void handleTable (SectionDemux&, const BinaryTable&) override;

        // Process specific tables
        void processPAT(PAT&);
        void processPMT(PMT&);
        void processSDT(SDT&);
    };
}

TS_REGISTER_PROCESSOR_PLUGIN(u"clear", ts::ClearPlugin);


//----------------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------------

ts::ClearPlugin::ClearPlugin(TSP* tsp_) :
    ProcessorPlugin(tsp_, u"Extract clear (non scrambled) sequences of a transport stream", u"[options]")
{
    // We need to define character sets to specify service names.
    duck.defineArgsForCharset(*this);

    option(u"audio", 'a');
    help(u"audio",
         u"Check only audio PIDs for clear packets. By default, audio and video "
         u"PIDs are checked.");

    option(u"drop-after-packets", 'd', POSITIVE);
    help(u"drop-after-packets",
         u"Specifies the number of packets after the last clear packet to wait "
         u"before stopping the packet transmission. By default, stop 1 second "
         u"after the last clear packet (based on current bitrate).");

    option(u"service", 's', STRING);
    help(u"service",
         u"The extraction of clear sequences is based on one \"reference\" service. "
         u"(see option -s). When a clear packet is found on any audio or video stream of "
         u"the reference service, all packets in the TS are transmitted. When no clear "
         u"packet has been found in the last second, all packets in the TS are dropped.\n\n"
         u"This option specifies the reference service. If the argument is an integer value "
         u"(either decimal or hexadecimal), it is interpreted as a service id. "
         u"Otherwise, it is interpreted as a service name, as specified in the "
         u"SDT. The name is not case sensitive and blanks are ignored. If this "
         u"option is not specified, the first service in the PAT is used.");

    option(u"stuffing");
    help(u"stuffing",
         u"Replace excluded packets with stuffing (null packets) instead "
         u"of removing them. Useful to preserve bitrate.");

    option(u"video", 'v');
    help(u"video",
         u"Check only video PIDs for clear packets. By default, audio and video "
         u"PIDs are checked.");
}


//----------------------------------------------------------------------------
// Start method
//----------------------------------------------------------------------------

bool ts::ClearPlugin::start()
{
    // Get option values
    duck.loadArgs(*this);
    _service.set (value(u"service"));
    _video_only = present(u"video");
    _audio_only = present(u"audio");
    _drop_status = present(u"stuffing") ? TSP_NULL : TSP_DROP;
    _drop_after = intValue<PacketCounter>(u"drop-after-packets", 0);

    // Initialize the demux. Filter the TOT to get timestamps.
    // If the service is known by name, filter the SDT, otherwise filter the PAT.
    _demux.reset();
    _demux.addPID(PID_TOT);
    _demux.addPID(PID(_service.hasName() ? PID_SDT : PID_PAT));

    // Reset other states
    _abort = false;
    _pass_packets = false; // initially drop packets
    _last_tot.invalidate();
    _last_clear_pkt = 0;
    _clear_pids.reset();

    return true;
}


//----------------------------------------------------------------------------
// Invoked by the demux when a complete table is available.
//----------------------------------------------------------------------------

void ts::ClearPlugin::handleTable(SectionDemux& demux, const BinaryTable& table)
{
    switch (table.tableId()) {

        case TID_PAT: {
            if (table.sourcePID() == PID_PAT) {
                PAT pat(duck, table);
                if (pat.isValid()) {
                    processPAT (pat);
                }
            }
            break;
        }

        case TID_SDT_ACT: {
            if (table.sourcePID() == PID_SDT) {
                SDT sdt(duck, table);
                if (sdt.isValid()) {
                    processSDT (sdt);
                }
            }
            break;
        }

        case TID_PMT: {
            PMT pmt(duck, table);
            if (pmt.isValid() && _service.hasId (pmt.service_id)) {
                processPMT (pmt);
            }
            break;
        }

        case TID_TOT: {
            if (table.sourcePID() == PID_TOT) {
                // Save last TOT
                _last_tot.deserialize(duck, table);
            }
            break;
        }

        default: {
            break;
        }
    }
}


//----------------------------------------------------------------------------
//  This method processes a Service Description Table (SDT).
//----------------------------------------------------------------------------

void ts::ClearPlugin::processSDT(SDT& sdt)
{
    // Look for the service by name
    uint16_t service_id;
    assert (_service.hasName());
    if (!sdt.findService(duck, _service.getName(), service_id)) {
        error(u"service \"%s\" not found in SDT", _service.getName());
        _abort = true;
        return;
    }

    // Remember service id
    _service.setId(service_id);
    verbose(u"found service \"%s\", service id is 0x%X", _service.getName(), _service.getId());

    // No longer need to filter the SDT
    _demux.removePID(PID_SDT);

    // Now filter the PAT to get the PMT PID
    _demux.addPID(PID_PAT);
    _service.clearPMTPID();
}


//----------------------------------------------------------------------------
//  This method processes a Program Association Table (PAT).
//----------------------------------------------------------------------------

void ts::ClearPlugin::processPAT(PAT& pat)
{
    if (_service.hasId()) {
        // The service id is known, search it in the PAT
        const auto it = pat.pmts.find(_service.getId());
        if (it == pat.pmts.end()) {
            // Service not found, error
            error(u"service id %n not found in PAT", _service.getId());
            _abort = true;
            return;
        }
        // If a previous PMT PID was known, no long filter it
        if (_service.hasPMTPID()) {
            _demux.removePID (_service.getPMTPID());
        }
        // Found PMT PID
        _service.setPMTPID(it->second);
        _demux.addPID(it->second);
    }
    else if (!pat.pmts.empty()) {
        // No service specified, use first one in PAT
        const auto it = pat.pmts.begin();
        _service.setId(it->first);
        _service.setPMTPID(it->second);
        _demux.addPID(it->second);
        verbose(u"using service %n", _service.getId());
    }
    else {
        // No service specified, no service in PAT, error
        error(u"no service in PAT");
        _abort = true;
    }
}


//----------------------------------------------------------------------------
//  This method processes a Program Map Table (PMT).
//----------------------------------------------------------------------------

void ts::ClearPlugin::processPMT(PMT& pmt)
{
    // Collect all audio/video PIDS
    _clear_pids.reset();
    for (const auto& it : pmt.streams) {
        const PID pid = it.first;
        const PMT::Stream& stream(it.second);
        if ((stream.isAudio(duck) && !_video_only) || (stream.isVideo(duck) && !_audio_only)) {
            _clear_pids.set(pid);
        }
    }
}


//----------------------------------------------------------------------------
// Packet processing method
//----------------------------------------------------------------------------

ts::ProcessorPlugin::Status ts::ClearPlugin::processPacket(TSPacket& pkt, TSPacketMetadata& pkt_data)
{
    const PID pid = pkt.getPID();
    bool previous_pass = _pass_packets;

    // Filter interesting sections
    _demux.feedPacket (pkt);

    // If a fatal error occured during section analysis, give up.
    if (_abort) {
        return TSP_END;
    }

    // If this is a clear packet from an audio/video PID of the reference service, let the packets pass.
    if (_clear_pids[pid] && pkt.isClear()) {
        _pass_packets = true;
        _last_clear_pkt = tsp->pluginPackets();
    }

    // Make sure we know how long to wait after the last clear packet
    if (_drop_after == 0) {
        // Number of packets in 1 second at current bitrate
        _drop_after = (tsp->bitrate() / PKT_SIZE_BITS).toInt();
        if (_drop_after == 0) {
            error(u"bitrate unknown or too low, use option --drop-after-packets");
            return TSP_END;
        }
        verbose(u"will drop %'d packets after last clear packet", _drop_after);
    }

    // If packets are passing but no clear packet recently found, drop packets
    if (_pass_packets && (tsp->pluginPackets() - _last_clear_pkt) > _drop_after) {
        _pass_packets = false;
    }

    // Report state change in verbose mode
    if (_pass_packets != previous_pass && verbose()) {
        // State has changed
        const UString curtime(_last_tot.isValid() && !_last_tot.regions.empty() ?
                              _last_tot.localTime(_last_tot.regions[0]).format(Time::DATETIME) :
                              u"unknown");
        verbose(u"now %s all packets, last TOT local time: %s, current packet: %'d", _pass_packets ? u"passing" : u"dropping", curtime, tsp->pluginPackets());
    }

    // Pass or drop the packets
    return _pass_packets ? TSP_OK : _drop_status;
}
