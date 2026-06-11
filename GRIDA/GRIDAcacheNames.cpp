//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GRIDAcacheNames.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <FIL/FILutils.h>
#include <GRID/GRIDnode.h>
#include <GRIDA/GRIDAcacheNames.h>
#include <FIL/FILatomic.h>
COL_LOG_MODULE;

static void GRIDAnamesError(COLvar& Result, const COLstring& Error) {
   Result.reset();
   Result["success"] = false;
   Result["error"]   = Error;
}

static void GRIDAcallbackMain(const COLvar Result, COLauto<COLclosure1<COLvar>> pCallback) {
   COL_FUNCTION(GRIDAcallbackMain);
   try                      { pCallback->run(Result); }
   catch(const COLerror& E) { COL_ERR("GRIDAcallbackMain encountered an error: " << E.description()); }
}

static void GRIDAnamesCallback(const COLvar Result, const COLaddress Address) {
   COL_FUNCTION(GRIDAnamesCallback);
   COL_VAR(Result.json(1));
   Result["success"].asBool() ? COLrespondSuccess(Address, Result["names"]) : COLrespondError(Address, Result["error"].asString());
}

static void GRIDupdateNamesCache(GRIDnode* pNode, const COLstring& File, COLvar& Result) {
   COL_FUNCTION(GRIDupdateNamesCache);
   COLvar UpdatedNames;
   UpdatedNames.setMapType();

   bool NeedsUpdate = false;
   for(const auto& it : pNode->getPeers()) {
      const auto&      Id      = it.first;
      const COLstring& MemName = it.second.InstanceName;
      COLstring        DiskName;
      if(Result.exists(Id)) { DiskName = Result[Id].asString(); }
      if(!MemName.is_null() && (DiskName.is_null() || MemName != DiskName)) { NeedsUpdate = true; }
      UpdatedNames[Id] = !MemName.is_null() ? MemName : DiskName;
   }

   Result.reset();
   Result["success"] = true;
   Result["names"]   = UpdatedNames;

   if(NeedsUpdate) {
      COL_VAR2(File, UpdatedNames);
      try                      { FILwriteFile(File, UpdatedNames.json(1)); }
      catch(const COLerror& E) { GRIDAnamesError(Result, E.description()); }
   }
}

static void GRIDAnames(GRIDnode* pNode, SCKloop* pLoop, COLauto<COLclosure1<COLvar>> pCallback) {
   COL_FUNCTION(GRIDAnames);
   const COLstring File    = DIRiguanaNameCache();
   COLvar Result;
   Result.setMapType();
   try {
      if(FILfileExists(File) ) { FILserializeLoadConfig(&Result, File); }
      GRIDupdateNamesCache(pNode, File, Result);
   }catch(const COLerror& E) { GRIDAnamesError(Result, E.description()); }
   pLoop->post(COLnewClosure0(&GRIDAcallbackMain, Result, pCallback));
}


// /iguanas/names
void GRIDAcacheNames(const COLwebRequest& Request, GRIDnode* pNode, SCKloop* pLoop) {
   COL_FUNCTION(GRIDAcacheNames);
   if(!pNode) { return COLrespondError(Request.Address, "Grid not initialized."); }
   COLauto<COLclosure1<COLvar>> pCallback = COLnewClosure1(&GRIDAnamesCallback, Request.Address);
   pLoop->threadPool()->scheduleTask(COLnewClosure0(&GRIDAnames, pNode, pLoop, pCallback));
}