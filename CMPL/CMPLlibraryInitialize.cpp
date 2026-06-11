//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: CMPLlibraryInitialize.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   27/03/25 10:27 AM
//  ---------------------------------------------------------------------------
#include <CFG/CFGconfig.h>
#include <CFG/CFGmapApi.h>
#include <CMPC/CMPCinitializeLibraries.h>
#include <CMPC/CMPCutils.h>
#include <CMPL/CMPLlibraryInitialize.h>
#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <FIL/FILutils.h>
#include <GITU/GITUmodules.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
#include <SCK/SCKloop.h>
COL_LOG_MODULE;

void CMPLlibraryInitializeImpl(COLvar Data, COLaddress Address, SCKloop* pLoop, COLclosure0* pCallback) {
   COL_FUNCTION(CMPLlibraryInitializeImpl);
   const COLstring Component  = Data["component"];
   const COLstring Username   = Data["username"];
   const COLstring ModuleName = Data["name"];
   // This is only ever run in development mode so no dev check
   COLstring ComponentDir = DIRcomponentProjectDir(Component, true);
   COL_VAR(ComponentDir);
   COLstring                     ErrorMessage;
   COLmap<COLstring, GITUmodule> ModuleList;
   COLstring                     FileName = ComponentDir + ".gitmodules";
   if(!FILfileExists(FileName)) {
      ErrorMessage = "Could not find the file '" + FileName + "'";
      COL_TRC(ErrorMessage);
      COLrespondError(Address, ErrorMessage);
      return pLoop->post(pCallback);
   }
   GITUmoduleLoad(FileName, &ModuleList);
   if(!ModuleList.count(ModuleName)) {
      ErrorMessage = "Could not find the module '" + ModuleName + "' referenced in the file '" + FileName + "'";
      COL_TRC(ErrorMessage);
      COLrespondError(Address, ErrorMessage);
      return pLoop->post(pCallback);
   }
   GITUmoduleGetCommits(ComponentDir, "", "", &ModuleList);
   GITUmodule Library = ModuleList[ModuleName];
   bool       Success = CMPCinitializeLibrary(Username, Component, Library, ModuleName, &ErrorMessage);
   Success ? COLrespondSuccess(Address) : COLrespondError(Address, ErrorMessage);
   pLoop->post(pCallback);
}

// /component/library/initialize
void CMPLlibraryInitialize(const COLwebRequest& Request, CFGmap* pMap, SCKloop* pLoop) {
   COL_FUNCTION(CMPLlibraryInitialize);
   COL_VAR(Request.Data.json(1));
   const COLstring Component   = COLvarRequiredString(Request.Data, "component");
   bool            Development = COLvarRequiredBool(Request.Data, "development");
   if(!Development) { return COLrespondError(Request.Address, "Cannot modify file structure of a run repository."); }
   CFGconfig* pConfig = CFGmapNode(pMap, Component);
   if(!pConfig) { return COLrespondError(Request.Address, Component); }
   COLstring Error;
   if(!PRMcheckRoleAccess(Request.FromTranslator, Request.User, pConfig->CoreConfig.Guid, ROLid::EDIT_SCRIPT, &Error)) {
      return COLrespondError(Request.Address, PRMsetError(ROLid::EDIT_SCRIPT));
   }
   COLvar Data;
   Data["component"] = Component;
   Data["username"]  = Request.User;
   Data["name"]      = COLvarRequiredString(Request.Data, "module_name");
   pLoop->threadPool()->scheduleTask(COLnewClosure0(&CMPLlibraryInitializeImpl, Data, Request.Address, pLoop,
                                                    CMPCcallback("/component/library/initialize")));
}
