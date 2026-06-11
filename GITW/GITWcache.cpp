// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITWcache
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Tuesday 28 November 2023 - 10:03AM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <FIL/FILutils.h>
#include <GITU/GITUcache.h>
#include <GITU/GITUcollection.h>
#include <GITU/GITUcollectionLoad.h>
#include <GITU/GITUlist.h>
#include <GITU/GITUlistLoad.h>
#include <GIT/GITpath.h>
#include <GITW/GITWcache.h>
#include <GSV/GSVlink.h>
COL_LOG_MODULE;

bool GITWrepositoryCached(const COLstring& GitUrl, const COLstring& Branch){
   COL_FUNCTION(GITWrepositoryCached);
   COLstring Dir = GITgitTemplateDir(GitUrl, Branch);
   COL_VAR(Dir);
   bool Cached = FILfileExists(Dir + ".git");
   return Cached;
}

// need to add in the cache flag checking logic
bool GITWrepositoryCachedFull(const COLstring& GitUrl, const COLstring& Branch, COLstring* pErrorLibUrl) {
   COL_FUNCTION(GITWrepositoryCachedFull);
   COLstring& ErrorLibUrl = *pErrorLibUrl;
   COL_VAR(GitUrl);
   if(!GITWrepositoryCached(GitUrl, Branch)) {
      COL_TRC("No cached repository for: " << GitUrl);
      return false;
   }
   GITUlist  List;
   COLstring Dir = GITgitTemplateDir(GitUrl, Branch);
   GITUlistLoad(Dir, GitUrl, &List);
   for(auto& i : List.List) {
      COLstring LibGitUrl = i.GitUrl;
      COLstring LibBranch = i.Branch;
      if(!GITWrepositoryCached(LibGitUrl, LibBranch)) {
         ErrorLibUrl = LibGitUrl;
         COL_TRC("No cached repository for: " << LibGitUrl);
         return false;
      }
   }
   return true;
}

void GITWcheckCached(COLvar* pOut) {
   COL_FUNCTION(GITWcheckCached);
   COLvar& Out = *pOut;
   COL_VAR(Out.json(1));
   int j = 0;
   for(int i = 0; i < Out.size(); i++) {
      COLstring  LibUrl;
      const bool Cached = GITWrepositoryCachedFull(Out[j]["git"], Out[j]["branch"], &LibUrl);
      Out[j]["cached"]  = Cached;
      if(!Cached && !LibUrl.is_null()) {
         GSVlink Link;
         GSVconstructLink(LibUrl, &Link);
         Out[j]["lib_url"]  = LibUrl;
         Out[j]["lib_urls"] = GSVlinkToVar(Link);
      }
      j++;
   }
   COL_VAR(Out.json(1));
}

void GITWextractType(const COLvar& Data, const COLstring& Type, COLvar* pOut) {
   COL_FUNCTION(GITWextractType);
   COLvar& Array = *pOut;
   Array.setArrayType();
   for(auto i : Data.map()) { if(i->second["type"].asString() == Type) Array.push_back(i->second); }
}

// /git/cache
void GITWcache(const COLwebRequest& Request) {
   COL_FUNCTION(GITWcache);
   const COLstring Type = COLvarGetString(Request.Data, "type", "component");
   if(Type != "component" && Type != "library") {
      return COLrespondError(Request.Address, "Unexpected type requested: " + Type + ". Please use 'library' or 'component'");
   }
   COLstring Log;
   if(!FILfileExists(GITcacheDir())) { GITUcacheInit(Request.User, &Log); }
   GITUlist Catalog;
   if(!GITUlistCatalogueLoad(&Catalog)) { return COLrespondError(Request.Address, "Failed to load catalogue"); }
   GITUcollection Collection;
   GITUcollectionLoadFromCatalogue(Catalog, &Collection);
   const COLvar Data = GITUcollectionItems(Collection, true);
   COLvar Result;
   GITWextractType(Data, Type, &Result);
   GITWcheckCached(&Result);
   COLrespondSuccess(Request.Address, Result);
}