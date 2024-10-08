//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2024, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------

#include "tsDeliverySystem.h"


//----------------------------------------------------------------------------
// A classification of delivery systems.
//----------------------------------------------------------------------------

// List of delivery systems, from most preferred to least preferred.
// This list is used to find the default delivery system of a tuner and
// to build the list of supported delivery systems in order of preference.
TS_STATIC_INSTANCE(const, ts::DeliverySystemList, PreferredOrder, ({
     // On a tuner, we consider terrestrial capabilities first.
     ts::DS_DVB_T,
     ts::DS_DVB_T2,
     ts::DS_ATSC,
     ts::DS_ISDB_T,
     ts::DS_DTMB,
     ts::DS_CMMB,
     // Then satellite capabilities.
     ts::DS_DVB_S,
     ts::DS_DVB_S2,
     ts::DS_DVB_S_TURBO,
     ts::DS_ISDB_S,
     ts::DS_DSS,
     // Then cable capabilities.
     ts::DS_DVB_C_ANNEX_A,
     ts::DS_DVB_C_ANNEX_B,
     ts::DS_DVB_C_ANNEX_C,
     ts::DS_DVB_C2,
     ts::DS_ISDB_C,
     // Exotic capabilities come last.
     ts::DS_DVB_H,
     ts::DS_ATSC_MH,
     ts::DS_DAB,
     ts::DS_UNDEFINED
}));

namespace {

    // Classification of delivery systems (bit flags).
    enum : uint16_t {
        DSF_TERRESTRIAL = 0x0001,
        DSF_SATELLITE   = 0x0002,
        DSF_CABLE       = 0x0004,
    };

    // Description of one delivery system.
    struct DeliverySystemDescription {
        ts::TunerType type;
        ts::Standards standards;
        uint32_t      flags;
    };
    using DeliverySystemDescriptionMap = std::map<ts::DeliverySystem, DeliverySystemDescription>;

    TS_STATIC_INSTANCE(const, DeliverySystemDescriptionMap, DelSysDescs, ({
        {ts::DS_UNDEFINED,     {ts::TT_UNDEFINED, ts::Standards::NONE, 0}},
        {ts::DS_DVB_S,         {ts::TT_DVB_S,     ts::Standards::DVB,  DSF_SATELLITE}},
        {ts::DS_DVB_S2,        {ts::TT_DVB_S,     ts::Standards::DVB,  DSF_SATELLITE}},
        {ts::DS_DVB_S_TURBO,   {ts::TT_DVB_S,     ts::Standards::DVB,  DSF_SATELLITE}},
        {ts::DS_DVB_T,         {ts::TT_DVB_T,     ts::Standards::DVB,  DSF_TERRESTRIAL}},
        {ts::DS_DVB_T2,        {ts::TT_DVB_T,     ts::Standards::DVB,  DSF_TERRESTRIAL}},
        {ts::DS_DVB_C_ANNEX_A, {ts::TT_DVB_C,     ts::Standards::DVB,  DSF_CABLE}},
        {ts::DS_DVB_C_ANNEX_B, {ts::TT_DVB_C,     ts::Standards::DVB,  DSF_CABLE}},
        {ts::DS_DVB_C_ANNEX_C, {ts::TT_DVB_C,     ts::Standards::DVB,  DSF_CABLE}},
        {ts::DS_DVB_C2,        {ts::TT_DVB_C,     ts::Standards::DVB,  DSF_CABLE}},
        {ts::DS_DVB_H,         {ts::TT_UNDEFINED, ts::Standards::DVB,  0}},
        {ts::DS_ISDB_S,        {ts::TT_ISDB_S,    ts::Standards::ISDB, DSF_SATELLITE}},
        {ts::DS_ISDB_T,        {ts::TT_ISDB_T,    ts::Standards::ISDB, DSF_TERRESTRIAL}},
        {ts::DS_ISDB_C,        {ts::TT_ISDB_C,    ts::Standards::ISDB, DSF_CABLE}},
        {ts::DS_ATSC,          {ts::TT_ATSC,      ts::Standards::ATSC, DSF_TERRESTRIAL | DSF_CABLE}},
        {ts::DS_ATSC_MH,       {ts::TT_UNDEFINED, ts::Standards::ATSC, 0}},
        {ts::DS_DTMB,          {ts::TT_UNDEFINED, ts::Standards::NONE, DSF_TERRESTRIAL}},
        {ts::DS_CMMB,          {ts::TT_UNDEFINED, ts::Standards::NONE, DSF_TERRESTRIAL}},
        {ts::DS_DAB,           {ts::TT_UNDEFINED, ts::Standards::NONE, 0}},
        {ts::DS_DSS,           {ts::TT_UNDEFINED, ts::Standards::NONE, DSF_SATELLITE}},
    }));
}


