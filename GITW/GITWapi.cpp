//---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITWapi
//
// Description: Implementation
//
// Author: Eliot Muir
// Date:   Tue 14 Feb 2023 14:43:01 EST
//---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <GITW/GITWapi.h>
#include <GITW/GITWcache.h>
#include <GITW/GITWcacheTimestamps.h>
#include <GITW/GITWcatalogueAdd.h>
#include <GITW/GITWcatalogueSave.h>
#include <GITW/GITWcollectionEdit.h>
#include <GITW/GITWcollectionSave.h>
#include <GITW/GITWcollections.h>
#include <GITW/GITWcreateRepo.h>
#include <GITW/GITWisWritable.h>
#include <GITW/GITWparse.h>
#include <GITW/GITWtemplatesRefresh.h>
COL_LOG_MODULE;

void GITWapiAsync(COLhashmap<COLstring, COLwebHandlerFunc>& ApiMap){
   COL_FUNCTION(GITWapiAsync);
   ApiMap.add("/git/catalogue/save",    &GITWcatalogueSave);
   ApiMap.add("/git/catalogue/add",     &GITWcatalogueAdd);
   ApiMap.add("/git/url/parse",         &GITWparse);
	ApiMap.add("/git/writable",          &GITWisWritable);
	ApiMap.add("/git/catalogue",         &GITWcollections);
	ApiMap.add("/git/cache",             &GITWcache);
	ApiMap.add("/git/collection/edit",   &GITWcollectionEdit);
	ApiMap.add("/git/collection/save",   &GITWcollectionSave);
	ApiMap.add("/git/create/repo",       &GITWcreateRepo);
   ApiMap.add("/git/cache/timestamps",  &GITWcacheTimestamps);
}

void GITWapi(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>>>& ApiMap, bool* pGitCacheRefreshing, SCKloop* pLoop) {
   COL_FUNCTION(GITWapi);
   ApiMap.add("/git/cache/refresh", COLnewClosure1(&GITWtemplatesRefresh, pGitCacheRefreshing, pLoop));
}
