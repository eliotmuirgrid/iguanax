//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LCOMfields
//
// Implementation
//
// Author: Matthew Sobkowski
// Date:   Monday 24 April 2023 - 3:59PM
// ---------------------------------------------------------------------------
#include <LCOM/LCOMgetFields.h>

#include <CFG/CFGconfig.h>
#include <CFG/CFGconfigLoad.h>

#include <CFG/CFGfieldGrammar.h>
#include <CFG/CFGfieldSanitize.h>

#include <FIL/FILpathUtils.h>
#include <LENV/LENVcache.h>
#include <LUA/LUAerror.h>
#include <LUA/LUAheader.h>
#include <LUA/LUAutils.h>
#include <LUA/LUAopen.h>

#include <PASS/PASSword.h>

#include <COL/COLauto.h>
#include <COL/COLlog.h>
#include <COL/COLutils.h>
#include <COL/COLvar.h>
COL_LOG_MODULE;

static COLstring LCOMresolveFieldValue(const CFGfield& Field, const COLvar& CachedEnvVars, const COLmap<COLstring, COLstring>& Values) {
   COL_FUNCTION(LCOMresolveFieldValue);
   if (!Values.count(Field.Name) || Values[Field.Name].is_null()) {
      COL_TRC("Value not set by user -- using default");
      return COLexpandEnvironmentVariablesUsingCache(Field.Default, CachedEnvVars);
   }
   COLstring Value = COLexpandEnvironmentVariablesUsingCache(Values[Field.Name], CachedEnvVars);
   COLstring Error;
   if (!CFGfieldsSanitizeValue(Field.Name, Field.FieldType, Value, &Error)) {
      COL_TRC("Sanitization failed  -- " << Field.Name << ":" << Value << " Error=" << Error);
      return COLexpandEnvironmentVariablesUsingCache(Field.Default, CachedEnvVars);
   }
   return Value;
}

static COLmap<COLstring, COLpair<COLstring, CFGfieldType>> LCOMmapValues(const COLarray<CFGfield>& Grammar, const COLvar& CachedEnvVars, const COLmap<COLstring, COLstring>& Values) {
   COL_FUNCTION(LCOMmapValues);
   COL_VAR2(Grammar.size(), Values.size());
   COLmap<COLstring, COLpair<COLstring, CFGfieldType>> Map;
   for(const auto& Field : Grammar) {
      COLpair<COLstring, CFGfieldType> Data;
      Data.first = LCOMresolveFieldValue(Field, CachedEnvVars, Values);
      Data.second = Field.FieldType;
      Map.add(Field.Name, Data);
      COL_VAR2(Field.Name, Data.first);
   }
   return Map;
}

static bool LCOMfieldsLoad(const COLstring& Component, const COLstring& Root, COLarray<CFGfield>* pGrammar, CFGconfig* pConfig, COLvar* pOut) {
   COL_FUNCTION(LCOMfieldsLoad);
   COLstring Err;
   bool Success = CFGfieldLoadGrammar(Root + "config.json", pGrammar, &Err);
   if(Success) { Success = CFGconfigLoad(Component, pConfig); }
   if(!Success) {
      COL_TRC("Failed loading field data");
      (*pOut)["error"] = Err;
   }
   return Success;
}

static void LCOMfieldsMerge(const COLstring& Component, const COLstring& Root, const COLvar& CachedEnvVars, COLvar* pFieldVar){
   COL_FUNCTION(LCOMfieldsMerge);
   COLvar& FieldVar = *pFieldVar;
   FieldVar.setMapType();
   COLarray<CFGfield> Grammar;
	CFGconfig Config;
   if(LCOMfieldsLoad(Component, Root, &Grammar, &Config, &FieldVar)) {
      COLmap<COLstring, COLpair<COLstring, CFGfieldType>> Fields = LCOMmapValues(Grammar, CachedEnvVars, Config.CoreConfig.Fields);
      for(const auto& it : Fields) {
         COL_VAR3(it->first, it->second.first, CFGfieldTypeToString(it->second.second));
         COLstring Value = it->second.first;
         switch(it->second.second) {
            case CFGfieldType::NUMBER  : FieldVar[it->first] = atof(Value.c_str());         break;
            case CFGfieldType::PORT    : FieldVar[it->first] = atoi(Value.c_str());         break;

            case CFGfieldType::FOLDER  :
            case CFGfieldType::FILE    : FieldVar[it->first] = FILexpand(Value);            break;

            case CFGfieldType::BOOL    : FieldVar[it->first] = Value.equals("true");        break;

            case CFGfieldType::STRING  : FieldVar[it->first] = Value;                       break;
         	case CFGfieldType::List    : FieldVar[it->first] = Value;							  break;

            case CFGfieldType::PASSWORD: FieldVar[it->first] = Value;                       break;

            default:                     FieldVar[it->first] = "Error: unknown field type"; break;
         }
      }
   }
}

// TODO - we probably should default to an empty set.

int LCOMgetFields(lua_State* L){
   COL_FUNCTION(LCOMgetFields);
   COLstring Guid  = LUAcomponentId(L);
   COLstring Root  = LUAprojectRoot(L);
   COLvar CachedEnvVars;
   LENVgetCache(L, &CachedEnvVars);
   COL_VAR(CachedEnvVars);
   COL_VAR2(Guid, Root);
   COLvar          FieldVar;
   LCOMfieldsMerge(Guid, Root, CachedEnvVars, &FieldVar);
   COL_VAR(FieldVar.json(1));
   FieldVar.toLua(L);
   return 1;
}
