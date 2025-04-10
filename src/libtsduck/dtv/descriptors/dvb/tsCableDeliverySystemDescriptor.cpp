//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2025, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------

#include "tsCableDeliverySystemDescriptor.h"
#include "tsDescriptor.h"
#include "tsTablesDisplay.h"
#include "tsPSIRepository.h"
#include "tsPSIBuffer.h"
#include "tsDuckContext.h"
#include "tsxmlElement.h"

#define MY_XML_NAME u"cable_delivery_system_descriptor"
#define MY_CLASS    ts::CableDeliverySystemDescriptor
#define MY_EDID     ts::EDID::Regular(ts::DID_DVB_CABLE_DELIVERY, ts::Standards::DVB)

TS_REGISTER_DESCRIPTOR(MY_CLASS, MY_EDID, MY_XML_NAME, MY_CLASS::DisplayDescriptor);


//----------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------

ts::CableDeliverySystemDescriptor::CableDeliverySystemDescriptor() :
    AbstractDeliverySystemDescriptor(MY_EDID, DS_DVB_C, MY_XML_NAME)
{
}


//----------------------------------------------------------------------------
// Constructor from a binary descriptor
//----------------------------------------------------------------------------

void ts::CableDeliverySystemDescriptor::clearContent()
{
    frequency = 0;
    FEC_outer = 0;
    modulation = 0;
    symbol_rate = 0;
    FEC_inner = 0;
}

ts::CableDeliverySystemDescriptor::CableDeliverySystemDescriptor(DuckContext& duck, const Descriptor& desc) :
    CableDeliverySystemDescriptor()
{
    deserialize(duck, desc);
}


//----------------------------------------------------------------------------
// Thread-safe init-safe static data patterns.
//----------------------------------------------------------------------------

const std::map<int, ts::InnerFEC>& ts::CableDeliverySystemDescriptor::ToInnerFEC()
{
    static const std::map<int, InnerFEC> data {
        {1,  FEC_1_2},
        {2,  FEC_2_3},
        {3,  FEC_3_4},
        {4,  FEC_5_6},
        {5,  FEC_7_8},
        {6,  FEC_8_9},
        {7,  FEC_3_5},
        {8,  FEC_4_5},
        {9,  FEC_9_10},
        {15, FEC_NONE},
    };
    return data;
}

const std::map<int, ts::Modulation>& ts::CableDeliverySystemDescriptor::ToModulation()
{
    static const std::map<int, Modulation> data {
        {1, QAM_16},
        {2, QAM_32},
        {3, QAM_64},
        {4, QAM_128},
        {5, QAM_256},
    };
    return data;
}

const ts::Names& ts::CableDeliverySystemDescriptor::ModulationNames()
{
    static const Names data({
        {u"16-QAM",  1},
        {u"32-QAM",  2},
        {u"64-QAM",  3},
        {u"128-QAM", 4},
        {u"256-QAM", 5},
    });
    return data;
}

const ts::Names& ts::CableDeliverySystemDescriptor::OuterFecNames()
{
    static const Names data({
        {u"undefined", 0},
        {u"none",      1},
        {u"RS",        2},
    });
    return data;
}

const ts::Names& ts::CableDeliverySystemDescriptor::InnerFecNames()
{
    static const Names data({
        {u"undefined", 0},
        {u"1/2",       1},
        {u"2/3",       2},
        {u"3/4",       3},
        {u"5/6",       4},
        {u"7/8",       5},
        {u"8/9",       6},
        {u"3/5",       7},
        {u"4/5",       8},
        {u"9/10",      9},
        {u"none",     15},
    });
    return data;
}


//----------------------------------------------------------------------------
// Serialization
//----------------------------------------------------------------------------

void ts::CableDeliverySystemDescriptor::serializePayload(PSIBuffer& buf) const
{
    buf.putBCD(frequency / 100, 8);  // coded in 100 Hz units
    buf.putBits(0xFFFF, 12);
    buf.putBits(FEC_outer, 4);
    buf.putUInt8(modulation);
    buf.putBCD(symbol_rate / 100, 7);  // coded in 100 sym/s units
    buf.putBits(FEC_inner, 4);
}


//----------------------------------------------------------------------------
// Deserialization
//----------------------------------------------------------------------------

