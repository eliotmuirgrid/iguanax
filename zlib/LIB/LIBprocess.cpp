//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: LIBprocess.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   10/06/25 1:07 PM
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <DIR/DIRpath.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <GIT/GITpath.h>
#include <GIT/GITstatusChanges.h>
#include <GITU/GITUaheadBehindLibrary.h>
#include <GITU/GITUcollection.h>
#include <GITU/GITUcollectionLoad.h>
#include <GITU/GITUlistLoad.h>
#include <GITU/GITUmodules.h>
#include <GSV/GSVlink.h>
#include <LIB/LIBprocess.h>
#include <GIT/GITfetch.h>
#include <LIB/LIBdata.h>
COL_LOG_MODULE;

static bool LIBcompareComponents(const LIBusage& a, const LIBusage& b) {
   return a.m_Component + a.m_Path < b.m_Component + b.m_Path;
}

static int LIBpartition(COLarray<LIBusage>* pUsages, int low, int high) {
   const LIBusage pivot = (*pUsages)[high];
   int i = low - 1;
   for(int j = low; j < high; j++) {
      if(LIBcompareComponents((*pUsages)[j], pivot)) {
         i++;
         const LIBusage temp = (*pUsages)[i];
         (*pUsages)[i] = (*pUsages)[j];
         (*pUsages)[j] = temp;
      }
   }
   const LIBusage temp = (*pUsages)[i+1];
   (*pUsages)[i+1] = (*pUsages)[high];
   (*pUsages)[high] = temp;
   return i + 1;
}

static void LIBquickSort(COLarray<LIBusage>* pUsages, int low, int high) {
   if(low < high) {
      const int pi = LIBpartition(pUsages, low, high);
      LIBquickSort(pUsages, low, pi-1);
      LIBquickSort(pUsages, pi+1, high);
   }
}

void LIBsortUsage(COLarray<LIBusage>* pUsages) {
   COL_FUNCTION(LIBsortUsage);
   if(pUsages->size() > 1) { LIBquickSort(pUsages, 0, pUsages->size() - 1); }
}

// null key is always a match
static bool LIBgitUrlMatchesKeyUrl(const COLstring& GitUrl, const COLmap<COLstring, GSVlink>& KeyMap) {
   COL_FUNCTION(LIBgitUrlMatchesKeyUrl);
   COL_VAR2(GitUrl, KeyMap.size());
   return KeyMap.empty() || KeyMap.count(GitUrl);
}

static void LIBaddCollectionVar(const GITUrepoInfo& Info, const GSVlink& Link, COLvar* pOut) {
   COL_FUNCTION(LIBcreateCollectionVar);
   GSVlink   SourceLink;
   COLstring SourceUrl    = LIBconstructUrlKey(Info.SourceGitUrl, &SourceLink);
   (*pOut)["collection"]  = GSVlinkToVar(SourceLink);
   (*pOut)["description"] = Info.Description;
   (*pOut)["name"]        = Info.Name;
   (*pOut)["urls"]        = GSVlinkToVar(Link);
}

void LIBprocessCatalogue(const COLmap<COLstring, GSVlink>& KeyMap, COLvar* pOut) {
   COL_FUNCTION(LIBprocessCatalogue);
   GITUlist Catalog;
   if(!FILfileExists(GITcacheDir()) || !GITUlistCatalogueLoad(&Catalog)) {
      COL_TRC("Unable to load catalogue");
      return;
   }
   GITUcollection  Collection;
   GITUcollectionLoadFromCatalogue(Catalog, &Collection);
   for(const auto& info : Collection.m_List.List) {
      GSVlink         ItemLink;
      const COLstring GitUrl = LIBconstructUrlKey(info.GitUrl, &ItemLink);
      if(GITUrepoTypeString(info.RepoType) == "library" && LIBgitUrlMatchesKeyUrl(GitUrl, KeyMap)) {
         LIBaddCollectionVar(info, ItemLink, &(*pOut)[GitUrl]);
      }
   }
   COL_VAR(pOut->json(1));
}

static void LIBpopulateAheadBehindInfo(const COLstring& User, const COLstring& DevDir, const COLstring& Path,
                                       COLvar* pItem) {
   COL_FUNCTION(LIBpopulateAheadBehindInfo);
   const COLstring FullPath = FILpathAppend(DevDir, Path);
   COLstring       Output, Error;
   if(!GITfetch(User, FullPath, "", &Output, &Error)) {
      COL_TRC("Fetch failed for " << Path << " \n" << Output + "\n" << Error);
   }
   Output.clear();
   Error.clear();
   if(GITstatusChanges(FullPath, &Output, &Error)) { (*pItem)["has_changes"] = !Output.is_null(); }
   bool Ahead, Behind;
   if(GITUaheadBehindLibrary(FullPath, &Ahead, &Behind, &Error)) {
      (*pItem)["ahead"]  = Ahead;
      (*pItem)["behind"] = Behind;
   } else {
      (*pItem)["error"] = Error;
   }
}

static void LIBaddModuleVar(const COLstring& User, const COLstring& DevDir, const COLstring& Guid,
                            const COLstring& GitUrl, const COLstring& Path, const GSVlink& Link, COLvar* pOut) {
   COL_FUNCTION(LIBcreateModuleVar);
   COLvar Item;
   Item["urls"]      = GSVlinkToVar(Link);
   Item["component"] = Guid;
   Item["path"]      = Path;
   Item["key"]       = GitUrl;  // used in callback map
   LIBpopulateAheadBehindInfo(User, DevDir, Path, &Item);
   pOut->push_back(Item);
}

void LIBprocessComponentModules(const COLstring& User, const COLstring& Guid, const COLmap<COLstring, GSVlink>& KeyMap,
                                COLvar* pOut) {
   COL_FUNCTION(LIBprocessComponentModules);
   const COLstring DevDir      = DIRcomponentDevDir(Guid);
   const COLstring ModulesFile = FILpathAppend(DevDir, ".gitmodules");
   if(!FILfileExists(ModulesFile)) {
      COL_TRC(ModulesFile << " not found");
      return;
   }
   COLmap<COLstring, GITUmodule> Modules;
   GITUmoduleLoad(ModulesFile, &Modules);
   for(const auto& module : Modules) {
      GSVlink         ModuleLink;
      const COLstring GitUrl = LIBconstructUrlKey(module.second.GitUrl, &ModuleLink);
      if(LIBgitUrlMatchesKeyUrl(GitUrl, KeyMap)) {
         LIBaddModuleVar(User, DevDir, Guid, GitUrl, module.first, ModuleLink, pOut);
      }
   }
   COL_VAR(pOut->json(1));
}

COLstring LIBconstructUrlKey(const COLstring& GitUrl, GSVlink* pLink) {
   COL_FUNCTION(LIBconstructUrlKey);
   if(GSVconstructLink(GitUrl, pLink)) { return pLink->m_Ssh; }
   COL_TRC("Could not construct link for " << GitUrl);
   return GitUrl;
}
