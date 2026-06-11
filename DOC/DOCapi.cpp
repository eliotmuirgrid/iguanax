// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DOCapi
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Sunday 24 August 2025 - 03:09PM
// ---------------------------------------------------------------------------

#include <DOC/DOCapi.h>
#include <DOC/DOChex.h>
#include <DOC/DOCindexCollection.h>
#include <DOC/DOCdocumentGet.h>
#include <DOC/DOCdocumentSet.h>
#include <DOC/DOCrandomTest.h>
#include <DOC/DOCsourceQuery.h>
#include <DOC/DOCtagTree.h>
#include <DOC/DOCdownload.h>
#include <DOC/DOCdocumentAdd.h>
#include <DOC/DOCdocumentDelete.h>
#include <DOC/DOCsearch.h>
#include <DOC/DOCsearchThreaded.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void DOCapi(COLhashmap<COLstring, COLwebHandlerFunc>& ApiMap){
   COL_FUNCTION(DOCapi);
   ApiMap.add("/doc/tag",             &DOCtagTree);
   ApiMap.add("/doc/random/generate", &DOCrandomTest);
   ApiMap.add("/doc/index",           &DOCindexCollection);
   ApiMap.add("/doc/hex/toint",       &DOCwebHexStringToInt);
   ApiMap.add("/doc/get",             &DOCwebDocumentGet);
   ApiMap.add("/doc/set",             &DOCwebDocumentSet);
   ApiMap.add("/doc/download",        &DOCdownload);
   ApiMap.add("/doc/upload",          &DOCwebDocumentUpload);
   ApiMap.add("/doc/delete",          &DOCwebDocumentDelete);
   ApiMap.add("/doc/search",          &DOCwebDocumentSearch);
   ApiMap.add("/doc/search/threaded", &DOCwebSearchDirectionally);
}


void DOCapiA(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >& ApiMap, BLUAinstanceManager* pBluaManager){
   COL_FUNCTION(DOCapiA);
   ApiMap.add("/doc/source/query", COLnewClosure1(&DOCsourceQuery, pBluaManager));
}
