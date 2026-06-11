//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: DASHcomponentList
//
// Author: Tyler Brown
//
// Implementation
//---------------------------------------------------------------------------
#include <CFG/CFGconfigSave.h>
#include <CFG/CFGmap.h>
#include <COL/COLlog.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <ICOL/ICOLcomponent.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLserialize.h>
#include <SDB/SDBcapi.h>
#include <DASH/DASHqueuePosition.h>
#include <DASH/DASHlistView.h>
#include <DASH/DASHsort.h>
COL_LOG_MODULE;

static COLvar DASHcopyComponent(const CFGconfig& In, const bool FromTranslator, const COLstring& Username) {
   COL_FUNCTION(DASHcopyComponent);
   COLvar    Data;
   COLstring Error;
   ROLrole   Role;
   PRMcheckRolePermissions(FromTranslator, Username, In.CoreConfig.Guid, &Role, &Error);
   Data["role"]             = ROLroleToVar(Role);
   Data["guid"]             = In.CoreConfig.Guid;
   Data["commit"]           = In.CoreConfig.Commit;
   Data["description"]      = In.CoreConfig.Description;
   Data["name"]             = In.CoreConfig.Name;
   Data["template"]         = In.CoreConfig.Template;
   Data["tags"]             = CFGconfigSerializeArray(In.CoreConfig.Tags);
   Data["host_server_guid"] = In.m_HostServerGuid;
   Data["error_count"]      = In.Status.ErrorCount;
   Data["tooltip"]          = In.Status.HtmlTooltipBlock;
   Data["light"]            = CFGoutputLight(In.Status.Light);
   Data["queue_count"]      = SDBIgetQueueCount(In.CoreConfig.Guid);
   Data["mps_in"]           = SDBIgetComponentStats(In.CoreConfig.Guid);
   Data["mps_out"]          = SDBIgetComponentStats(In.CoreConfig.Guid, false);
   Data["last_activity"]    = COLuint64ToString(int(In.Status.LastActivity));
   ICOLcomponentGet(In.CoreConfig.Guid, &Data["columns"]);
   return Data;
}

static void DASHprocessComponents(CFGmap* pMap, const bool FromTranslator, const COLstring& Username,
                                  const COLarray<COLstring>& Components, COLvar& Result) {
   COL_FUNCTION(DASHlistDataImp);
   Result["component_data"].setArrayType();
   for(const auto& Component : Components) {
      Result["component_data"].push_back(DASHcopyComponent(pMap->ConfigMap[Component], FromTranslator, Username));
   }
   Result["total_components"].set((int)pMap->ConfigMap.size());
}

static void DASHlistDataImp(const COLstring& GlobTerms, const bool ExactMatch, const COLstring& Order,
                            const COLvar& Selected, const bool OnlyChecked, const bool AscendingOrder, CFGmap* pMap,
                            const COLstring& Username, const bool FromTranslator, COLvar& Result) {
   COL_FUNCTION(DASHlistDataImp);
   COLarray<COLstring> Components;
   if(!OnlyChecked) { DASHglobMatch(GlobTerms, pMap, ExactMatch, &Components); }
   DASHcombineLists(DASHvarToArray(Selected), pMap, &Components);
   DASHsort(Order, AscendingOrder, pMap, &Components);
   DASHprocessComponents(pMap, FromTranslator, Username, Components, Result);
}

// /component/list
void DASHlistData(const COLwebRequest& Request, CFGmap* pMap) {
   COL_FUNCTION(DASHdashboardListData);
   COL_VAR(Request.Data.json(1));
   COLvar          Result;
   const COLstring GlobTerms      = COLvarGetString(Request.Data, "filter", "");
   const COLstring Order          = COLvarGetString(Request.Data, "order", "NAME");
   const COLvar    Selected       = COLvarGetObject(Request.Data, "checked", COLvar());
   const bool      ExactMatch     = COLvarGetBool(Request.Data, "whole_word", false);
   const bool      OnlyChecked    = COLvarGetBool(Request.Data, "checked_only", false);
   const bool      AscendingOrder = COLvarGetBool(Request.Data, "ascending", true);
   DASHlistDataImp(GlobTerms, ExactMatch, Order, Selected, OnlyChecked, AscendingOrder, pMap, Request.User,
                   Request.FromTranslator, Result);
   COL_VAR(Result.json(1));
   COLrespondSuccess(Request.Address, Result);
}
