//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2025, Paul Higgs
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------
//!
//!  @file
//!  Representation of an media_authenticity_descriptor
//!
//----------------------------------------------------------------------------

#pragma once
#include "tsAbstractDescriptor.h"
#include "tsUUID.h"

namespace ts {
    //!
    //! Representation of a media_authenticity_descriptor.
    //!
    //! @see ISO/IEC 13818-1 2.6.xxx
    //! @ingroup libtsduck descriptor
    //!
    //!
    class TSDUCKDLL MediaAuthenticityDescriptor: public AbstractDescriptor
    {
    public:
        //!
        //! Description of stream information.
        //!
        class stream_information_type
        {
        public:
            stream_information_type() = default;    //!< Constructor
            uint8_t authenticated_stream_type = 0;  //!< 8 bits, stream_type for the list of stream_ids
            std::vector<uint8_t> stream_ids {};     //!< 8 bits, stream_id that contains media authenticity information

            //!
            //!
            //! @param [in] stream_id The stream identifier, read from the XML element, to be inserted into the \p stream_ids vector
            //! @param [in,out] element The XML element containing the \p stream_id value
            //! @return True if the value is in the correct format and was inserted into the stream_ids vector, false otherwise with an error recorded against \p element
            //! 
            bool insert_stream_id(std::string stream_id, const ts::xml::Element* element);
        };

        // Public members:
        std::optional<std::vector<stream_information_type>> stream_information {};  //!< list of stream identifiers (grouped by stream type) that have authenticity information
        std::optional<UUID> content_uuid {};                        //!< identifier if the authenticity scheme used
        std::optional<UString> uri {};                              //!< URL to certificate of the content provider or register of certificates
        std::optional<uint16_t> register_idx {};                    //!< 10 bits, certificate of the content provider, in the certificate register indicated in uri

        //!
        //! Default constructor.
        //!
        MediaAuthenticityDescriptor();

        //!
        //! Constructor from a binary descriptor
        //! @param [in,out] duck TSDuck execution context.
        //! @param [in] bin A binary descriptor to deserialize.
        //!
        MediaAuthenticityDescriptor(DuckContext& duck, const Descriptor& bin);

        // Inherited methods
        DeclareDisplayDescriptor();

    protected:
        // Inherited methods
        virtual void clearContent() override;
        virtual void serializePayload(PSIBuffer&) const override;
        virtual void deserializePayload(PSIBuffer&) override;
        virtual void buildXML(DuckContext&, xml::Element*) const override;
        virtual bool analyzeXML(DuckContext&, const xml::Element*) override;
    };
}
