//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: LIBinfo.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   09/06/25 2:34 PM
//  ---------------------------------------------------------------------------
#include <CFG/CFGmap.h>
#include <COL/COLclosure6.h>
#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <COL/COLvarUtils.h>
#include <GSV/GSVlink.h>
#include <LIB/LIBdata.h>
#include <LIB/LIBinfo.h>
#include <LIB/LIBprocess.h>
#include <SCK/SCKloop.h>

#include <memory>
COL_LOG_MODULE;

struct LIBinfoTracker {
   explicit LIBinfoTracker(const int Pending) : m_Pending(Pending) {}
   COLmap<COLstring, LIBdata> m_Data;
   COLmap<COLstring, GSVlink> m_KeyMap;
   int m_Pending;
};

static void LIBinfoSetKeyMap(const COLvar& KeyVar, COLmap<COLstring, GSVlink>* pMap) {
   COL_FUNCTION(LIBinfoSetKeyMap);
   COL_VAR(KeyVar.json(1));
   if(!KeyVar.isArray()) { return; }
   for(const auto& it : KeyVar.array()) {
      GSVlink         KeyLink;
      const COLstring KeyUrl = LIBconstructUrlKey(it, &KeyLink);
      pMap->add(KeyUrl, KeyLink);
   }
}

static void LIBinfoCompleteCallback() {
   COL_FUNCTION(LIBinfoCompleteCallback);
   COL_TRC("LIBinfo complete");
}

static void LIBinfoCallbackMain(const LIBtype Type, const COLvar Result,
                                COLauto<COLclosure2<LIBtype, COLvar>> pCallback) {
   try {
      pCallback->run(Type, Result);
   } catch(const COLerror& Error) {
      COL_ERR("ERROR - MAJOR PROBLEM - LIBinfoCallbackMain HANDLER THREW EXCEPTION" << newline << Error);
   }
}

static COLvar LIBcreateResponse(std::shared_ptr<LIBinfoTracker> pTracker) {
   COL_FUNCTION(LIBcreateResponse);
   COLvar Response;
   Response.setMapType();
   for(const auto& it : pTracker->m_Data) {
      if(it.second.m_Usage.size() > 0) {
         LIBsortUsage(&it.second.m_Usage);
         LIBdataToVar(it.second, &Response[it.first]);
      }
   }
   COL_VAR(Response.json(1));
   return Response;
}

static void LIBaddCollectionData(COLmap<COLstring, LIBdata>* pData, const COLvar& Data) {
   COL_FUNCTION(LIBaddCollectionData);
   COL_VAR(Data.json(1));
   if(!Data.isMap()) {
      COL_TRC("Collection data load failed");
      return;
   }
   for(const auto& it : Data.map()) {
      LIBsetLibDataInfo(&(*pData)[it.first], it.second);
   }
}

// components could have multiple libs with same upstream which is why we use an array instead of map
static void LIBaddComponentData(COLmap<COLstring, LIBdata>* pData, const COLvar& Data) {
   COL_FUNCTION(LIBaddComponentData);
   COL_VAR(Data.json(1));
   if(!Data.isArray()) {
      COL_TRC("Component data load failed");
      return;
   }
   for(const auto& it : Data.array()) { LIBsetLibDataUsage(&(*pData)[it["key"].asString()], it); }
}

static void LIBcallback(const LIBtype Type, const COLvar Result, std::shared_ptr<LIBinfoTracker> pTracker, SCKloop* pLoop,
                        const COLaddress Address) {
   COL_FUNCTION(LIBcallback);
   pTracker->m_Pending--;
   Type == LIB_COMPONENT_DATA ? LIBaddComponentData(&pTracker->m_Data, Result)
                              : LIBaddCollectionData(&pTracker->m_Data, Result);
   if(pTracker->m_Pending == 0) {
      COLrespondSuccess(Address, LIBcreateResponse(pTracker));
      pLoop->shutdown();
   }
}

static void LIBloadCatalogue(COLmap<COLstring, GSVlink>* pKeyMap, SCKloop* pLoop,
                             COLauto<COLclosure2<LIBtype, COLvar>> pCallback) {
   COL_FUNCTION(LIBloadCatalogue);
   COLvar Result;
   Result.setMapType();
   LIBprocessCatalogue(*pKeyMap, &Result);
   pLoop->post(COLnewClosure0(&LIBinfoCallbackMain, LIB_COLLECTION_DATA, Result, pCallback));
}

static void LIBcomponentWorker(const COLstring User, const COLstring Guid, COLmap<COLstring, GSVlink>* pKeyMap,
                               SCKloop* pLoop, COLauto<COLclosure2<LIBtype, COLvar>> pCallback) {
   COL_FUNCTION(LIBcomponentWorker);
   COLvar Result;
   Result.setArrayType();
   LIBprocessComponentModules(User, Guid, *pKeyMap, &Result);
   pLoop->post(COLnewClosure0(&LIBinfoCallbackMain, LIB_COMPONENT_DATA, Result, pCallback));
}

static void LIBinfoImpl(CFGmap* pMap, const COLvar Keys, const COLstring User, const COLaddress Address,
                        SCKloop* pMainLoop, COLclosure0* pMainCallback) {
   COLthreadPool Pool(1, 20);
   SCKloop Loop(&Pool);
   Pool.start();
   auto pTracker = std::make_shared<LIBinfoTracker>(pMap->ConfigMap.size() + 1);
   LIBinfoSetKeyMap(Keys, &pTracker->m_KeyMap);
   for(const auto& it : pMap->ConfigMap) {
      COLauto<COLclosure2<LIBtype, COLvar>> pCallback = COLnewClosure2(&LIBcallback, pTracker, &Loop, Address);
      Loop.threadPool()->scheduleTask(COLnewClosure0(&LIBcomponentWorker, User, it.first, &pTracker->m_KeyMap, &Loop, pCallback));
   }
   COLauto<COLclosure2<LIBtype, COLvar>> pCallback = COLnewClosure2(&LIBcallback, pTracker, &Loop, Address);
   Loop.threadPool()->scheduleTask(COLnewClosure0(&LIBloadCatalogue, &pTracker->m_KeyMap, &Loop, pCallback));
   Loop.start();
   pMainLoop->post(pMainCallback);
}

// /llibrary/info
void LIBinfo(const COLwebRequest& Request, CFGmap* pMap, SCKloop* pLoop) {
   COL_FUNCTION(LIBinfo);
   COLvar Keys;
   Keys.setArrayType();
   if(Request.Data.exists("key")) { Keys = COLvarRequiredStringOrStringList(Request.Data, "key"); }
   COLclosure0*     pCallback = COLnewClosure0(&LIBinfoCompleteCallback);
   pLoop->threadPool()->scheduleTask(
       COLnewClosure0(&LIBinfoImpl, pMap, Keys, Request.User, Request.Address, pLoop, pCallback));
}
