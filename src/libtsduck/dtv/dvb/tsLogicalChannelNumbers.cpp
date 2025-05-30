//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2025, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------

#include "tsLogicalChannelNumbers.h"
#include "tsEacemLogicalChannelNumberDescriptor.h"
#include "tsEacemHDSimulcastLogicalChannelDescriptor.h"
#include "tsEutelsatChannelNumberDescriptor.h"
#include "tsDTGLogicalChannelDescriptor.h"
#include "tsDTGHDSimulcastLogicalChannelDescriptor.h"
#include "tsNorDigLogicalChannelDescriptorV1.h"
#include "tsNorDigLogicalChannelDescriptorV2.h"
#include "tsSkyLogicalChannelNumberDescriptor.h"


//----------------------------------------------------------------------------
// Constructors and destructors
//----------------------------------------------------------------------------

ts::LogicalChannelNumbers::LogicalChannelNumbers(DuckContext& duck) :
    _duck(duck)
{
}


//----------------------------------------------------------------------------
// Add the logical channel number of a service.
//----------------------------------------------------------------------------

void ts::LogicalChannelNumbers::addLCN(uint16_t lcn, uint16_t srv_id, uint16_t ts_id, uint16_t onet_id, bool visible)
{
    // Loop for similar entry to update.
    for (auto it = _lcn_map.lower_bound(srv_id); it != _lcn_map.end() && it->first == srv_id; ++it) {
        if (it->second.ts_id == ts_id && it->second.onet_id == onet_id) {
            // Update existing entry.
            it->second.lcn = lcn;
            it->second.visible = visible;
            return;
        }
    }

    // No existing entry found, add a new one.
    _lcn_map.insert(std::make_pair(srv_id, LCN{lcn, ts_id, onet_id, visible}));
}


//----------------------------------------------------------------------------
// Collect all LCN which are declared in a list of descriptors.
//----------------------------------------------------------------------------

size_t ts::LogicalChannelNumbers::addFromDescriptors(const DescriptorList& descs, uint16_t ts_id, uint16_t onet_id)
{
    size_t count = 0;
    for (size_t index = 0; index < descs.size(); ++index) {
        const Descriptor& bindesc(descs[index]);
        if (bindesc.isValid()) {

            // Most LCN descriptors are private descriptors. Get tag and PDS.
            const DID tag = bindesc.tag();
            const PDS pds = _duck.actualPDS(descs.privateDataSpecifier(index));

            // Check all known forms of LCN descriptors.
            if (pds == PDS_EACEM && tag == DID_EACEM_LCN) {
                const EacemLogicalChannelNumberDescriptor desc(_duck, bindesc);
                count += addFromAbstractLCN(desc, ts_id, onet_id);
            }
            else if (pds == PDS_EACEM && tag == DID_EACEM_HD_SIMULCAST_LCN) {
                const EacemHDSimulcastLogicalChannelDescriptor desc(_duck, bindesc);
                count += addFromAbstractLCN(desc, ts_id, onet_id);
            }
            else if (pds == PDS_OFCOM && tag == DID_OFCOM_LOGICAL_CHAN) {
                const DTGLogicalChannelDescriptor desc(_duck, bindesc);
                count += addFromAbstractLCN(desc, ts_id, onet_id);
            }
            else if (pds == PDS_OFCOM && tag == DID_OFCOM_HD_SIMULCAST) {
                DTGHDSimulcastLogicalChannelDescriptor desc(_duck, bindesc);
                count += addFromAbstractLCN(desc, ts_id, onet_id);
            }
            else if (pds == PDS_BSKYB && tag == DID_SKY_LCN) {
                SkyLogicalChannelNumberDescriptor desc(_duck, bindesc);
                if (desc.isValid()) {
                    for (const auto& it : desc.entries) {
                        addLCN(it.lcn, it.service_id, ts_id, onet_id);
                        count++;
                    }
                }
            }
            else if (pds == PDS_EUTELSAT && tag == DID_EUTELSAT_CHAN_NUM) {
                EutelsatChannelNumberDescriptor desc(_duck, bindesc);
                if (desc.isValid()) {
                    for (const auto& it : desc.entries) {
                        addLCN(it.ecn, it.service_id, it.ts_id, it.onetw_id);
                        count++;
                    }
                }
            }
            else if (pds == PDS_NORDIG && tag == DID_NORDIG_CHAN_NUM_V1) {
                NorDigLogicalChannelDescriptorV1 desc(_duck, bindesc);
                if (desc.isValid()) {
                    for (const auto& it : desc.entries) {
                        addLCN(it.lcn, it.service_id, ts_id, onet_id, it.visible);
                        count++;
                    }
                }
            }
            else if (pds == PDS_NORDIG && tag == DID_NORDIG_CHAN_NUM_V2) {
                NorDigLogicalChannelDescriptorV2 desc(_duck, bindesc);
                if (desc.isValid()) {
                    for (const auto& it1 : desc.entries) {
                        for (const auto& it2 : it1.services) {
                            addLCN(it2.lcn, it2.service_id, ts_id, onet_id, it2.visible);
                            count++;
                        }
                    }
                }
            }
        }
    }
    return count;
}


