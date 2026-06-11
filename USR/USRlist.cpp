//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: USRlist
//
// Description:
//
// API for listing all users
//
// Author: Matthew Sobkowski
// Date:   Mon 29 Mar 2021 11:13:50 EDT
//---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLweb.h>
#include <USR/USRlist.h>
#include <USR/USRmap.h>
COL_LOG_MODULE;

// /user/list
void USRlist(const COLwebRequest& Request, COLmap<COLstring, USRuser>*  pUserMap) {
   COL_FUNCTION(USRlist);
   COL_VAR(Request.SessionId);
   COLrespondSuccess(Request.Address, USRmapToVarLocalUsers(pUserMap, false));
}