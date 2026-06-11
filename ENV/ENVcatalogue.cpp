// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: ENVcatalogue
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Thursday 23 March 2023 - 11:50AM
// ---------------------------------------------------------------------------
#include <ENV/ENVcatalogue.h>
#include <ENV/ENVfile.h>
#include <ENV/ENVparse.h>

#include <PRO/PROenv.h>

#include <COL/COLvar.h>
#include <COL/COLlog.h>
#include <COL/COLweb.h>
COL_LOG_MODULE;

static void ENVremoveDuplicates(const ENVmap& EnvMap, COLvar* pOut) {
   COL_FUNCTION(ENVremoveDuplicates);
   for(const auto& it : EnvMap.Map) {
      if(pOut->exists(it->first)) {
         COL_TRC("Found duplicate variable -- " << it->first << " -- removing from output");
         pOut->erase(it->first);
      }
   }
}

// /environment/all
void ENVcatalogue(const COLwebRequest& Request){
   COL_FUNCTION(ENVcatalogue);
   COLvar Result;
   PROfullEnv(&Result["system"]);
   ENVmap Map;
   ENVfileLoad(&Map);
   ENVfileToVar(Map, &Result["local"]);
   COL_VAR(Result.json(1));
   ENVremoveDuplicates(Map, &Result["system"]); // local overrides system
   COLrespondSuccess(Request.Address, Result);
}

void ENVretrieveAll(COLvar* pResult){
   COL_FUNCTION(ENVretrieveAll);
   COLvar& Result = *pResult;
   PROfullEnv(&Result);
   ENVmap Map;
   ENVfileLoad(&Map);
   ENVfileToVar(Map, &Result); // The local vars will overwrite any system var with the same name
   COL_VAR(Result.json(1));
}