//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2025, Paul Higgs
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------
//!
//!  @file
//!  Representation of an Media_authenticity_descriptor
//!
//----------------------------------------------------------------------------

#pragma once
#include "tsAbstractDescriptor.h"
#include "tsUUID.h"

namespace ts {
    //!
    //! Representation of an Media_authenticity_descriptor.
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
            uint8_t authenticated_stream_type = 0;  //!< 8 bits, stream_types carried in the program to which the descriptor applies which include authenticity information
            std::vector<uint16_t> stream_id {};
        };

        // Public members:
        std::optional<std::vector<stream_information_type>> stream_information {};
        std::optional<UUID> content_uuid {};
        std::optional<UString> uri {};
        std::optional<uint16_t> register_idx {};

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
