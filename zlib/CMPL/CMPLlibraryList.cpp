//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: CMPLlibraryList2.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   27/03/25 10:27 AM
//  ---------------------------------------------------------------------------
#include <CFG/CFGconfig.h>
#include <CFG/CFGconfigApi.h>
#include <CFG/CFGmapApi.h>
#include <CFG/CFGmap.h>
#include <CMPC/CMPCutils.h>
#include <CMPL/CMPLlibraryList.h>
#include <COL/COLclosure6.h>
#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <GITU/GITUmodules.h>
#include <GITU/GITUurl.h>
#include <GSV/GSVlink.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
#include <SCK/SCKloop.h>
COL_LOG_MODULE;

static void CMPLlibraryListImpl(COLstring Component, const bool Development, const COLaddress Address, SCKloop* pLoop,
                                COLclosure0* pCallback) {
   COL_FUNCTION(CMPLlibraryListImpl);
   COL_VAR(Component);
   const COLstring ComponentDir = CFGresolveRepoFile(Component, "", Development);
   COLvar Data;
   Data.setMapType();
   const COLstring FileName = FILpathAppend(ComponentDir, ".gitmodules");
   COL_VAR(FileName);
   if(!FILfileExists(FileName)) {
      COLrespondSuccess(Address, Data);
      return pLoop->post(pCallback);
   }
   COLmap<COLstring, GITUmodule> ModuleList;
   GITUmoduleLoad(FileName, &ModuleList);
   for(const auto& i : ModuleList) {
      COLvar    Entry;
      COLstring GitUrl = i->second.GitUrl;
      Entry["url"]     = GitUrl;
      Entry["locked"]  = GITUisHttpxUrl(i->second.GitUrl);
      GSVlink Link;
      GSVconstructLink(GitUrl, &Link, i->second.Branch);
      Entry["git"]        = GSVlinkToVar(Link);
      Entry["git"]["set"] = GitUrl;
      Data[i->first]      = Entry;
   }
   COLrespondSuccess(Address, Data);
   pLoop->post(pCallback);
}

// /component/library/list
void CMPLlibraryList(const COLwebRequest& Request, SCKloop* pLoop) {
   COL_FUNCTION(CMPLlibraryList);
   COLstring ComponentId, Error;
   const bool      Dev       = COLvarRequiredBool(Request.Data, "development");
   if (!CFGcomponentId(Request.Data, &ComponentId, &Error)){
      return COLrespondError(Request.Address, Error);
   }
   const ROLid     Id        = Dev ? EDIT_SCRIPT : VIEW_SCRIPT;
   if(!PRMcheckRoleAccess(Request.FromTranslator, Request.User, ComponentId, Id, &Error)) {
      return COLrespondError(Request.Address, PRMsetError(Id));
   }
   pLoop->threadPool()->scheduleTask(COLnewClosure0(&CMPLlibraryListImpl, ComponentId, Dev,
                                                    Request.Address, pLoop, CMPCcallback("/component/library/list")));
}
