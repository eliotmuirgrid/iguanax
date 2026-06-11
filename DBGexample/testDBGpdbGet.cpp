// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testDBGpdbGet
//
// Description:
//
// Implementation of tests
//
// Author: Eliot Muir 
// Date:   Sunday 13 August 2023 - 09:12AM
// ---------------------------------------------------------------------------
#include "testDBGpdbGet.h"

#include <UNIT/UNITapp.h>

#include <DBG/DBGdownloadPdb.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

static void testDownloadUrl(){
   COL_FUNCTION(testDownloadUrl);
   // Abandoned this approach.
   //COLstring Url = DBGpdpUrl();
   //COL_VAR(Url);
   //COLstring Error;
   //DBGpdbGet(&Error);
}

void testDBGpdbGet(UNITapp* pApp) {
   pApp->add("download/url", &testDownloadUrl);
}
