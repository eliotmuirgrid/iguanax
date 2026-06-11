#ifndef __TRNC_APIS_H__
#define __TRNC_APIS_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNConfig
//
// Description:
//
// APIs for adding, removing and configuring of translators.
//
// Author: Shuo Hou
// Date:   Tue August 31 2021
//---------------------------------------------------------------------------
#include <COL/COLweb.h>
#include <COL/COLhashmap.h>
#include <COL/COLstring.h>

struct CFGmap;
class SCKloop;

void TRNCapi(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >& ApiMap, SCKloop* pLoop, CFGmap* pMap);

void TRNCworker(COLhashmap<COLstring, COLwebHandlerFunc>& ApiMap);

#endif  // end of defensive include