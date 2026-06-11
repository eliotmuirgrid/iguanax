#ifndef _SDBSRETRIEVESYNC_H
#define _SDBSRETRIEVESYNC_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SDBSretrieveSync.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   2025-08-21, 10:57a.m.
//  ---------------------------------------------------------------------------
class COLaddress;
class SDBfilterInfo2;
class SDBUparams;
class SCKloop;

void SDBSretrieveSyncImpl(SDBUparams Params, std::shared_ptr<SDBSretrieveItem> pReport, COLclosure0* pCallback);

void SDBSretrieveSync(const SDBUparams& Params, SCKloop* pLoop, COLaddress Address);

#endif  // end of defensive include
