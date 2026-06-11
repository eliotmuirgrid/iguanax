//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: CMPLlibraryRenameContent.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   27/03/25 10:27 AM
//  ---------------------------------------------------------------------------

#include <CFG/CFGconfig.h>
#include <CFG/CFGmapApi.h>
#include <CFG/CFGmap.h>
#include <CMPC/CMPCutils.h>
#include <CMPL/CMPLlibraryRename.h>
#include <CMPL/CMPLlibraryRenameContent.h>
#include <CMPL/CMPLlibraryRemoteRemove.h>
#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <FIL/FILdirEnumerator.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <GITU/GITUutils.h>
#include <INI/INIdata.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
#include <SCK/SCKloop.h>
#include <SRCH/SRCHreplaceAll.h>

COL_LOG_MODULE;

void CMPLreplaceFileNameReferences(const COLstring& RootDir, const COLstring& OldFileName,
                                   const COLstring& NewFileName) {
   COL_FUNCTION(CMPLreplaceFileNameReferences);
   COLstring OldFile = FILpathNameWithoutLastExt(OldFileName);
   COLstring NewFile = FILpathNameWithoutLastExt(NewFileName);
   COL_VAR3(RootDir, OldFile, NewFile);
   // A way of targeting lua paths with the name instead of filename that appears as content
   SRCHreplaceAllInit(RootDir, "." + OldFile + ".", "." + NewFile + ".", true, true);
   SRCHreplaceAllInit(RootDir, "." + OldFile, "." + NewFile, true, true);
   SRCHreplaceAllInit(RootDir, OldFile + ".", NewFile + ".", true, true);
   SRCHreplaceAllInit(RootDir, ":" + OldFile, ":" + NewFile, true, true);
}

COLstring CMPLnewFileName(const COLstring& FileName, const COLstring& OldLibraryName, const COLstring& NewLibraryName) {
   COL_FUNCTION(CMPLnewFileName);
   COLstring NewFileName = FileName;
   if(COLstringHasPrefix(NewFileName, OldLibraryName)) {
      COL_TRC("Removing old library prefix");
      NewFileName = COLstringCutPrefix(NewFileName, OldLibraryName);
   }
   return NewLibraryName + NewFileName;
}

bool CMPLrenameLibraryFile(const COLstring& RootDir, const COLstring& Dir, const COLstring& OldFileName,
                           const COLstring& OldLibraryName, const COLstring& NewLibraryName, COLstring* pError) {
   COL_FUNCTION(CMPLrenameLibraryFile);
   COL_VAR4(Dir, OldFileName, OldLibraryName, NewLibraryName);
   COLstring OldFilePath = FILpathAppend(Dir, OldFileName);
   if(FILisDirectory(OldFilePath)) {
      COL_TRC("Don't name folders");
      return true;
   }
   COLstring NewFileName = CMPLnewFileName(OldFileName, OldLibraryName, NewLibraryName);
   COLstring NewFilePath = FILpathAppend(Dir, NewFileName);
   COL_VAR2(OldFilePath, NewFilePath);
   try {
      FILrenameFile(OldFilePath, NewFilePath);
      CMPLreplaceFileNameReferences(RootDir, OldFileName, NewFileName);
      // this needs to do a global search and replace of oldfilename with newfilename with whole word matching minus the
      // file ext
      return true;
   } catch(const COLerror& E) {
      COLstring Error = E.description();
      COL_ERR(Error);
      *pError = Error;
      return false;
   }
}

bool CMPLrenameLibraryFiles(const COLstring& RootDir, const COLstring& Dir, const COLstring& OldLibraryName,
                            const COLstring& NewLibraryName, COLstring* pError) {
   COL_FUNCTION(CMPLrenameLibraryFiles);
   bool             Success = true;
   FILdirEnumerator Enum(Dir + FIL_DIR_SEPARATOR + FIL_ANYFILE);
   try {
      COLstring OldFileName;
      while(Enum.getFile(OldFileName)) {
         const COLstring Path = FILpathAppend(Dir, OldFileName);
         if(FILisDirectory(Path)) { CMPLrenameLibraryFiles(RootDir, Path, OldLibraryName, NewLibraryName, pError); }
         if(!CMPLrenameLibraryFile(RootDir, Dir, OldFileName, OldLibraryName, NewLibraryName, pError)) {
            Success = false;
         }
      }
      return Success;
   } catch(const COLerror& E) {
      COLstring Error = E.description();
      COL_ERR(Error);
      *pError = Error;
      return false;
   }
}

static void CMPLlibraryRenameContentImpl(COLvar Data, COLaddress Address, SCKloop* pLoop, COLclosure0* pCallback) {
   COL_FUNCTION(CMPLlibraryRenameContentImpl);
   const COLstring Component      = Data["component"];
   const COLstring OldLibraryName = Data["old_name"];
   const COLstring NewLibraryName = Data["new_name"];
   const COLstring Path           = Data["path"];
   const COLstring Dir            = DIRcomponentDevDir(Component);
   const COLstring NewPath        = FILpathAppend(Dir, Path);
   COLstring       Error;
   CMPLlibraryRemoveUpstreamHelper(Component, Path);
   CMPLrenameLibraryFiles(Path, NewPath, OldLibraryName, NewLibraryName, &Error);
   SRCHreplaceAllInit(Dir, OldLibraryName, NewLibraryName, true, false);
   COLrespondSuccess(Address);
   pLoop->post(pCallback);
}

// /component/library/rename/content
void CMPLlibraryRenameContent(const COLwebRequest& Request, SCKloop* pLoop) {
   COL_FUNCTION(CMPLlibraryRenameContent);
   COLstring ComponentId, Error;
   bool Development = COLvarRequiredBool(Request.Data, "development");
   if(!Development) { return COLrespondError(Request.Address, "Cannot modify file structure of a run repository."); }
   if (!CFGcomponentId(Request.Data, &ComponentId, &Error)){
      return COLrespondError(Request.Address, Error);
   }
   if(!PRMcheckRoleAccess(Request.FromTranslator, Request.User, ComponentId, ROLid::EDIT_SCRIPT, &Error)) {
      return COLrespondError(Request.Address, PRMsetError(ROLid::EDIT_SCRIPT));
   }
   COLvar Data;
   Data["component"] = ComponentId;
   Data["new_name"]  = COLvarRequiredString(Request.Data, "new_name");
   Data["old_name"]  = COLvarRequiredString(Request.Data, "old_name");
   Data["path"]      = COLvarRequiredString(Request.Data, "path");
   pLoop->threadPool()->scheduleTask(COLnewClosure0(&CMPLlibraryRenameContentImpl, Data, Request.Address, pLoop,
                                                    CMPCcallback("/component/library/rename/content")));
}