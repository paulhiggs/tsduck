//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2025, Paul Higgs
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------

#include "tsMediaAuthenticityDescriptor.h"
#include "tsDescriptor.h"
#include "tsTablesDisplay.h"
#include "tsPSIRepository.h"
#include "tsPSIBuffer.h"
#include "tsDuckContext.h"
#include "tsxmlElement.h"
#include "tsStreamType.h"

#define MY_XML_NAME u"media_authenticity_descriptor"
#define MY_CLASS    ts::MediaAuthenticityDescriptor
#define MY_EDID     ts::EDID::ExtensionMPEG(ts::XDID_MPEG_MEDIA_AUTHENTICITY)

TS_REGISTER_DESCRIPTOR(MY_CLASS, MY_EDID, MY_XML_NAME, MY_CLASS::DisplayDescriptor);


//----------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------

ts::MediaAuthenticityDescriptor::MediaAuthenticityDescriptor() :
    AbstractDescriptor(MY_EDID, MY_XML_NAME)
{
}

void ts::MediaAuthenticityDescriptor::clearContent()
{
    stream_information.reset();
    content_uuid.reset();
    uri.reset();
    register_idx.reset();
}

ts::MediaAuthenticityDescriptor::MediaAuthenticityDescriptor(DuckContext& duck, const Descriptor& desc) :
    MediaAuthenticityDescriptor()
{
    deserialize(duck, desc);
}


//----------------------------------------------------------------------------
// Serialization
//----------------------------------------------------------------------------

void ts::MediaAuthenticityDescriptor::serializePayload(PSIBuffer& buf) const
{
    buf.putBit(stream_information.has_value());
    buf.putBit(content_uuid.has_value());
    buf.putBit(uri.has_value());
    buf.putBit(register_idx.has_value());

    buf.putBits(0xFF, 4); // PH padding not included in draft contribution

    if (stream_information.has_value()) {
        std::vector<stream_information_type> siv = stream_information.value();
        buf.putBits(siv.size(), 4);
        buf.putBits(0xFF, 4);  // PH padding not included in draft contribution
        for (const auto& si : siv) {
            buf.putUInt8(si.authenticated_stream_type);
            buf.putBits(0xFF, 3);
            buf.putBits(si.stream_ids.size(), 5);
            for (const auto& sid : si.stream_ids) {
                buf.putBits(sid, 8);
            }
        }
    }

    if (content_uuid.has_value()) {
       UUID uuid = content_uuid.value();
       uuid.serialize(buf);
    }

    if (uri.has_value()) {
        buf.putUTF8WithLength(uri.value());
        if (register_idx.has_value()) {
            buf.putBits(0xFF, 6);
            buf.putBits(register_idx.value(), 10);
        }
    }

}


//----------------------------------------------------------------------------
// Deserialization
//----------------------------------------------------------------------------

void ts::MediaAuthenticityDescriptor::deserializePayload(PSIBuffer& buf)
{
    bool _stream_information_present_flag = buf.getBool();
    bool _content_uuid_present_flag = buf.getBool();
    bool _key_source_uri_present_flag = buf.getBool();
    bool _register_idx_present_flag = buf.getBool();
    buf.skipBits(4); // PH padding not included in draft contribution

    if (_stream_information_present_flag) {
        std::vector<stream_information_type> newSI;
        uint8_t _authenticated_stream_types_count = buf.getBits<uint8_t>(4);
        buf.skipBits(4);  // PH padding not included in draft contribution
        for (uint8_t i = 0; i < _authenticated_stream_types_count; i++) {
            stream_information_type si;
            si.authenticated_stream_type = buf.getUInt8();
            buf.skipBits(3);
            uint8_t _authenticated_streams_count = buf.getBits<uint8_t>(5);
            for (uint8_t j = 0; j < _authenticated_streams_count; j++) {
                si.stream_ids.push_back(buf.getUInt8());
            }
            newSI.push_back(si);
        }
        stream_information = newSI;
    }

    if (_content_uuid_present_flag) {
        UUID cid(buf);
        content_uuid = cid;
    }

    if (_key_source_uri_present_flag) {
        uri = buf.getUTF8WithLength();
        if (_register_idx_present_flag) {
            buf.skipBits(6);
            uint16_t t = buf.getBits<uint16_t>(10);
            register_idx = t;
        }
    }
}


//----------------------------------------------------------------------------
// Static method to display a descriptor.
//----------------------------------------------------------------------------


