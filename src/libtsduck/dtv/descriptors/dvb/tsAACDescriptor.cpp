//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2025, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------

#include "tsAACDescriptor.h"
#include "tsComponentDescriptor.h"
#include "tsDescriptor.h"
#include "tsTablesDisplay.h"
#include "tsPSIRepository.h"
#include "tsPSIBuffer.h"
#include "tsDuckContext.h"
#include "tsxmlElement.h"

#define MY_XML_NAME u"AAC_descriptor"
#define MY_CLASS    ts::AACDescriptor
#define MY_EDID     ts::EDID::Regular(ts::DID_DVB_AAC, ts::Standards::DVB)

TS_REGISTER_DESCRIPTOR(MY_CLASS, MY_EDID, MY_XML_NAME, MY_CLASS::DisplayDescriptor);


//----------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------

ts::AACDescriptor::AACDescriptor() :
    AbstractDescriptor(MY_EDID, MY_XML_NAME)
{
}

ts::AACDescriptor::AACDescriptor(DuckContext& duck, const Descriptor& desc) :
    AACDescriptor()
{
    deserialize(duck, desc);
}

void ts::AACDescriptor::clearContent()
{
    profile_and_level = 0;
    SAOC_DE.reset();
    AAC_type.reset();
    additional_info.clear();
}


//----------------------------------------------------------------------------
// Serialization
//----------------------------------------------------------------------------

void ts::AACDescriptor::serializePayload(PSIBuffer& buf) const
{
    buf.putUInt8(profile_and_level);
    if (SAOC_DE.has_value()) {
        buf.putBit(AAC_type.has_value());
        buf.putBit(SAOC_DE.value());
        buf.putBits(0, 6);
        if (AAC_type.has_value()) {
            buf.putUInt8(AAC_type.value());
        }
        buf.putBytes(additional_info);
    }
}


//----------------------------------------------------------------------------
// Deserialization
//----------------------------------------------------------------------------

void ts::AACDescriptor::deserializePayload(PSIBuffer& buf)
{
    profile_and_level = buf.getUInt8();
    if (buf.canRead()) {
        bool has_AAC_type = buf.getBool();
        SAOC_DE = buf.getBool();
        buf.skipBits(6);
        if (has_AAC_type) {
            AAC_type = buf.getUInt8();
        }
        buf.getBytes(additional_info);
    }
}


//----------------------------------------------------------------------------
// Static method to display a descriptor.
//----------------------------------------------------------------------------

void ts::AACDescriptor::DisplayDescriptor(TablesDisplay& disp, const ts::Descriptor& desc, PSIBuffer& buf, const UString& margin, const ts::DescriptorContext& context)
{
    if (buf.canRead()) {
        disp << margin << UString::Format(u"Profile and level: 0x%X", buf.getUInt8()) << std::endl;
    }

    if (buf.canRead()) {
        bool has_AAC_type = buf.getBool();
        disp << margin << UString::Format(u"SOAC DE flag: %s", buf.getBool()) << std::endl;
        buf.skipBits(6);
        if (has_AAC_type && buf.canRead()) {
            disp << margin << "AAC type: " << ComponentDescriptor::ComponentTypeName(disp.duck(), 6, 0, buf.getUInt8(), NamesFlags::HEX_VALUE_NAME, 8) << std::endl;
        }
        disp.displayPrivateData(u"Additional information", buf, NPOS, margin);
    }
}


//----------------------------------------------------------------------------
// Get the string representation of an AAC type.
//----------------------------------------------------------------------------

ts::UString ts::AACDescriptor::aacTypeString() const
{
    return isValid() && AAC_type.has_value() ? aacTypeString(AAC_type.value()) : UString();
}

ts::UString ts::AACDescriptor::aacTypeString(uint8_t type)
{
    DuckContext duck; // only needed by component_descriptor when in Japan.
    return ComponentDescriptor::ComponentTypeName(duck, 6, 0, type, NamesFlags::NAME, 8);
}



//----------------------------------------------------------------------------
// XML serialization
//----------------------------------------------------------------------------

void ts::AACDescriptor::buildXML(DuckContext& duck, xml::Element* root) const
{
    root->setIntAttribute(u"profile_and_level", profile_and_level, true);
    root->setOptionalBoolAttribute(u"SAOC_DE", SAOC_DE);
    root->setOptionalIntAttribute(u"AAC_type", AAC_type, true);
    root->addHexaTextChild(u"additional_info", additional_info, true);
}


//----------------------------------------------------------------------------
// XML deserialization
//----------------------------------------------------------------------------

bool ts::AACDescriptor::analyzeXML(DuckContext& duck, const xml::Element* element)
{
    bool ok = element->getIntAttribute(profile_and_level, u"profile_and_level", true) &&
              element->getOptionalBoolAttribute(SAOC_DE, u"SAOC_DE") &&
              element->getOptionalIntAttribute(AAC_type, u"AAC_type") &&
              element->getHexaTextChild(additional_info, u"additional_info", false, 0, MAX_DESCRIPTOR_SIZE - 5);

    if (ok && !SAOC_DE.has_value() && (AAC_type.has_value() || !additional_info.empty())) {
        element->report().error(u"SAOC_DE is required to be specified when either AAC_type or additional_info is specified in <%s>, line %d", element->name(), element->lineNumber());
        ok = false;
    }
    return ok;
}
