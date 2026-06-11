//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMPdirContent
//
// Description:
//
// Implementation
//
// Author: Tyler Brown
//---------------------------------------------------------------------------
#include <CFG/CFGconfigApi.h>
#include <CFG/CFGmap.h>
#include <CMP/CMPdirContent.h>
#include <CMP/CMPutils.h>
#include <COL/COLlog.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <FIL/FILdirEnumerator.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
COL_LOG_MODULE;

static COLvar CMPlistDirContents(COLstring const& FilePath, const COLvar& Exclude){
   COL_FUNCTION(CMPlistDirContents);
   COLvar DirContent;
   DirContent.setArrayType();
   FILdirEnumerator Enum(FilePath + "*", false, true);
   COLstring FileName;
   Enum.moveToFirst();
   while (Enum.getFile(FileName)) {
      if (Exclude.exists(FILpathExtLast(FileName, false))) { COL_TRC("Excluding " + FileName + " from results."); continue; }
      COLvar CurrentFileOrDir;
      CurrentFileOrDir["name"] = FileName;
      CurrentFileOrDir["dir"] = Enum.isDirectory();
      DirContent.push_back(CurrentFileOrDir);
   }
   return DirContent;
}

static void CMPdirContentImpl(const COLstring& ComponentId, const COLstring& Path, const COLvar& Exclude, const bool& Dev, const COLaddress& Address) {
   COL_FUNCTION(CMPdirContentImpl);
   COL_VAR3(ComponentId, Path, Dev);
   COLstring FilePath, FilePathAltered;
   COLstring RepoDir;
   FilePath = CFGresolveRepoFile(ComponentId, Path, Dev);
   COLvar Data;
   Data["component"] = ComponentId;
   // TODO - probably remove the whole filepath altered stuff
   FilePathAltered = FilePath;
   FILremovePathSeparator(FilePathAltered);
   Data["exists"] = true;
   if(!FILfileExists(FilePath)       && !FILfileExists (FilePathAltered)) { Data["exists"] = false; }
   else if(!FILisDirectory(FilePath) || !FILisDirectory(FilePathAltered)) { Data["dir"]    = false; }
   else {
      Data["dir"]     = true;
      Data["content"] = CMPlistDirContents(FilePath, Exclude);
   }
   COLrespondSuccess(Address, Data);
}

// "/component/dir"
void CMPdirContent(const COLwebRequest& Request) {
   COL_FUNCTION(CMPdirContent);
   COLvar Data = Request.Data;
   COLstring ComponentId, Error;
   COL_VAR(Data);
   bool IsDev     = COLvarRequiredBool(Data, "development");
   COLvar Exclude = COLvarGetObject(Data, "exclude", COLvar());
   if (!CFGcomponentId(Data, &ComponentId, &Error)){
      return COLrespondError(Request.Address, Error);
   }
   ROLid Id = IsDev ? ROLid::EDIT_SCRIPT : ROLid::VIEW_SCRIPT;
   if (!PRMcheckRoleAccess(Request.FromTranslator, Request.User, ComponentId, Id, &Error)){
      return COLrespondError(Request.Address, PRMsetError(Id));
   }
   CMPdirContentImpl(
      ComponentId,
      COLvarRequiredString(Data, "path"), // Path gets resolved to last directory so include trailing slash if needed
      Exclude,
      IsDev,
      Request.Address);
}
