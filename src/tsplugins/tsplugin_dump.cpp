//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2025, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------
//
//  Dump transport stream packets.
//
//----------------------------------------------------------------------------

#include "tsPluginRepository.h"
#include "tsTSDumpArgs.h"


//----------------------------------------------------------------------------
// Plugin definition
//----------------------------------------------------------------------------

namespace ts {
    class DumpPlugin: public ProcessorPlugin
    {
        TS_PLUGIN_CONSTRUCTORS(DumpPlugin);
    public:
        // Implementation of plugin API
        virtual bool getOptions() override;
        virtual bool start() override;
        virtual bool stop() override;
        virtual Status processPacket(TSPacket&, TSPacketMetadata&) override;

    private:
        // Command line options:
        TSDumpArgs _dump {};
        fs::path   _outname {};

        // Working data.
        std::ofstream _outfile {};
        std::ostream* _out = &std::cout;
        bool          _add_endline = false;
    };
}

TS_REGISTER_PROCESSOR_PLUGIN(u"dump", ts::DumpPlugin);


//----------------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------------

ts::DumpPlugin::DumpPlugin(TSP* tsp_) :
    ProcessorPlugin(tsp_, u"Dump transport stream packets", u"[options]")
{
    duck.defineArgsForStandards(*this);
    _dump.defineArgs(*this);

    option(u"output-file", 'o', FILENAME);
    help(u"output-file", u"Output file for dumped packets. By default, use the standard output.");
}


//----------------------------------------------------------------------------
// Get options method
//----------------------------------------------------------------------------

bool ts::DumpPlugin::getOptions()
{
    bool ok = _dump.loadArgs(duck, *this) && duck.loadArgs(*this);
    getPathValue(_outname, u"output-file");

    if (_dump.log && !_outname.empty()) {
        error(u"--log and --output-file are mutually exclusive");
        ok = false;
    }
    return ok;
}


//----------------------------------------------------------------------------
// Start method
//----------------------------------------------------------------------------

bool ts::DumpPlugin::start()
{
    if (_outname.empty()) {
        _out = &std::cout;
    }
    else {
        _outfile.open(_outname);
        if (!_outfile) {
            error(u"error creating output file %s", _outname);
            return false;
        }
        _out = &_outfile;
    }
    _add_endline = false;
    return true;
}


//----------------------------------------------------------------------------
// Stop method
//----------------------------------------------------------------------------

bool ts::DumpPlugin::stop()
{
    if (_add_endline) {
        (*_out) << std::endl;
    }
    if (_outfile.is_open()) {
        _outfile.close();
    }
    return true;
}


//----------------------------------------------------------------------------
// Packet processing method
//----------------------------------------------------------------------------

ts::ProcessorPlugin::Status ts::DumpPlugin::processPacket(TSPacket& pkt, TSPacketMetadata& pkt_data)
{
    if (_dump.pids.test(pkt.getPID())) {
        if (_dump.log) {
            std::ostringstream strm;
            _dump.dump(duck, strm, pkt, &pkt_data);
            UString str;
            str.assignFromUTF8(strm.str());
            str.trim();
            info(str);
        }
        else {
            (*_out) << std::endl << "* Packet " << ts::UString::Decimal(tsp->pluginPackets()) << std::endl;
            _dump.dump(duck, *_out, pkt, &pkt_data);
            _add_endline = true;
        }
    }
    return TSP_OK;
}
