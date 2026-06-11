//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: CMPCfieldsSet.cpp
//
//  Description:
//
//  Implementation
//
//  Author: Matthew Sobkowski
//  Date:   4/19/23 2:07 PM
//  ---------------------------------------------------------------------------
#include <CFG/CFGconfig.h>
#include <CFG/CFGconfigApi.h>
#include <CFG/CFGconfigLoad.h>
#include <CFG/CFGconfigSave.h>
#include <CFG/CFGfieldGrammar.h>
#include <CFG/CFGmapApi.h>
#include <CMPC/CMPCfieldsSet.h>
#include <CMPC/CMPCutils.h>
#include <COL/COLlog.h>
#include <COL/COLutils.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <FIL/FILutils.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
#include <SCK/SCKloop.h>
#include <SCK/SCKworker.h>
COL_LOG_MODULE;

static void CMPCfieldsSetError(const COLstring& Err, COLvar* pOut) {
   (*pOut)["success"] = false;
   (*pOut)["error"]   = Err;
}

static void CMPCfieldsSetFinish(COLvar Result, CFGconfig* pConfig, COLstring Name, COLstring Value,
                                COLaddress Address) {
   COL_VAR(Result);
   if(!Result["success"].asBool()) { return COLrespondError(Address, Result["error"].asString()); }
   pConfig->CoreConfig.Fields[Name] = Value;
   COLrespondSuccess(Address);
}

static void CMPCfieldsSetWorker(const COLvar& In, COLvar* pOut) {
   COL_FUNCTION(CMPCfieldsSetWorker);
   COLstring  Err;
   const bool Success = FILsave(In["file"].asString(), In["config"].json(1), &Err);
   (*pOut)["success"] = Success;
   if(!Success) { (*pOut)["error"] = Err; }
   COL_VAR3(In, Success, Err);
}

static void CMPCfieldsSetWrite(COLvar Result, CFGconfig* pConfig, COLstring Name, SCKloop* pLoop, COLaddress Address) {
   COL_FUNCTION(CMPCfieldsSetWrite);
   if(!Result["success"].asBool()) { return COLrespondError(Address, Result["error"]); }
   const COLstring Value                  = Result["value"];
   CFGconfig       SaveConfig             = *pConfig;  // dont want to modify actual pointer unless save is successful
   SaveConfig.CoreConfig.Fields[Name]     = Value;
   COLauto<COLclosure1<COLvar>> pCallback = COLnewClosure1(&CMPCfieldsSetFinish, pConfig, Name, Value, Address);
   COL_TRC("Writing config to disk");
   COLvar Data;
   Data["user"]           = Result["user"];
   Data["file"]           = DIRcomponentConfig(pConfig->CoreConfig.Guid);
   Data["name"]           = Name;
   Data["component"]      = pConfig->CoreConfig.Guid;
   Data["component_name"] = pConfig->CoreConfig.Name;
   Data["config"]         = CFGconfigSerialize(SaveConfig);
   COL_VAR(Data);
   SCKinvokeWorker(pLoop, CMPCfieldsSetWorker, Data, pCallback);
}

static void CMPCfieldsSetPrepare(const COLvar& In, COLvar* pOut) {
   COL_FUNCTION(CMPCfieldsSetPrepare);
   const COLstring Component = In["component"];
   const COLstring Name      = In["name"];
   const COLstring Username  = In["user"];
   const COLstring Value     = In["value"];
   COLstring       Expanded  = COLexpandEnvironmentVariables(Value);
   CFGconfig       Config;
   if(!CFGconfigLoad(Component, &Config)) { return CMPCfieldsSetError("Could not load component config", pOut); }
   const bool          IsDev       = CFGdevelopmentMode(Config.CoreConfig.Commit, false);
   const COLstring     GrammarPath = DIRcomponentProjectDir(Component, IsDev) + "config.json";
   COLarray<CFGfield> Grammar;
   COLstring           Err;
   COL_VAR(GrammarPath);
   if(!CFGfieldLoadGrammar(GrammarPath, &Grammar, &Err)) { return CMPCfieldsSetError(Err, pOut); }
   if(!CFGfieldIsValid(Grammar, Name, Expanded, &Err)) { return CMPCfieldsSetError(Err, pOut); }
   (*pOut)["success"] = true;
   (*pOut)["value"]   = Value;
   (*pOut)["user"]    = Username;
}

// /component/fields/set
void CMPCfieldsSet(const COLwebRequest& Request, CFGmap* pMap, SCKloop* pLoop) {
   COL_FUNCTION(CMPCfieldsSet);
   COL_VAR(Request.Data);
   const COLstring Component = COLvarRequiredString(Request.Data, "component");
   CFGconfig*      pConfig   = CFGmapNode(pMap, Component);
   if(!pConfig) { return COLrespondError(Request.Address, "No component exists with name or id " + Component); }
   COLstring Error;
   if(!PRMcheckRoleAccess(Request.FromTranslator, Request.User, pConfig->CoreConfig.Guid, ROLid::EDIT_FIELDS, &Error)) {
      return COLrespondError(Request.Address, PRMsetError(ROLid::EDIT_FIELDS));
   }
   const COLstring Name  = COLvarRequiredString(Request.Data, "name").strip(COLstring::Both);
   const COLstring Value = COLvarRequiredString(Request.Data, "value").strip(COLstring::Both);
   COLvar          Data;
   Data["component"] = pConfig->CoreConfig.Guid;
   Data["name"]      = Name;
   Data["value"]     = Value;
   Data["user"]      = Request.User;
   SCKinvokeWorker(pLoop, CMPCfieldsSetPrepare, Data,
                   COLnewClosure1(CMPCfieldsSetWrite, pConfig, Name, pLoop, Request.Address));
}
