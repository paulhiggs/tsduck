//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2025, Paul Higgs
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------
//!
//!  @file
//!  Representation of a UUID for PSI serialization.
//!
//----------------------------------------------------------------------------

#pragma once
#include "tsPSIBuffer.h"

namespace ts {
    class UUID
    {
    private:
        uint64_t hi = 0, lo = 0;

    public:
        UUID() = default;
        UUID(uint64_t, uint64_t);
        UUID(PSIBuffer&);

        void deserialize(PSIBuffer&);
        void serialize(PSIBuffer&);

        UString format();
        bool parse(UString);
    };
}  // namespace ts
