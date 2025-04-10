//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2025, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------

#include "tsIPMACPlatformProviderNameDescriptor.h"
#include "tsDescriptor.h"
#include "tsTablesDisplay.h"
#include "tsPSIRepository.h"
#include "tsPSIBuffer.h"
#include "tsDuckContext.h"
#include "tsxmlElement.h"

#define MY_XML_NAME u"IPMAC_platform_provider_name_descriptor"
#define MY_CLASS    ts::IPMACPlatformProviderNameDescriptor
#define MY_EDID     ts::EDID::TableSpecific(ts::DID_INT_PF_PROVIDER, ts::Standards::DVB, ts::TID_INT)

TS_REGISTER_DESCRIPTOR(MY_CLASS, MY_EDID, MY_XML_NAME, MY_CLASS::DisplayDescriptor);


//----------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------

ts::IPMACPlatformProviderNameDescriptor::IPMACPlatformProviderNameDescriptor(const UString& lang, const UString& name) :
    AbstractDescriptor(MY_EDID, MY_XML_NAME),
    language_code(lang),
    text(name)
{
}

void ts::IPMACPlatformProviderNameDescriptor::clearContent()
{
    language_code.clear();
    text.clear();
}

ts::IPMACPlatformProviderNameDescriptor::IPMACPlatformProviderNameDescriptor(DuckContext& duck, const Descriptor& desc) :
    IPMACPlatformProviderNameDescriptor()
{
    deserialize(duck, desc);
}


//----------------------------------------------------------------------------
// Serialization
//----------------------------------------------------------------------------

void ts::IPMACPlatformProviderNameDescriptor::serializePayload(PSIBuffer& buf) const
{
    buf.putLanguageCode(language_code);
    buf.putString(text);
}

void ts::IPMACPlatformProviderNameDescriptor::deserializePayload(PSIBuffer& buf)
{
    buf.getLanguageCode(language_code);
    buf.getString(text);
}


//----------------------------------------------------------------------------
// Static method to display a descriptor.
//----------------------------------------------------------------------------

void ts::IPMACPlatformProviderNameDescriptor::DisplayDescriptor(TablesDisplay& disp, const ts::Descriptor& desc, PSIBuffer& buf, const UString& margin, const ts::DescriptorContext& context)
{
    if (buf.canReadBytes(3)) {
        disp << margin << "Language: " << buf.getLanguageCode() << std::endl;
        disp << margin << "Platform name: " << buf.getString() << std::endl;
    }
}


//----------------------------------------------------------------------------
// XML serialization
//----------------------------------------------------------------------------

void ts::IPMACPlatformProviderNameDescriptor::buildXML(DuckContext& duck, xml::Element* root) const
{
    root->setAttribute(u"language_code", language_code);
    root->setAttribute(u"text", text);
}

bool ts::IPMACPlatformProviderNameDescriptor::analyzeXML(DuckContext& duck, const xml::Element* element)
{
    return element->getAttribute(language_code, u"language_code", true, UString(), 3, 3) &&
           element->getAttribute(text, u"text", true, UString(), 0, MAX_DESCRIPTOR_SIZE - 5);
}
