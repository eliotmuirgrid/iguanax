// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: USRadminUnsafe
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Tuesday 29 August 2023 - 03:14PM
// ---------------------------------------------------------------------------

#include <USR/USRadminUnsafe.h>
#include <USR/USRuser.h>

#include <COL/COLweb.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;


bool USRadminUnsafeImpl(COLmap<COLstring, USRuser>* pUserMap){
   COL_FUNCTION(USRadminUnsafeImpl);
   if(!pUserMap->count("admin")){
      COL_TRC("No admin account so no admin account password is password warning");
      return false;
   }
   USRuser& User = (*pUserMap)["admin"];
   bool Unsafe = User.authenticate("password");
   COL_VAR(Unsafe);
   return Unsafe;
}

void USRadminUnsafe(const COLwebRequest& Request, COLmap<COLstring, USRuser>*  pUserMap){
   COL_FUNCTION(USRadminUnsafe);
   COLvar Result;
   bool Unsafe = USRadminUnsafeImpl(pUserMap);
   Result["unsafe"]     = Unsafe;
   Result["only_admin"] = Unsafe && pUserMap->size() == 1;
   COLrespondSuccess(Request.Address, Result);
}