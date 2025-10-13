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
        uint64_t hi_bits = 0;  //!< most significant bits of the UUID
        uint64_t lo_bits = 0;  //!< least significant bits of the UUID

    public:
        //!
        //! Default constructor.
        //!
        UUID() = default;

        //!
        //! Initialising constructor
        //! @param [in] hi Most significant bits of the UUID
        //! @param [in] lo Least significant order 
        //! 
        UUID(uint64_t hi, uint64_t lo);

        //!
        //! Bitstream deserialising constructor
        //! @param [in,out] buf Deserialization buffer. See definition in AbstractDescriptor::serializePayload
        //! 
        UUID(PSIBuffer& buf);

        //!
        //! Deserialize (read) the UUID from the buffer
        //! @param [in,out] buf Deserialization buffer. See definition in AbstractDescriptor::serializePayload
        //!
        void deserialize(PSIBuffer& buf);

        //!
        //! Serialize (write) the UUID to the buffer
        //! @param [in,out] buf Serialization buffer. See definition in AbstractDescriptor::serializePayload
        //! 
        void serialize(PSIBuffer& buf);

        //!
        //! Convert the binary (128-bit) representation to the hyphen seperated UUID format
        //! @return A string representation (per RFC 4122) of the UUID
        //! 
        UString format();

        //!
        //! Read the hyphen seperated UUID format into the binary (128-bit) representation
        //! @param [in] uuid String representation of a UUID, hyphen seperated string from RFC 4122
        //! @return True if the provided UUID is in the correct format and has been parsed into the object, otherwise false
        //! 
        bool parse(UString uuid);
    };
}  // namespace ts
