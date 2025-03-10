//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2025, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------
//!
//!  @file
//!  Display PSI/SI information plugin for tsp.
//!
//----------------------------------------------------------------------------

#pragma once
#include "tsProcessorPlugin.h"
#include "tsPSILogger.h"
#include "tsTablesDisplay.h"

namespace ts {
    //!
    //! Display PSI/SI information plugin for tsp.
    //! @ingroup libtsduck plugin
    //!
    class TSDUCKDLL PSIPlugin : public ProcessorPlugin, private SectionHandlerInterface
    {
        TS_PLUGIN_CONSTRUCTORS(PSIPlugin);
    public:
        // Implementation of plugin API
        virtual bool getOptions() override;
        virtual bool start() override;
        virtual bool stop() override;
        virtual Status processPacket(TSPacket&, TSPacketMetadata&) override;

    private:
        TablesDisplay _display {duck};
        PSILogger     _logger {_display};
        bool          _signal_event = false;  // Signal a plugin event on section.
        uint32_t      _event_code = 0;        // Event code to signal.

        // Implementation of SectionHandlerInterface
        virtual void handleSection(SectionDemux& demux, const Section& section) override;
    };
}
