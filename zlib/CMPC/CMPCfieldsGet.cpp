//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: CMPCfieldsGet.cpp
//
//  Description:
//
//  Implementation
//
//  Author: Matthew Sobkowski
//  Date:   4/19/23 10:40 AM
//  ---------------------------------------------------------------------------
#include <BLUA/BLUAmanager.h>
#include <CFG/CFGconfig.h>
#include <CFG/CFGconfigApi.h>
#include <CFG/CFGconfigLoad.h>
#include <CFG/CFGfieldGrammar.h>
#include <CFG/CFGfieldSanitize.h>
#include <CFG/CFGmapApi.h>
#include <CMPC/CMPCfieldsGet.h>
#include <CMPC/CMPCutils.h>
#include <COL/COLclosure6.h>
#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <COL/COLutils.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
#include <SCK/SCKloop.h>
COL_LOG_MODULE;

#define CMPC_MASKED_PASSWORD "******"

static void CMPCfieldsSetPair(const COLstring& TrueValue, bool UseCachedEnvironment, const COLvar& CachedEnvironment,
                              COLvar* pFieldVar) {
   COL_FUNCTION(CMPCfieldsSetPair);
   COL_VAR(TrueValue);
   (*pFieldVar)["true_value"]    = TrueValue;
   (*pFieldVar)["display_value"] = UseCachedEnvironment
                                       ? COLexpandEnvironmentVariablesUsingCache(TrueValue, CachedEnvironment)
                                       : CMPCexpandEnvironmentVariables(TrueValue);
}

static void CMPCfieldsMapValue(const CFGfield& Field, const COLmap<COLstring, COLstring>& Values,
                               bool UseCachedEnvironment, const COLvar& CachedEnvironment, COLvar* pFieldVar) {
   COL_FUNCTION(CMPCfieldsMapValue);
   const bool      IsPassword = Field.FieldType == CFGfieldType::PASSWORD;
   const COLstring Default    = IsPassword && !Field.Default.is_null() ? CMPC_MASKED_PASSWORD : Field.Default;
   if(!Values.count(Field.Name)) {
      COL_TRC("Value not set by user -- using default");
      return CMPCfieldsSetPair(Default, UseCachedEnvironment, CachedEnvironment, pFieldVar);
   }
   COLstring TrueValue = Values[Field.Name];
   COLstring Expanded  = UseCachedEnvironment ? COLexpandEnvironmentVariablesUsingCache(TrueValue, CachedEnvironment)
                                              : CMPCexpandEnvironmentVariables(TrueValue);
   COLstring Error;
   if(!CFGfieldsSanitizeValue(Field.Name, Field.FieldType, Expanded, &Error)) {
      COL_TRC("Sanitization failed -- " << Field.Name << ":" << Expanded << " Error=" << Error);
      return CMPCfieldsSetPair(Default, UseCachedEnvironment, CachedEnvironment, pFieldVar);
   }
   TrueValue = IsPassword && !TrueValue.is_null()
                   ? CMPC_MASKED_PASSWORD
                   : TrueValue;  // dont want to show password value in CARD -- but if empty it is ok
   CMPCfieldsSetPair(TrueValue, UseCachedEnvironment, CachedEnvironment, pFieldVar);
}

void CMPCfieldsGetFieldsData(const COLarray<CFGfield>& Grammar, bool UseCachedEnvironment,
                             const COLvar& CachedEnvironment, const COLmap<COLstring, COLstring>& FieldValues,
                             COLvar* pResult) {
   COL_FUNCTION(CMPCfieldsGetFieldsData);
   COLvar& Result = *pResult;
   for(const auto& Field : Grammar) {
      COL_VAR3(Field.Name, (int)Field.FieldType, Field.Default);
      COLvar FieldVar;
      FieldVar["name"]    = Field.Name;
      FieldVar["default"] = Field.FieldType == CFGfieldType::PASSWORD ? CMPC_MASKED_PASSWORD : Field.Default;
      FieldVar["type"]    = CFGfieldTypeToString(Field.FieldType);
      FieldVar["options"] = CFGfieldsParseOptions(Field.Options);
      if(!Field.Description.is_null()) { FieldVar["description"] = Field.Description; }
      CMPCfieldsMapValue(Field, FieldValues, UseCachedEnvironment, CachedEnvironment, &FieldVar);
      COL_VAR(FieldVar.json(1));
      Result.push_back(FieldVar);
   }
}

static void CMPCfieldsGetImpl(CFGconfig Config, bool UseCachedEnvironment, COLvar CachedEnvironment, COLaddress Address,
                              SCKloop* pLoop, COLclosure0* pCallback) {
   COL_FUNCTION(CMPCfieldsGetImpl);
   const bool         IsDev       = CFGdevelopmentMode(Config.CoreConfig.Commit, false);
   const COLstring    GrammarPath = DIRcomponentProjectDir(Config.CoreConfig.Guid, IsDev) + "config.json";
   COLstring          Error;
   COLarray<CFGfield> Grammar;
   if(!CFGfieldLoadGrammar(GrammarPath, &Grammar, &Error)) {
      COLrespondError(Address, Error);
      return pLoop->post(pCallback);
   }
   COL_VAR3(IsDev, GrammarPath, Grammar.size());
   COLvar Result;
   CMPCfieldsGetFieldsData(Grammar, UseCachedEnvironment, CachedEnvironment, Config.CoreConfig.Fields, &Result);
   COL_VAR(Result.json(1));
   COLrespondSuccess(Address, Result);
   pLoop->post(pCallback);
}

// /component/fields/get
void CMPCfieldsGet(const COLwebRequest& Request, CFGmap* pMap, BLUAinstanceManager* pManager, SCKloop* pLoop) {
   COL_FUNCTION(CMPCfieldsGet);
   COLstring       Error;
   const COLstring Component = COLvarRequiredString(Request.Data, "component");
   CFGconfig*      pConfig   = CFGmapNode(pMap, Component);
   if(!pConfig) { return COLrespondError(Request.Address, "No component exists with name or id " + Component); }
   if(!PRMcheckRoleAccess(Request.FromTranslator, Request.User, pConfig->CoreConfig.Guid, ROLid::VIEW_SCRIPT, &Error)) {
      return COLrespondError(Request.Address, PRMsetError(ROLid::VIEW_SCRIPT));
   }
   COLvar CachedEnvironment;
   bool   UseCachedEnvironment = BLUAinstanceRunning(*pManager, pConfig->CoreConfig.Guid);
   if(UseCachedEnvironment) {
      COL_TRC("Component is running, use environment variables from cache in pInstance");
      CachedEnvironment = pManager->m_Instances[pConfig->CoreConfig.Guid]->m_Fields;
   }
   pLoop->threadPool()->scheduleTask(COLnewClosure0(&CMPCfieldsGetImpl, *pConfig, UseCachedEnvironment,
                                                    CachedEnvironment, Request.Address, pLoop,
                                                    CMPCcallback("/component/fields/get")));
}
