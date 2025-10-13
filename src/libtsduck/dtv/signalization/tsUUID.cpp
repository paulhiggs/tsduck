//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2025, Paul Higgs
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------

#include "tsUUID.h"
#include "tsUString.h"

//----------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------

ts::UUID::UUID(uint64_t hi, uint64_t lo)
{
    hi_bits = hi;
    lo_bits = lo;
}

ts::UUID::UUID(PSIBuffer& buf)
{
    deserialize(buf);
}

void ts::UUID::deserialize(PSIBuffer& buf)
{
    hi_bits = buf.getUInt64();
    lo_bits = buf.getUInt64();
}

void ts::UUID::serialize(PSIBuffer& buf)
{
    buf.putUInt64(hi_bits);
    buf.putUInt64(lo_bits);
};


//----------------------------------------------------------------------------
// Formatter
//----------------------------------------------------------------------------

// UUID formats
// aabbccdd-eeff-0011-2233-66778899aabb
// ..........11111111112222222222333333
// 012345678901234567890123456789012345

ts::UString ts::UUID::format()
{
    char buf[48];
    snprintf(buf, 48, "%08X-%04X-%04X-%04X-%04X%08X",
             uint32_t((hi_bits & 0xffffffff00000000) >> 32),
             uint16_t((hi_bits & 0x00000000ffff0000) >> 16),
             uint16_t((hi_bits & 0x000000000000ffff)),
             uint16_t((lo_bits & 0xffff000000000000) >> 48),
             uint16_t((lo_bits & 0x0000ffff00000000) >> 32),
             uint32_t((lo_bits & 0x00000000ffffffff)));
    return UString(buf);
}

//----------------------------------------------------------------------------
// Parser
//----------------------------------------------------------------------------

bool ts::UUID::parse(UString uuid)
{
    hi_bits = 0;
    lo_bits = 0;
    if (uuid.length() != 36)
        return false;
    std::string s = uuid.toUTF8();
    hi_bits = (std::stoll(s.substr(0, 8), nullptr, 16) << 32) + (std::stoll(s.substr(9, 4), nullptr, 16) << 16) + (std::stoll(s.substr(14, 4), nullptr, 16));
    lo_bits = (std::stoll(s.substr(19, 4), nullptr, 16) << 48) + (std::stoll(s.substr(24, 12), nullptr, 16));
    return true;
}