//----------------------------------------------------------------------------
// Collect LCN for a generic form of LCN descriptor.
//----------------------------------------------------------------------------

size_t ts::LogicalChannelNumbers::addFromAbstractLCN(const AbstractLogicalChannelDescriptor& desc, uint16_t ts_id, uint16_t onet_id)
{
    size_t count = 0;
    if (desc.isValid()) {
        for (const auto& it : desc.entries) {
            addLCN(it.lcn, it.service_id, ts_id, onet_id, it.visible);
            count++;
        }
    }
    return count;
}


//----------------------------------------------------------------------------
// Collect all LCN which are declared in a NIT.
//----------------------------------------------------------------------------

size_t ts::LogicalChannelNumbers::addFromNIT(const NIT& nit, uint16_t ts_id, uint16_t onet_id)
{
    size_t count = 0;
    if (nit.isValid()) {
        for (const auto& it : nit.transports) {
            if ((ts_id == INVALID_TS_ID || it.first.transport_stream_id == INVALID_TS_ID || ts_id == it.first.transport_stream_id) &&
                (onet_id == INVALID_NETWORK_ID || it.first.original_network_id == INVALID_NETWORK_ID || onet_id == it.first.original_network_id))
            {
                count += addFromDescriptors(it.second.descs, it.first.transport_stream_id, it.first.original_network_id);
            }
        }
    }
    return count;
}


//----------------------------------------------------------------------------
// Collect all LCN which are declared in an Astra-defined SGT.
//----------------------------------------------------------------------------

size_t ts::LogicalChannelNumbers::addFromSGT(const SGT& sgt, uint16_t ts_id, uint16_t onet_id)
{
    size_t count = 0;
    if (sgt.isValid()) {
        for (const auto& it : sgt.services) {
            if ((ts_id == INVALID_TS_ID || it.first.transport_stream_id == INVALID_TS_ID || ts_id == it.first.transport_stream_id) &&
                (onet_id == INVALID_NETWORK_ID || it.first.original_network_id == INVALID_NETWORK_ID || onet_id == it.first.original_network_id))
            {
                addLCN(it.second.logical_channel_number, it.first.service_id, it.first.transport_stream_id, it.first.original_network_id, it.second.visible_service_flag);
                count++;
            }
        }
    }
    return count;
}


//----------------------------------------------------------------------------
// Get the logical channel number of a service.
//----------------------------------------------------------------------------

uint16_t ts::LogicalChannelNumbers::getLCN(const ServiceIdTriplet& srv) const
{
    return getLCN(srv.service_id, srv.transport_stream_id, srv.original_network_id);
}

uint16_t ts::LogicalChannelNumbers::getLCN(uint16_t srv_id, uint16_t ts_id, uint16_t onet_id) const
{
    const auto it = findLCN(srv_id, ts_id, onet_id);
    return it == _lcn_map.end() ? 0xFFFF : it->second.lcn;
}

bool ts::LogicalChannelNumbers::getVisible(const ServiceIdTriplet& srv) const
{
    return getVisible(srv.service_id, srv.transport_stream_id, srv.original_network_id);
}

bool ts::LogicalChannelNumbers::getVisible(uint16_t srv_id, uint16_t ts_id, uint16_t onet_id) const
{
    const auto it = findLCN(srv_id, ts_id, onet_id);
    return it == _lcn_map.end() ? true : it->second.visible;
}

ts::LogicalChannelNumbers::LCNMap::const_iterator ts::LogicalChannelNumbers::findLCN(uint16_t srv_id, uint16_t ts_id, uint16_t onet_id) const
{
    LCNMap::const_iterator result = _lcn_map.end();
    for (LCNMap::const_iterator it = _lcn_map.lower_bound(srv_id); it != _lcn_map.end() && it->first == srv_id; ++it) {
        if (it->second.ts_id == ts_id) {
            if (it->second.onet_id == onet_id) {
                // Found an exact match, including if both are INVALID_NETWORK_ID, final value.
                return it;
            }
            else if (it->second.onet_id == INVALID_NETWORK_ID || onet_id == INVALID_NETWORK_ID) {
                // Possible match, keep it but continue to search an exact match.
                result = it;
            }
        }
    }
    return result;
}


//----------------------------------------------------------------------------
// Get the set of all services in this object.
//----------------------------------------------------------------------------

void ts::LogicalChannelNumbers::addServices(std::set<ServiceIdTriplet>& services) const
{
    for (const auto& it : _lcn_map) {
        services.insert(ServiceIdTriplet(it.first, it.second.ts_id, it.second.onet_id));
    }
}


//----------------------------------------------------------------------------
// Update a service description with its LCN.
//----------------------------------------------------------------------------

bool ts::LogicalChannelNumbers::updateService(Service& srv, bool replace) const
{
    if (srv.hasId() && srv.hasTSId() && (replace || !srv.hasLCN())) {
        const uint16_t onid = srv.hasONId() ? srv.getONId() : INVALID_NETWORK_ID;
        const auto it = findLCN(srv.getId(), srv.getTSId(), onid);
        if (it != _lcn_map.end()) {
            srv.setLCN(it->second.lcn);
            srv.setHidden(it->second.visible);
            return true;
        }
    }
    return false;
}