void ts::MediaAuthenticityDescriptor::DisplayDescriptor(TablesDisplay& disp, const ts::Descriptor& desc, PSIBuffer& buf, const UString& margin, const ts::DescriptorContext& context)
{
    bool _stream_information_present_flag = buf.getBool();
    bool _content_uuid_present_flag = buf.getBool();
    bool _key_source_uri_present_flag = buf.getBool();
    bool _register_idx_present_flag = buf.getBool();
    buf.skipBits(4);  // PH padding not included in draft contribution
    
    if (_stream_information_present_flag) {
        uint8_t _authenticated_stream_types_count = buf.getBits<uint8_t>(4);
        buf.skipBits(4);  // PH padding not included in draft contribution
        for (uint8_t i = 0; i < _authenticated_stream_types_count; i++) {
            uint8_t _authenticated_stream_type = buf.getUInt8();
            buf.skipBits(3);
            uint8_t _authenticated_streams_count = buf.getBits<uint8_t>(5);
            std::vector<uint8_t> sids;
            for (uint8_t j = 0; j < _authenticated_streams_count; j++) {
                sids.push_back(buf.getUInt8());
            }
            disp << margin << "Stream type: " << StreamTypeName(_authenticated_stream_type, NamesFlags::VALUE_NAME) << std::endl;
            disp.displayVector(u"Stream IDs:", sids, margin + u" ", true, 16, false); 
        }
    }

    if (_content_uuid_present_flag) {
        UUID uuid(buf);
        disp << margin << "Content UUID: " << uuid.format() << std::endl;
    }

    if (_key_source_uri_present_flag) {
        disp << margin << "Key Source URI: " << buf.getUTF8WithLength() << std::endl;
        if (_register_idx_present_flag) {
            buf.skipReservedBits(6);
            disp << margin << "Register Index: " << buf.getBits<uint16_t>(10) << std::endl;
        }
    }
}


//----------------------------------------------------------------------------
// XML serialization
//----------------------------------------------------------------------------

template <typename Iterator, typename Separator>
std::string join(Iterator begin, Iterator end, Separator&& separator = '.')
{
    std::ostringstream o;
    if (begin != end) {
        o << *begin++;
        for (; begin != end; ++begin)
            o << separator << *begin;
    }
    return o.str();
}

template <typename Container>
ts::UString join(Container const& c, char separator = '.') 
{
    using std::begin;
    using std::end;
    return ts::UString(join(begin(c), end(c), separator));
}

void ts::MediaAuthenticityDescriptor::buildXML(DuckContext& duck, xml::Element* root) const
{
    if (content_uuid.has_value()) {
        UUID uuid = content_uuid.value();
        root->setAttribute(u"uuid", uuid.format());
    }
    root->setOptionalAttribute(u"uri", uri);
    root->setOptionalIntAttribute(u"register_index", register_idx);

    if (stream_information.has_value()) {
        std::vector<stream_information_type> streams = stream_information.value();
        for (const auto& si : streams) {
            ts::xml::Element* element = root->addElement(u"authenticated_stream");
            element->setIntAttribute(u"stream_type", si.authenticated_stream_type, true);
            element->setAttribute(u"authenticated_stream_ids", join(si.stream_ids, ' '));
        }
    }
}


//----------------------------------------------------------------------------
// XML deserialization
//----------------------------------------------------------------------------

bool ts::MediaAuthenticityDescriptor::stream_information_type::insert_stream_id(std::string stream_id, const ts::xml::Element* element)
{
    bool ok = true;
    if (stream_id.length() == 0)
        return true;
    try {
        if (std::stol(stream_id) > 255) {
            element->report().error(u"stream_id must be less than 255 (got '%d') is not a valid level_id in <%s>, line %d", std::stol(stream_id), element->name(), element->lineNumber());
            ok = false;
        }
        else {
            stream_ids.push_back(uint8_t(std::stol(stream_id)));
        }
    }
    catch (std::invalid_argument const& ex) {
        element->report().error(u"'%s' is not a valid stream_id (%s) in <%s>, line %d", stream_id, ex.what(), element->name(), element->lineNumber());
        ok = false;
    }
    return ok;
}

bool ts::MediaAuthenticityDescriptor::analyzeXML(DuckContext& duck, const xml::Element* element)
{
    std::optional<UString> read_uuid;
    bool ok = element->getOptionalAttribute(read_uuid, u"uuid")
           && element->getOptionalAttribute(uri, u"uri")
           && element->getOptionalIntAttribute(register_idx, u"register_index", 0, 0b0000001111111111);

    if (ok && read_uuid.has_value()) {
        UUID uuid;
        if (!uuid.parse(read_uuid.value())) {
            element->report().error(u"incorrect format for @uuid in <%s>, line %d", element->name(), element->lineNumber());               
            ok = false;
        }
        content_uuid = uuid;
    }

    xml::ElementVector children;
    ok &= element->getChildren(children, u"authenticated_stream", 0, 0b00001111);
    if (ok && !children.empty()) {
        std::vector<stream_information_type> auth_streams;
        for (size_t i = 0; ok && i < children.size(); i++) {
            stream_information_type sit;
            UString stream_ids;
            ok = children[i]->getIntAttribute(sit.authenticated_stream_type, u"stream_type") &&
                children[i]->getAttribute(stream_ids, u"authenticated_stream_ids");

            if (ok) {
                const auto delim = ' ';
                auto pos = stream_ids.find(delim);
                while (ok && pos != UString::npos) {
                    std::string val = stream_ids.substr(0, pos).toUTF8();
                    ok = sit.insert_stream_id(val, element);
                    stream_ids.erase(0, pos + sizeof(delim));
                    pos = stream_ids.find(delim);
                }
                ok = sit.insert_stream_id(stream_ids.toUTF8(), element);

                if (sit.stream_ids.size() > 32) {
                    element->report().error(u"a maximum of 32 stream_ids can be specified for a given stream_type in <%s>, line %d", element->name(), element->lineNumber());   
                    ok = false;
                }
                if (ok) {
                    auth_streams.push_back(sit);
                }
            }
        }
        if (ok) {
            stream_information = auth_streams;
        }
    }
    return ok;
}
