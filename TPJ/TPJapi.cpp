//---------------------------------------------------------------------------
// Copyright (C) 1997-2020 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: TPJapi
//
// Author: Tyer Brown
//
// Description:
//
// Implementation
//---------------------------------------------------------------------------
#include "TPJapi.h"
#include "TPJupdateTree.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

void TPJapi(COLhashmap<COLstring, COLwebHandlerFunc>& ApiMap) {
   COL_FUNCTION(TPJapi);
   ApiMap.add("/ide/project_tree", &TPJupdateProjectTree);
}