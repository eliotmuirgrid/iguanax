// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: WEBapi
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Thursday 25 May 2023 - 09:30PM
// ---------------------------------------------------------------------------

#include <WEB/WEBapi.h>
#include <WEB/WEBlogout.h>
#include <WEB/WEBlogin.h>
#include <WEB/WEBsession.h>

#include <USR/USRuser.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void WEBapi(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >& ApiMap, COLmap<COLstring, WEBsession>* pSessionMap,
   COLmap<COLstring, USRuser>* pUserMap, COLclosure3<COLstring, COLstring, COLclosure1<COLvar>*>* pExternalAuthenticator) {
   COL_FUNCTION(WEBapi);
   ApiMap.add("/session/logout", COLnewClosure1(&WEBlogout, pSessionMap, pUserMap));
   ApiMap.add("/session/login",  COLnewClosure1(&WEBlogin,  pSessionMap, pUserMap, pExternalAuthenticator));
}