//----------------------------------------------------------------------------
// Enumerations, names for values
//----------------------------------------------------------------------------

TS_DEFINE_GLOBAL(const, ts::Enumeration, ts::DeliverySystemEnum, ({
    {u"undefined",   ts::DS_UNDEFINED},
    {u"DVB-S",       ts::DS_DVB_S},
    {u"DVB-S2",      ts::DS_DVB_S2},
    {u"DVB-S-Turbo", ts::DS_DVB_S_TURBO},
    {u"DVB-T",       ts::DS_DVB_T},
    {u"DVB-T2",      ts::DS_DVB_T2},
    {u"DVB-C",       ts::DS_DVB_C}, // a synonym for DS_DVB_C_ANNEX_A
    {u"DVB-C/A",     ts::DS_DVB_C_ANNEX_A},
    {u"DVB-C/B",     ts::DS_DVB_C_ANNEX_B},
    {u"DVB-C/C",     ts::DS_DVB_C_ANNEX_C},
    {u"DVB-C2",      ts::DS_DVB_C2},
    {u"DVB-H",       ts::DS_DVB_H},
    {u"ISDB-S",      ts::DS_ISDB_S},
    {u"ISDB-T",      ts::DS_ISDB_T},
    {u"ISDB-C",      ts::DS_ISDB_C},
    {u"ATSC",        ts::DS_ATSC},
    {u"ATSC-MH",     ts::DS_ATSC_MH},
    {u"DTMB",        ts::DS_DTMB},
    {u"CMMB",        ts::DS_CMMB},
    {u"DAB",         ts::DS_DAB},
    {u"DSS",         ts::DS_DSS},
}));

TS_DEFINE_GLOBAL(const, ts::Enumeration, ts::TunerTypeEnum, ({
    {u"DVB-S",  ts::TT_DVB_S},
    {u"DVB-T",  ts::TT_DVB_T},
    {u"DVB-C",  ts::TT_DVB_C},
    {u"ISDB-S", ts::TT_ISDB_S},
    {u"ISDB-T", ts::TT_ISDB_T},
    {u"ISDB-C", ts::TT_ISDB_C},
    {u"ATSC",   ts::TT_ATSC},
}));


//----------------------------------------------------------------------------
// Check if a delivery system is a satellite or terrestrial one.
//----------------------------------------------------------------------------

bool ts::IsSatelliteDelivery(DeliverySystem sys)
{
    const auto it = DelSysDescs->find(sys);
    return it != DelSysDescs->end() && (it->second.flags & DSF_SATELLITE) != 0;
}

bool ts::IsTerrestrialDelivery(DeliverySystem sys)
{
    const auto it = DelSysDescs->find(sys);
    return it != DelSysDescs->end() && (it->second.flags & DSF_TERRESTRIAL) != 0;
}


//----------------------------------------------------------------------------
// Get the tuner type of a delivery system.
//----------------------------------------------------------------------------

ts::TunerType ts::TunerTypeOf(ts::DeliverySystem system)
{
    const auto it = DelSysDescs->find(system);
    return it != DelSysDescs->end() ? it->second.type : TT_UNDEFINED;
}


//----------------------------------------------------------------------------
// Get the list of standards for a delivery system.
//----------------------------------------------------------------------------

ts::Standards ts::StandardsOf(DeliverySystem system)
{
    const auto it = DelSysDescs->find(system);
    return it != DelSysDescs->end() ? it->second.standards : Standards::NONE;
}


//----------------------------------------------------------------------------
// Delivery system sets.
//----------------------------------------------------------------------------

ts::DeliverySystem ts::DeliverySystemSet::preferred() const
{
    // Inspect delivery systems in decreasing order of preference.
    for (auto it : *PreferredOrder) {
        if (contains(it)) {
            return it;
        }
    }
    return DS_UNDEFINED;
}

ts::DeliverySystemList ts::DeliverySystemSet::toList() const
{
    DeliverySystemList list;
    for (auto it : *PreferredOrder) {
        if (contains(it)) {
            list.push_back(it);
        }
    }
    return list;
}

ts::Standards ts::DeliverySystemSet::standards() const
{
    Standards st = Standards::NONE;
    for (auto it : *this) {
        st |= StandardsOf(it);
    }
    return st;
}

void ts::DeliverySystemSet::insertAll(TunerType type)
{
    for (const auto& it : *DelSysDescs) {
        if (it.second.type == type) {
            insert(it.first);
        }
    }
}

ts::UString ts::DeliverySystemSet::toString() const
{
    UString str;
    // Build list of delivery systems in decreasing order of preference.
    for (auto it : *PreferredOrder) {
        if (contains(it)) {
            if (!str.empty()) {
                str += u", ";
            }
            str += DeliverySystemEnum->name(int(it));
        }
    }
    return str.empty() ? u"none" : str;
}
