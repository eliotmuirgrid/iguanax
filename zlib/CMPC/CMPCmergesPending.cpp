// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMPCmergesPending
//
// Description:
//
// Implementation
//
// Author: Vismay Shah
// Date:   Thursday 22 August 2024 - 12:27PM
// ---------------------------------------------------------------------------
#include <CFG/CFGconfig.h>
#include <CFG/CFGconfigApi.h>
#include <CFG/CFGmapApi.h>
#include <CMPC/CMPCmergesPending.h>
#include <COL/COLclosure6.h>
#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <FIL/FILdirEnumerator.h>
#include <FIL/FILpathUtils.h>
#include <SCK/SCKloop.h>

#include <memory>
COL_LOG_MODULE;

#define CMPC_MERGE_SCAN_SIZE_MAX 1048576

struct CMPCmergesTracker {
   explicit CMPCmergesTracker(const int Pending, const bool Development)
       : m_Pending(Pending), m_Development(Development) {}
   int    m_Pending     = 0;
   bool   m_Development = true;
   COLvar ResultVar;
};

static void CMPCrunMergesCallback(const bool Success, const COLvar Result, COLauto<COLclosure2<bool, COLvar>> pCallback) {
   COL_FUNCTION(CMPCrunMergesCallback);
   try {
      pCallback->run(Success, Result);
   } catch(const COLerror& E) {
      COL_ERR("ERROR - MAJOR PROBLEM - CMPCrunMergesCallback HANDLER THREW EXCEPTION" << newline << E.description());
   }
}

static void CMPCmergesCallback(const bool Success, const COLvar Out, std::shared_ptr<CMPCmergesTracker> pTracker,
                               const COLaddress Address) {
   COL_FUNCTION(CMPCmergesCallback);
   pTracker->m_Pending--;
   COL_VAR(pTracker->m_Pending);
   const COLstring& Component          = Out["component"];
   const COLstring  Key                = Success ? "output" : "error";
   pTracker->ResultVar[Component][Key] = Out["output"];
   if(pTracker->m_Pending == 0) {
      COLrespondSuccess(Address, pTracker->ResultVar);
   }
}

static bool CMPCfileHasMergesPending(const COLstring& FileName) {
   COL_FUNCTION(CMPCfileHasMergesPending);
   COLstring Data;
   FILreadFile(FileName, &Data);
   if(COLstringContains(Data, "<<<<<<< HEAD") && COLstringContains(Data, "=======") &&
      COLstringContains(Data, ">>>>>>>")) {
      COL_TRC("Found conflict in  " + FileName);
      return true;
   }
   return false;
}

static bool CMPCgetFilesWithMergesPending(const COLstring& Root, const COLstring& ComponentDir, COLvar* pOut) {
   COL_FUNCTION(CMPCgetFilesWithMergesPending);
   COLstring        FileName;
   FILdirEnumerator Enum(ComponentDir + FIL_ANYFILE, false, true);
   try {
      while(Enum.getFile(FileName)) {
         COLstring FullPath = FILpathAppend(ComponentDir, FileName);
         if(Enum.isDirectory()) {
            COLstring NewDir = FullPath;
            FILaddPathSeparator(NewDir);
            CMPCgetFilesWithMergesPending(Root, NewDir, pOut);
         } else if(FILfileSize(FullPath) < CMPC_MERGE_SCAN_SIZE_MAX) {
            if(CMPCfileHasMergesPending(FullPath)) {
               FullPath.replace(Root, "");
               (*pOut)[FullPath] = true;
            }
         }
      }
   } catch(const COLerror& Error) {
      COL_VAR(Error.description());
      pOut->reset();
      *pOut = Error.description();
      return false;
   }
   return true;
}

static void CMPCmergesWorker(const COLstring Component, CFGmap* pMap, SCKloop* pLoop,
                             std::shared_ptr<CMPCmergesTracker> pTracker,
                             COLauto<COLclosure2<bool, COLvar>> pCallback) {
   COL_FUNCTION(CMPCmergesWorker);
   COLvar Result;
   Result["component"] = Component;
   CFGconfig* pConfig = CFGmapNode(pMap, Component);
   if(!pConfig) {
      Result["output"] = Component + " not found";
      return pLoop->post(COLnewClosure0(&CMPCrunMergesCallback, false, Result, pCallback));
   }
   COL_VAR(pTracker->m_Development);
   COLstring ComponentDir = CFGresolveRepoFile(Component, "", pTracker->m_Development);
   FILaddPathSeparator(ComponentDir);
   COL_VAR(ComponentDir);
   COLvar Out;
   Out.setMapType();
   const bool Success = CMPCgetFilesWithMergesPending(ComponentDir, ComponentDir, &Out);
   if(Success) {
      const COLstring GitModules = FILpathAppend(ComponentDir, ".gitmodules");
      if(FILfileExists(GitModules) && CMPCfileHasMergesPending(GitModules)) { Out[".gitmodules"] = true; }
      Result["output"] = Out;
   }
   else        { Result["output"] = "Failed to scan for merge conflicts. " + Out.asString(); }
   pLoop->post(COLnewClosure0(&CMPCrunMergesCallback, Success, Result, pCallback));
}

// /component/merge_conflicts
void CMPCmergesPending(const COLwebRequest& Request, CFGmap* pMap, SCKloop* pLoop) {
   COL_FUNCTION(CMPCmergesPending);
   COL_VAR(Request.Data.json(1));
   const COLvar& Components  = COLvarRequiredStringOrStringList(Request.Data, "component");
   const bool    Development = COLvarGetBool(Request.Data, "development", false);
   auto pTracker = std::make_shared<CMPCmergesTracker>(Components.size(), Development);
   for(const auto& it : Components.array()) {
      COLauto<COLclosure2<bool, COLvar>> pCallback = COLnewClosure2(&CMPCmergesCallback, pTracker, Request.Address);
      pLoop->threadPool()->scheduleTask(COLnewClosure0(&CMPCmergesWorker, it.asString(), pMap, pLoop, pTracker, pCallback));
   }
}