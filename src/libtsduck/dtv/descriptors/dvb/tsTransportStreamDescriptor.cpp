//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2025, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------

#include "tsTransportStreamDescriptor.h"
#include "tsDescriptor.h"
#include "tsTablesDisplay.h"
#include "tsPSIRepository.h"
#include "tsPSIBuffer.h"
#include "tsDuckContext.h"
#include "tsxmlElement.h"

#define MY_XML_NAME u"transport_stream_descriptor"
#define MY_CLASS    ts::TransportStreamDescriptor
#define MY_EDID     ts::EDID::Regular(ts::DID_DVB_TRANSPORT_STREAM, ts::Standards::DVB)

TS_REGISTER_DESCRIPTOR(MY_CLASS, MY_EDID, MY_XML_NAME, MY_CLASS::DisplayDescriptor);


//----------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------

ts::TransportStreamDescriptor::TransportStreamDescriptor(const UString& comp) :
    AbstractDescriptor(MY_EDID, MY_XML_NAME),
    compliance(comp)
{
}


//----------------------------------------------------------------------------
// Constructor from a binary descriptor
//----------------------------------------------------------------------------

void ts::TransportStreamDescriptor::clearContent()
{
    compliance.clear();
}

ts::TransportStreamDescriptor::TransportStreamDescriptor(DuckContext& duck, const Descriptor& desc) :
    TransportStreamDescriptor()
{
    deserialize(duck, desc);
}


//----------------------------------------------------------------------------
// Serialization
//----------------------------------------------------------------------------

void ts::TransportStreamDescriptor::serializePayload(PSIBuffer& buf) const
{
    buf.putUTF8(compliance);
}

void ts::TransportStreamDescriptor::deserializePayload(PSIBuffer& buf)
{
    buf.getUTF8(compliance);
}


//----------------------------------------------------------------------------
// Static method to display a descriptor.
//----------------------------------------------------------------------------

void ts::TransportStreamDescriptor::DisplayDescriptor(TablesDisplay& disp, const ts::Descriptor& desc, PSIBuffer& buf, const UString& margin, const ts::DescriptorContext& context)
{
    disp << margin << "Compliance: \"" << buf.getUTF8() << "\"" << std::endl;
}


//----------------------------------------------------------------------------
// XML serialization
//----------------------------------------------------------------------------

void ts::TransportStreamDescriptor::buildXML(DuckContext& duck, xml::Element* root) const
{
    root->setAttribute(u"compliance", compliance);
}

bool ts::TransportStreamDescriptor::analyzeXML(DuckContext& duck, const xml::Element* element)
{
    return element->getAttribute(compliance, u"compliance", true, u"", 0, MAX_DESCRIPTOR_SIZE - 2);
}
