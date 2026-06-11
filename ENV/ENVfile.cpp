//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: ENVfile.cpp
//
//  Description:
//
//  Implementation
//
//  Author: Matthew Sobkowski
//  Date:   3/27/23 12:54 PM
//  ---------------------------------------------------------------------------
#include <ENV/ENVfile.h>
#include <ENV/ENVparse.h>

#include <DIR/DIRpath.h>

#include <FIL/FILutils.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void ENVfileToVar(const ENVmap& EnvMap, COLvar* pOut) {
   COL_FUNCTION(ENVfileToVar);
   pOut->setMapType();
   for(const auto& it : EnvMap.Map) {
      COL_DBG(it->first << "=" << it->second);
      (*pOut)[it->first] = it->second;
   }
}

void ENVfileLoad(ENVmap* pEnvMap) {
   COL_FUNCTION(ENVfileLoad);
   COLstring EnvString, Err;
   COLstring IguanaEnv = DIRfileConfigEnvironment();
   if(!FILfileExists(IguanaEnv)) { return; }
   FILreadFile(IguanaEnv, &EnvString);
   ENVparse(&EnvString, pEnvMap, &Err);
}

void ENVfileSave(const ENVmap& EnvMap) {
   COL_FUNCTION(ENVfileSave);
   COLstring EnvString;
   for(const auto& it : EnvMap.Map) {
      COLstring Line = it->first + "=" + it->second + "\n";
      COL_TRC(Line);
      EnvString.append(Line);
   }
	COLstring Error;
	bool Success = FILsave(DIRfileConfigEnvironment(), EnvString, &Error);
	COL_VAR2(Success, Error);
}