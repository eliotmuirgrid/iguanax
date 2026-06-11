#ifndef __TRNC_CLEANUP_H__
#define __TRNC_CLEANUP_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: TRNCcleanup
//
// Author: Matthew Sobkowski
//
// Routines to clean up components which fail.
//---------------------------------------------------------------------------
#include <COL/COLarray.h>
struct CFGmap;

// IX-786 Implement these.
void TRNCarchiveComponent(const COLstring& ComponentId);
void TRNCarchive(const COLarray<COLstring>& InvalidComponents, const COLstring& Reason);
void TRNClog(const COLstring& ComponentId, const COLstring& Reason);
void TRNCremoveComponentsFromMap(const COLarray<COLstring>& InvalidComponents, CFGmap* pMap);

#endif // end of defensive include
