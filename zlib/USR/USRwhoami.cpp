//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: USRwhoami.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <USR/USRwhoami.h>
#include <USR/USRuser.h>
#include <COL/COLweb.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

// /user/whoami
void USRwhoami(const COLwebRequest& Request, COLmap<COLstring, USRuser>* pUserMap) {
   COL_FUNCTION(USRwhoami);
   if(pUserMap->count(Request.User) == 0) {
      COL_TRC("Unknown user: " << Request.User << "?");
      return COLrespondError(Request.Address, "Unknown user: " + Request.User);
   }
   COLrespondSuccess(Request.Address, (*pUserMap)[Request.User].toVar(false));
}