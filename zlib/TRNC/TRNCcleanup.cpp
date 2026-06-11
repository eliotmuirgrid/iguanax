// ---------------------------------------------------------------------------
//  Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: TRNCcleanup.cpp
//
//  Description
//
//  Author: Matthew Sobkowski
//  Date:   9/1/22 12:30 PM
// ---------------------------------------------------------------------------
#include <CFG/CFGmap.h>
#include <CFG/CFGmapApi.h>
#include <COL/COLlog.h>
#include <DIR/DIRpath.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <SDB/SDBcapi.h>
#include <TRNC/TRNCcleanup.h>
COL_LOG_MODULE;

static COLstring TRNCarchiveCompsDir() {
   COL_FUNCTION(TRNCarchiveCompsDir);
   return FILpathAppend(DIRarchiveDir(), "comps/");
}

static void TRNCarchiveComponentConfig(const COLstring& ComponentId) {
   COL_FUNCTION(TRNCarchiveComponentConfig);
   const COLstring ArchivedDirPath = TRNCarchiveCompsDir();
   FILmakeFullDir(ArchivedDirPath, (FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX));
   const COLstring ComponentPath = DIRcomponentDir(ComponentId);
   const COLstring ArchivedComp  = FILpathAppend(ArchivedDirPath, ComponentId + FIL_PATH_SEP_NATIVE);
   FILcopyDirectoryContents(ComponentPath, ArchivedComp, FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX);
   COL_TRC("Moving " << ComponentId << " to archived folder");
   COLstring Error;
   FILremoveFullDirNewSafe(ComponentPath, &Error);
}

void TRNCarchiveComponent(const COLstring& ComponentId) {
   COL_FUNCTION(TRNCarchiveComponent);
   COL_VAR(ComponentId);
   TRNCarchiveComponentConfig(ComponentId);
}

void TRNCarchive(const COLarray<COLstring>& InvalidComponents, const COLstring& Reason) {
   COL_FUNCTION(TRNCarchive);
   COL_VAR(Reason);
   const COLstring ConfigDirPath   = DIRcompsDir();
   const COLstring ArchivedDirPath = TRNCarchiveCompsDir();
   FILmakeFullDir(ArchivedDirPath, (FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX));
   COLstring Message = "Failed to load components from " + ConfigDirPath;
   Message.append("\nReason: " + Reason);
   Message.append("\nThey have been moved to: " + ArchivedDirPath);
   for(const auto& InvalidComponent : InvalidComponents) {
      TRNCarchiveComponent(InvalidComponent);
      Message.append(InvalidComponent + "\n");
   }
   SDBIlogService(Message, "#error");
}

void TRNClog(const COLstring& ComponentId, const COLstring& Reason) {
   COL_FUNCTION(TRNClog);
   COL_VAR2(ComponentId, Reason);
   COLstring Message = "Component " + ComponentId + " failed to load";
   Message.append("\nReason: " + Reason);
   SDBIlogService(Message, "#error");
}

void TRNCremoveComponentsFromMap(const COLarray<COLstring>& InvalidComponents, CFGmap* pMap) {
   COL_FUNCTION(TRNCremoveComponentsFromMap);
   for(const auto& Component : InvalidComponents) {
      if(CFGmapremoveComponent(pMap, Component)) { COL_TRC(Component + " removed from component map"); }
      else { COL_TRC("Removal failed -- " + Component); }
   }
}
