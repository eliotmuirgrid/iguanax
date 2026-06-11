//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: USRmap
//
// Description:
//
// Implementation
//
// Author: Matthew Sobkowski
// Date:   Mon 29 Mar 2021 11:13:50 EDT
//---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLmap.h>
#include <COL/COLvar.h>
#include <USR/USRmap.h>
COL_LOG_MODULE;

COLvar USRmapToVarUsers(COLmap<COLstring, USRuser>* pUserMap, const bool withPassword) {
   COL_FUNCTION(USRmapToVarUsers);
   COLvar Result;
   Result.setMapType();
   for(const auto& it : *pUserMap) { Result[it->first] = it->second.toVar(withPassword); }
   return Result;
}

COLvar USRmapToVarLocalUsers(COLmap<COLstring, USRuser>* pUserMap, const bool withPassword) {
   COL_FUNCTION(USRmapToVarLocalUsers);
   COLvar Result;
   Result.setMapType();  // in case we have zero users.
   for(const auto& i : *pUserMap) {
      if(!i.second.isExternalUser()) { Result[i.first] = i.second.toVar(withPassword); }
   }
   return Result;
}

bool USRmapAuthenticate(COLmap<COLstring, USRuser>*  pUserMap, const COLstring& Username, const COLstring& Password){
   COL_FUNCTION(USRmapAuthenticate);
   COL_VAR2(Username, Password);
   if(!Username.is_null() && pUserMap->count(Username) > 0) {
      return (*pUserMap)[Username].authenticate(Password);
   }
   return false;
}