void ts::CableDeliverySystemDescriptor::deserializePayload(PSIBuffer& buf)
{
    frequency = 100 * buf.getBCD<uint64_t>(8);  // coded in 100 Hz units
    buf.skipReservedBits(12);
    buf.getBits(FEC_outer, 4);
    modulation = buf.getUInt8();
    symbol_rate = 100 * buf.getBCD<uint64_t>(7);  // coded in 100 sym/s units.
    buf.getBits(FEC_inner, 4);
}


//----------------------------------------------------------------------------
// XML serialization
//----------------------------------------------------------------------------

void ts::CableDeliverySystemDescriptor::buildXML(DuckContext& duck, xml::Element* root) const
{
    root->setIntAttribute(u"frequency", frequency, false);
    root->setEnumAttribute(OuterFecNames(), u"FEC_outer", FEC_outer);
    root->setEnumAttribute(ModulationNames(), u"modulation", modulation);
    root->setIntAttribute(u"symbol_rate", symbol_rate, false);
    root->setEnumAttribute(InnerFecNames(), u"FEC_inner", FEC_inner);
}


//----------------------------------------------------------------------------
// XML deserialization
//----------------------------------------------------------------------------

bool ts::CableDeliverySystemDescriptor::analyzeXML(DuckContext& duck, const xml::Element* element)
{
    return element->getIntAttribute(frequency, u"frequency", true) &&
           element->getEnumAttribute(FEC_outer, OuterFecNames(), u"FEC_outer", false, 2) &&
           element->getEnumAttribute(modulation, ModulationNames(), u"modulation", false, 1) &&
           element->getIntAttribute(symbol_rate, u"symbol_rate", true) &&
           element->getEnumAttribute(FEC_inner, InnerFecNames(), u"FEC_inner", true);
}


//----------------------------------------------------------------------------
// Static method to display a descriptor.
//----------------------------------------------------------------------------

void ts::CableDeliverySystemDescriptor::DisplayDescriptor(TablesDisplay& disp, const ts::Descriptor& desc, PSIBuffer& buf, const UString& margin, const ts::DescriptorContext& context)
{
    if (buf.canReadBytes(11)) {
        disp << margin << UString::Format(u"Frequency: %d", buf.getBCD<uint32_t>(4));
        disp << UString::Format(u".%04d MHz", buf.getBCD<uint32_t>(4)) << std::endl;
        buf.skipReservedBits(12);
        const uint8_t fec_outer = buf.getBits<uint8_t>(4);
        const uint8_t modulation = buf.getUInt8();
        disp << margin << UString::Format(u"Symbol rate: %d", buf.getBCD<uint32_t>(3));
        disp << UString::Format(u".%04d Msymbol/s", buf.getBCD<uint32_t>(4)) << std::endl;
        disp << margin << "Modulation: ";
        switch (modulation) {
            case 0:  disp << "not defined"; break;
            case 1:  disp << "16-QAM"; break;
            case 2:  disp << "32-QAM"; break;
            case 3:  disp << "64-QAM"; break;
            case 4:  disp << "128-QAM"; break;
            case 5:  disp << "256-QAM"; break;
            default: disp << "code " << int(modulation) << " (reserved)"; break;
        }
        disp << std::endl;
        disp << margin << "Outer FEC: ";
        switch (fec_outer) {
            case 0:  disp << "not defined"; break;
            case 1:  disp << "none"; break;
            case 2:  disp << "RS(204/188)"; break;
            default: disp << "code " << int(fec_outer) << " (reserved)"; break;
        }
        const uint8_t fec_inner = buf.getBits<uint8_t>(4);
        disp << ", Inner FEC: ";
        switch (fec_inner) {
            case 0:  disp << "not defined"; break;
            case 1:  disp << "1/2 conv. code rate"; break;
            case 2:  disp << "2/3 conv. code rate"; break;
            case 3:  disp << "3/4 conv. code rate"; break;
            case 4:  disp << "5/6 conv. code rate"; break;
            case 5:  disp << "7/8 conv. code rate"; break;
            case 6:  disp << "8/9 conv. code rate"; break;
            case 7:  disp << "3/5 conv. code rate"; break;
            case 8:  disp << "4/5 conv. code rate"; break;
            case 9:  disp << "9/10 conv. code rate"; break;
            case 15: disp << "none"; break;
            default: disp << "code " << int(fec_inner) << " (reserved)"; break;
        }
        disp << std::endl;
    }
}
