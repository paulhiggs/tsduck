//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2023, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------
//!
//!  @file
//!  Memory input plugin for tsp.
//!
//----------------------------------------------------------------------------

#pragma once
#include "tsInputPlugin.h"

namespace ts {
    //!
    //! Memory input plugin for tsp.
    //! @ingroup plugin
    //!
    class TSDUCKDLL MemoryInputPlugin: public InputPlugin
    {
        TS_NOBUILD_NOCOPY(MemoryInputPlugin);
    public:
        //!
        //! Constructor.
        //! @param [in] tsp Associated callback to @c tsp executable.
        //!
        MemoryInputPlugin(TSP* tsp);

        // Implementation of plugin API
        virtual bool getOptions() override;
        virtual size_t receive(TSPacket*, TSPacketMetadata*, size_t) override;

    private:
        uint32_t _event_code;
    };
}
