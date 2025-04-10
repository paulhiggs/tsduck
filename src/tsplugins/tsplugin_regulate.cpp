//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2025, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------
//
//  Transport stream processor shared library:
//  Regulate (slow down) the packet flow according to a bitrate.
//
//----------------------------------------------------------------------------

#include "tsPluginRepository.h"
#include "tsBitRateRegulator.h"
#include "tsPCRRegulator.h"

#define DEF_PACKET_BURST 16


//----------------------------------------------------------------------------
// Plugin definition
//----------------------------------------------------------------------------

namespace ts {
    class RegulatePlugin: public ProcessorPlugin
    {
        TS_PLUGIN_CONSTRUCTORS(RegulatePlugin);
    public:
        // Implementation of plugin API
        virtual bool getOptions() override;
        virtual bool start() override;
        virtual bool isRealTime() override {return true;}
        virtual Status processPacket(TSPacket&, TSPacketMetadata&) override;

    private:
        // Command line options:
        bool             _pcr_synchronous = false;
        BitRate          _bitrate = 0;
        PacketCounter    _burst = 0;
        cn::milliseconds _wait_min {};
        PID              _pid_pcr = PID_NULL;

        // Working data:
        BitRateRegulator _bitrate_regulator {this, Severity::Verbose};
        PCRRegulator     _pcr_regulator {this, Severity::Verbose};
    };
}

TS_REGISTER_PROCESSOR_PLUGIN(u"regulate", ts::RegulatePlugin);


//----------------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------------

ts::RegulatePlugin::RegulatePlugin(TSP* tsp_) :
    ProcessorPlugin(tsp_, u"Regulate the TS packets flow based on PCR or bitrate", u"[options]")
{
    option<BitRate>(u"bitrate", 'b');
    help(u"bitrate",
         u"Specify a bitrate in b/s and regulate (slow down only) the TS packets "
         u"flow according to this bitrate. By default, use the \"input\" bitrate, "
         u"typically resulting from the PCR analysis of the input file.");

    option(u"packet-burst", 'p', POSITIVE);
    help(u"packet-burst",
         u"Number of packets to burst at a time. Does not modify the average "
         u"output bitrate but influence smoothing and CPU load. The default "
         u"is " TS_STRINGIFY(DEF_PACKET_BURST) u" packets.");

    option(u"pcr-synchronous");
    help(u"pcr-synchronous",
         u"Regulate the flow based on the Program Clock Reference from the transport "
         u"stream. By default, use a bitrate, not PCR's.");

    option(u"pid-pcr", 0, PIDVAL);
    help(u"pid-pcr",
         u"With --pcr-synchronous, specify the reference PID for PCR's. By default, "
         u"use the first PID containing PCR's.");

    option<cn::milliseconds>(u"wait-min", 'w');
    help(u"wait-min",
         u"With --pcr-synchronous, specify the minimum wait time in milli-seconds. "
         u"The default is " + UString::Chrono(PCRRegulator::DEFAULT_MIN_WAIT) + u"");
}


//----------------------------------------------------------------------------
// Get command line options.
//----------------------------------------------------------------------------

bool ts::RegulatePlugin::getOptions()
{
    getValue(_bitrate, u"bitrate", 0);
    getIntValue(_burst, u"packet-burst", DEF_PACKET_BURST);
    getChronoValue(_wait_min, u"wait-min", PCRRegulator::DEFAULT_MIN_WAIT);
    getIntValue(_pid_pcr, u"pid-pcr", PID_NULL);
    _pcr_synchronous = present(u"pcr-synchronous");

    if (present(u"bitrate") && _pcr_synchronous) {
        error(u"--bitrate cannot be used with --pcr-synchronous");
        return false;
    }
    if (present(u"pid-pcr") && !_pcr_synchronous) {
        error(u"--pid-pcr cannot be used without --pcr-synchronous");
        return false;
    }
    return true;
}


//----------------------------------------------------------------------------
// Start method
//----------------------------------------------------------------------------

bool ts::RegulatePlugin::start()
{
    // Initialize the appropriate regulator.
    if (_pcr_synchronous) {
        debug(u"starting PCR-synchronous regulation");
        _pcr_regulator.reset();
        _pcr_regulator.setBurstPacketCount(_burst);
        _pcr_regulator.setReferencePID(_pid_pcr);
        _pcr_regulator.setMinimimWait(_wait_min);
    }
    else {
        debug(u"starting bitrate-based regulation");
        _bitrate_regulator.setBurstPacketCount(_burst);
        _bitrate_regulator.setFixedBitRate(_bitrate);
        _bitrate_regulator.start();
    }
    return true;
}


//----------------------------------------------------------------------------
// Packet processing method
//----------------------------------------------------------------------------

ts::ProcessorPlugin::Status ts::RegulatePlugin::processPacket(TSPacket& pkt, TSPacketMetadata& pkt_data)
{
    bool flush = false;
    bool bitrate_changed = false;

    if (_pcr_synchronous) {
        flush = _pcr_regulator.regulate(pkt);
    }
    else {
        _bitrate_regulator.regulate(tsp->bitrate(), flush, bitrate_changed);
    }

    pkt_data.setFlush(flush);
    pkt_data.setBitrateChanged(bitrate_changed);
    return TSP_OK;
}
