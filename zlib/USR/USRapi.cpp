//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: USRapi
//
// Description:
//
// API for user framework.
//
// Author: Matthew Sobkowski
// Date:   Mon 29 Mar 2021 11:13:50 EDT
//---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <SCK/SCKloop.h>
#include <USR/USRadd.h>
#include <USR/USRadminUnsafe.h>
#include <USR/USRapi.h>
#include <USR/USRdelete.h>
#include <USR/USRlist.h>
#include <USR/USRneeded.h>
#include <USR/USRsetPassword.h>
#include <USR/USRsetRoles.h>
#include <USR/USRuser.h>
#include <USR/USRwhoami.h>
COL_LOG_MODULE;

void USRapiStartup(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >& ApiMap, SCKloop* pLoop, COLmap<COLstring, USRuser>* pUserMap) {
   COL_FUNCTION(USRapiStartup);
   ApiMap.add("/user/add",    COLnewClosure1(&USRadd, pUserMap, pLoop));
   ApiMap.add("/user/needed", COLnewClosure1(&USRneeded, pUserMap));
}

void USRapi(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>>>& ApiMap, SCKloop* pLoop, COLmap<COLstring, USRuser>* pUserMap) {
   COL_FUNCTION(USRapi);
   ApiMap.add("/user/add",          COLnewClosure1(&USRadd,             pUserMap, pLoop));
   ApiMap.add("/user/set/roles",    COLnewClosure1(&USRsetRoles,        pUserMap, pLoop));
   ApiMap.add("/user/set/password", COLnewClosure1(&USRsetPassword,     pUserMap, pLoop));
   ApiMap.add("/user/delete",       COLnewClosure1(&USRdelete,          pUserMap, pLoop));
   ApiMap.add("/user/list",         COLnewClosure1(&USRlist,            pUserMap));
   ApiMap.add("/user/admin/unsafe", COLnewClosure1(&USRadminUnsafe,     pUserMap));
   ApiMap.add("/user/whoami",       COLnewClosure1(&USRwhoami,          pUserMap));
}