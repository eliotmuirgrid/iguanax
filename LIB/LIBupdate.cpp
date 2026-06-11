//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: LIBupdate.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   11/06/25 10:05 AM
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <COL/COLvarUtils.h>
#include <GIT/GITpull.h>
#include <LIB/LIBdata.h>
#include <LIB/LIBupdate.h>
#include <SCK/SCKloop.h>
#include <COL/COLclosure6.h>
#include <DIR/DIRpath.h>
#include <FIL/FILpathUtils.h>
#include <PRM/PRMcallbacks.h>
#include <memory>
COL_LOG_MODULE;

struct LIBupdateData {
   LIBupdateData(const COLstring& Component, const COLstring& Path) : m_Component(Component), m_Path(Path) {}
   COLstring m_Component;
   COLstring m_Path;
   COLstring m_Output;
   COLstring m_Error;
   bool      m_Success = false;
};

struct LIBupdateTracker {
   explicit LIBupdateTracker(const int Pending) : m_Pending(Pending) {}
   COLarray<LIBupdateData> m_Results;
   int m_Pending;
};

static void LIBupdateCallbackMain(const LIBupdateData Data, COLauto<COLclosure1<LIBupdateData>> pCallback) {
   try {
      pCallback->run(Data);
   } catch(const COLerror& Error) {
      COL_ERR("ERROR - MAJOR PROBLEM - LIBupdateCallbackMain HANDLER THREW EXCEPTION" << newline << Error);
   }
}

static COLvar LIBupdateCreateResponse(const COLarray<LIBupdateData>& Results) {
   COL_FUNCTION(LIBupdateCreateResponse);
   COLvar Response;
   Response.setArrayType();
   for(const auto& it : Results) {
      COLvar Item;
      Item["component"] = it.m_Component;
      Item["path"]      = it.m_Path;
      Item["output"]    = it.m_Output;
      Item["error"]     = it.m_Error;
      Item["success"]   = it.m_Success;
      Response.push_back(Item);
   }
   return Response;
}

static void LIBupdateCallback(const LIBupdateData Data, std::shared_ptr<LIBupdateTracker> pTracker,
                              const COLaddress Address) {
   COL_FUNCTION(LIBupdateCallback);
   pTracker->m_Pending--;
   pTracker->m_Results.push_back(Data);
   if(pTracker->m_Pending == 0) {
      COLrespondSuccess(Address, LIBupdateCreateResponse(pTracker->m_Results));
   }
}

static void LIBupdateWorker(const COLstring User, const COLstring Guid, const COLstring Path, SCKloop* pLoop,
                            COLauto<COLclosure1<LIBupdateData>> pCallback) {
   COL_FUNCTION(LIBupdateWorker);
   LIBupdateData   Data(Guid, Path);
   const COLstring DevDir = DIRcomponentDevDir(Guid);
   const COLstring LibDir = FILpathAppend(DevDir, Path);
   Data.m_Success         = GITpull(User, LibDir, &Data.m_Output, &Data.m_Error);
   pLoop->post(COLnewClosure0(&LIBupdateCallbackMain, Data, pCallback));
}

// /llibrary/update
void LIBupdate(const COLwebRequest& Request, SCKloop* pLoop) {
   COL_FUNCTION(LIBupdate);
   COL_VAR(Request.Data);
   COLstring Error;
   if (!PRMcheckRoleAccess(Request.FromTranslator, Request.User, "", EDIT_SCRIPT, &Error)){
      return COLrespondError(Request.Address, PRMsetError(EDIT_SCRIPT));
   }
   const COLvar& Items = COLvarRequiredObject(Request.Data, "items");
   if(!Items.isArray()) { return COLrespondError(Request.Address, "Invalid data object"); }
   auto pTracker = std::make_shared<LIBupdateTracker>(Items.array().size());
   for(const auto& it : Items.array()) {
      COLauto<COLclosure1<LIBupdateData>> pCallback = COLnewClosure1(&LIBupdateCallback, pTracker, Request.Address);
      pLoop->threadPool()->scheduleTask(COLnewClosure0(&LIBupdateWorker, Request.User, it["component"].asString(),
                                                       it["path"].asString(), pLoop, pCallback));
   }
}
