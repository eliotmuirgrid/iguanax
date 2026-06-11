//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: LCOMsetField.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   23/01/24 11:50 AM
//  ---------------------------------------------------------------------------
#include <LCOM/LCOMsetField.h>

#include <CFG/CFGconfig.h>
#include <CFG/CFGconfigLoad.h>
#include <CFG/CFGfieldGrammar.h>
#include <CFG/CFGfieldSanitize.h>

#include <LUA/LUAerror.h>
#include <LUA/LUAtable.h>
#include <LUA/LUAopen.h>

#include <SCK/SCKcall.h>

#include <COL/COLlog.h>
#include <COL/COLvar.h>
COL_LOG_MODULE;

static bool LCOMsetRequestField(lua_State* L, const COLstring& Key, const COLarray<CFGfield>& Grammar, COLvar* pRequest, COLstring* pErr) {
   COL_FUNCTION(LCOMsetRequestField);
   for (const auto& it : Grammar) {
      if (it.Name == Key) {
         switch (it.FieldType) {
            case CFGfieldType::BOOL:
               (*pRequest)["value"] = LUAexpectedBoolParam(L, 1, "value");
               return true;
            case CFGfieldType::NUMBER:
               (*pRequest)["value"] = LUAexpectedIntParam(L, 1, "value");
               return true;
            default:
               (*pRequest)["value"] = LUAexpectedStringParam(L, 1, "value");
               return true;
         }
      }
   }
   COL_TRC("Didn't find key");
   *pErr = "Failed to set value: there is no custom field named " + Key;
   return false;
}

int LCOMsetField(lua_State* L) {
   COL_FUNCTION(LCOMsetField);
   const COLstring Component = LUAcomponentId(L);
   const COLstring Key       = LUAexpectedStringParam(L, 1, "key");
   const COLstring JsonFile  = LUAprojectRoot(L) + "config.json";
   COL_VAR3(Component, Key, JsonFile);
   COLstring Err;
   COLarray<CFGfield> Grammar;
	CFGconfig Config;
   if(!CFGfieldLoadGrammar(JsonFile, &Grammar, &Err)) { return LUAraiseLuaError(L, Err); }
	if(!CFGconfigLoad(Component, &Config))             { return LUAraiseLuaError(L, "Failed loading config file"); }
   COLvar Request, Result;
   if(!LCOMsetRequestField(L, Key, Grammar, &Request, &Err)) { return LUAraiseLuaError(L, Err); }
   Request["component"] = Component;
   Request["name"]      = Key;
   bool StatusSetResult = SCKcall("/component/fields/set", Request, Component, &Result); // sanitization happens here
   COL_VAR2(StatusSetResult, Result.json(1));
   if (!StatusSetResult || Result.exists("error")) {LUAraiseLuaError(L, "The " + Result["error"].asString() + " for this commit. Please switch to the correct commit or DEVELOPMENT.");}
   return 0;
}