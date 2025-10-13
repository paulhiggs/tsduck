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
    //!
    //! Representation of a UUID
    //!
    //! @see IETF RFC 4122 "A Universally Unique IDentifier (UUID) URN Namespace"
    //! @ingroup libtsduck datatype
    //!
    //!
    class UUID
    {
    private:
        uint64_t hi = 0;  //!< most significant bits of the UUID
        uint64_t lo = 0;  //!< least significant bits of the UUID

    public:
        //!
        //! Default constructor.
        //!
        UUID() = default;

        //!
        //! Initialising constructor
        //! 
        UUID(uint64_t _hi, uint64_t _lo);

        //!
        //! Bitstream deserialising constructor
        //! 
        UUID(PSIBuffer& buf);

        void deserialize(PSIBuffer& buf);
        void serialize(PSIBuffer& buf);

        //!
        //! Convert the binary (128-bit) representation to the hyphen seperated UUID format
        //! 
        UString format();

        //!
        //! Read the hyphen seperated UUID format into the binary (128-bit) representation
        //! 
        bool parse(UString uuid);
    };
}  // namespace ts
