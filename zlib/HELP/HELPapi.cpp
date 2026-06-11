//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: HELPapi
//
// Description:
//
// Implementation
//
// Author: Orkhan Amikishiyev
//---------------------------------------------------------------------------
#include <HELP/HELPapi.h>
#include <HELP/HELPload.h>
#include <HELP/HELPlist.h>
#include <HELP/HELPsave.h>
#include <HELP/HELPdirRoot.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void HELPapiWorker(COLhashmap<COLstring, COLwebHandlerFunc>& ApiMap) {
   COL_FUNCTION(HELPapiWorker);
   ApiMap.add("/help/dir/root",      &HELPdirRoot);
   ApiMap.add("/component/help",     &HELPload);
   ApiMap.add("/component/help/set", &HELPsave);
   ApiMap.add("/help/index",         &HELPlist);
}