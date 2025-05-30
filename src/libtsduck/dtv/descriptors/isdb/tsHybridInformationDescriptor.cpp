//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2025, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------

#include "tsHybridInformationDescriptor.h"
#include "tsDescriptor.h"
#include "tsTablesDisplay.h"
#include "tsPSIRepository.h"
#include "tsPSIBuffer.h"
#include "tsDuckContext.h"
#include "tsxmlElement.h"

#define MY_XML_NAME u"hybrid_information_descriptor"
#define MY_CLASS    ts::HybridInformationDescriptor
#define MY_EDID     ts::EDID::Regular(ts::DID_ISDB_HYBRID_INFO, ts::Standards::ISDB)

TS_REGISTER_DESCRIPTOR(MY_CLASS, MY_EDID, MY_XML_NAME, MY_CLASS::DisplayDescriptor);


//----------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------

ts::HybridInformationDescriptor::HybridInformationDescriptor() :
    AbstractDescriptor(MY_EDID, MY_XML_NAME)
{
}

ts::HybridInformationDescriptor::HybridInformationDescriptor(DuckContext& duck, const Descriptor& desc) :
    HybridInformationDescriptor()
{
    deserialize(duck, desc);
}

void ts::HybridInformationDescriptor::clearContent()
{
    has_location = false;
    location_type = false;
    format = 0;
    component_tag = 0;
    module_id = 0;
    URL.clear();
}


//----------------------------------------------------------------------------
// Serialization
//----------------------------------------------------------------------------

void ts::HybridInformationDescriptor::serializePayload(PSIBuffer& buf) const
{
    buf.putBit(has_location);
    buf.putBit(location_type);
    buf.putBits(format, 4);
    buf.putBits(0xFF, 2);
    if (has_location) {
        if (location_type) {
            // We assume here that the URL is encoded in ARIB STD-B24. Could be in ASCII ?
            buf.putStringWithByteLength(URL);
        }
        else {
            buf.putUInt8(component_tag);
            buf.putUInt16(module_id);
        }
    }
}


//----------------------------------------------------------------------------
// Deserialization
//----------------------------------------------------------------------------

void ts::HybridInformationDescriptor::deserializePayload(PSIBuffer& buf)
{
    has_location = buf.getBool();
    location_type = buf.getBool();
    buf.getBits(format, 4);
    buf.skipBits(2);
    if (has_location) {
        if (location_type) {
            // We assume here that the URL is encoded in ARIB STD-B24. Could be in ASCII ?
            buf.getStringWithByteLength(URL);
        }
        else {
            component_tag = buf.getUInt8();
            module_id = buf.getUInt16();
        }
    }
}


//----------------------------------------------------------------------------
// Static method to display a descriptor.
//----------------------------------------------------------------------------

void ts::HybridInformationDescriptor::DisplayDescriptor(TablesDisplay& disp, const ts::Descriptor& desc, PSIBuffer& buf, const UString& margin, const ts::DescriptorContext& context)
{
    if (buf.canReadBytes(1)) {
        const bool has_location = buf.getBool();
        const bool location_type = buf.getBool();
        disp << margin << "Has location: " << UString::YesNo(has_location) << std::endl;
        disp << margin << "Location type: " << (location_type ? "connected" : "broadcast") << std::endl;
        disp << margin << "Format: " << DataName(MY_XML_NAME, u"Format", buf.getBits<uint8_t>(4), NamesFlags::DEC_VALUE_NAME) << std::endl;
        buf.skipBits(2);
        if (has_location) {
            if (location_type) {
                disp << margin << "URL: \"" << buf.getStringWithByteLength() << "\"" << std::endl;
            }
            else if (buf.canReadBytes(3)) {
                disp << margin << UString::Format(u"Component tag: 0x0%X (%<d)", buf.getUInt8()) << std::endl;
                disp << margin << UString::Format(u"Module id: 0x0%X (%<d)", buf.getUInt16()) << std::endl;
            }
        }
    }
}


//----------------------------------------------------------------------------
// XML serialization
//----------------------------------------------------------------------------

void ts::HybridInformationDescriptor::buildXML(DuckContext& duck, xml::Element* root) const
{
    root->setIntAttribute(u"format", format);
    if (has_location) {
        if (location_type) {
            root->setAttribute(u"URL", URL);
        }
        else {
            root->setIntAttribute(u"component_tag", component_tag, true);
            root->setIntAttribute(u"module_id", module_id, true);
        }
    }
}


//----------------------------------------------------------------------------
// XML deserialization
//----------------------------------------------------------------------------

bool ts::HybridInformationDescriptor::analyzeXML(DuckContext& duck, const xml::Element* element)
{
    const bool has_connect = element->hasAttribute(u"URL");
    const int bcast_count = element->hasAttribute(u"component_tag") + element->hasAttribute(u"module_id");

    has_location = has_connect || bcast_count > 0;
    location_type = has_connect;

    if (bcast_count == 1) {
        element->report().error(u"attributes 'component_tag' and 'module_id' must be both present or both absent in <%s>, line %d", element->name(), element->lineNumber());
        return false;
    }
    else if (has_connect && bcast_count > 0) {
        element->report().error(u"attribute 'URL' and attributes 'component_tag', 'module_id' are mutually exclusive in <%s>, line %d", element->name(), element->lineNumber());
        return false;
    }
    else if (!element->getIntAttribute(format, u"format", true, 0, 0, 15)) {
        return false;
    }
    else if (!has_location) {
        return true;
    }
    else if (location_type) {
        return element->getAttribute(URL, u"URL");
    }
    else {
        return element->getIntAttribute(component_tag, u"component_tag") && element->getIntAttribute(module_id, u"module_id");
    }
}
