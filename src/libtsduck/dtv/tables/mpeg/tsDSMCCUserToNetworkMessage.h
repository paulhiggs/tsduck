//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2025, Piotr Serafin
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------
//!
//!  @file
//!  Representation of an DSM-CC User-to-Network Message Table (DownloadServerInitiate, DownloadInfoIndication)
//!
//----------------------------------------------------------------------------

#pragma once
#include "tsAbstractLongTable.h"

namespace ts {
    //!
    //! Representation of an DSM-CC User-to-Network Message Table (DownloadServerInitiate, DownloadInfoIndication)
    //!
    //! @see ISO/IEC 13818-6, ITU-T Rec. 9.2.2 and 9.2.7. ETSI TR 101 202 V1.2.1 (2003-01), A.1, A.3, A.4, B.
    //! @ingroup table
    //!
    class TSDUCKDLL DSMCCUserToNetworkMessage: public AbstractLongTable {
    public:
        class TSDUCKDLL MessageHeader {
            TS_DEFAULT_COPY_MOVE(MessageHeader);

        public:
            // DSMCCUserToNetworkMessage public members:
            uint8_t  protocol_discriminator = DSMCC_PROTOCOL_DISCRIMINATOR;  //!< Indicates that the message is MPEG-2 DSM-CC message.
            uint8_t  dsmcc_type = DSMCC_TYPE_DOWNLOAD_MESSAGE;               //!< Indicates type of MPEG-2 DSM-CC message.
            uint16_t message_id = 0;                                         //!< Indicates type of message which is being passed.
            uint32_t transaction_id = 0;                                     //!< Used for session integrity and error processing.

            //!
            //! Default constructor.
            //!
            MessageHeader() = default;

            //!
            //! Clear values.
            //!
            void clear();
        };

        class TSDUCKDLL Tap {
        public:
            Tap() = default;        //!< Default constructor.
            uint16_t id = 0x0000;   //!< Tap Id
            uint16_t use = 0x0016;  //!<
            uint16_t association_tag = 0x0000;
            uint16_t selector_type = 0x0001;
            uint32_t transaction_id = 0;
            uint32_t timeout = 0;
        };

        //  ******************************
        //  *** DownloadServerInitiate ***
        //  ******************************
        class TSDUCKDLL LiteComponent {
        public:
            LiteComponent() = default;  //!< Default constructor.
            uint32_t component_id_tag = 0;

            // BIOPObjectLocation
            uint32_t  carousel_id = 0;
            uint16_t  module_id = 0;
            uint8_t   version_major = 0x01;
            uint8_t   version_minor = 0x00;
            ByteBlock object_key_data {};

            // DSMConnBinder
            Tap tap {};

            // UnknownComponent
            std::optional<ByteBlock> component_data {};
        };

        class TSDUCKDLL TaggedProfile {
        public:
            TaggedProfile() = default;  //!< Default constructor.
            uint32_t profile_id_tag = 0;
            uint8_t  profile_data_byte_order = 0;

            // BIOP Profile Body
            std::list<LiteComponent> liteComponents {};

            // Any other profile for now
            std::optional<ByteBlock> profile_data {};
        };

        class TSDUCKDLL IOR {
        public:
            IOR() = default;  //!< Default constructor.
            ByteBlock                type_id {};
            std::list<TaggedProfile> tagged_profiles {};
        };

        //  ******************************
        //  *** DownloadInfoIndication ***
        //  ******************************
        class TSDUCKDLL Module: public EntryWithDescriptors {
            TS_NO_DEFAULT_CONSTRUCTORS(Module);
            TS_DEFAULT_ASSIGMENTS(Module);

        public:
            uint16_t       module_id = 0;
            uint32_t       module_size = 0;
            uint8_t        module_version = 0;
            uint32_t       module_timeout = 0;
            uint32_t       block_timeout = 0;
            uint32_t       min_block_time = 0;
            std::list<Tap> taps {};

            explicit Module(const AbstractTable* table);
        };

        MessageHeader header {};
        ByteBlock     server_id {};  //!< Conveys the data of the block.
        IOR           ior {};

        using ModuleList = EntryWithDescriptorsList<Module>;

        uint32_t   download_id = 0;
        uint16_t   block_size = 0;
        ModuleList modules;

        //!
        //! Default constructor.
        //! @param [in] vers Table version number.
        //! @param [in] cur True if table is current, false if table is next.
        //!
        DSMCCUserToNetworkMessage(uint8_t vers = 0, bool cur = true);

        //!
        //! Constructor from a binary table.
        //! @param [in,out] duck TSDuck execution context.
        //! @param [in] table Binary table to deserialize.
        //!
        DSMCCUserToNetworkMessage(DuckContext& duck, const BinaryTable& table);

        // Inherited methods
        DeclareDisplaySection();
        virtual bool     isPrivate() const override;
        virtual uint16_t tableIdExtension() const override;

    protected:
        // Inherited methods
        virtual void   clearContent() override;
        virtual size_t maxPayloadSize() const override;
        virtual void   serializePayload(BinaryTable&, PSIBuffer&) const override;
        virtual void   deserializePayload(PSIBuffer&, const Section&) override;
        virtual void   buildXML(DuckContext&, xml::Element*) const override;
        virtual bool   analyzeXML(DuckContext& duck, const xml::Element* element) override;

    private:
        static constexpr size_t MESSAGE_HEADER_SIZE = 12;  //!< DSM-CC Message Header size w/o adaptation header
        static constexpr size_t SERVER_ID_SIZE = 20;       //!< Fixed size in bytes of server_id.

        static constexpr uint8_t  DSMCC_PROTOCOL_DISCRIMINATOR = 0x11;     //!< Protocol Discriminator for DSM-CC
        static constexpr uint8_t  DSMCC_TYPE_DOWNLOAD_MESSAGE = 0x03;      //!< MPEG-2 DSM-CC Download Message
        static constexpr uint16_t DSMCC_MESSAGE_ID_DII = 0x1002;           //!< DownloadInfoIndication
        static constexpr uint16_t DSMCC_MESSAGE_ID_DSI = 0x1006;           //!< DownloadServerInitiate
        static constexpr uint32_t DSMCC_TAG_LITE_OPTIONS = 0x49534F05;     //!< TAG_LITE_OPTIONS (Lite Options Profile Body)
        static constexpr uint32_t DSMCC_TAG_BIOP_PROFILE = 0x49534F06;     //!< TAG_BIOP (BIOP Profile Body)
        static constexpr uint32_t DSMCC_TAG_CONN_BINDER = 0x49534F40;      //!< TAG_ConnBinder (DSM::ConnBinder)
        static constexpr uint32_t DSMCC_TAG_OBJECT_LOCATION = 0x49534F50;  //!< TAG_ObjectLocation (BIOP::ObjectLocation)
    };
}  // namespace ts
