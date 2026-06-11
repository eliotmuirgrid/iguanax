//---------------------------------------------------------------------------
// Copyright (C) 1997-2020 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TPJupdateTree
//
// Description:
//
// Implementation
//
// Author: Tyler Brown
//---------------------------------------------------------------------------
#include <CFG/CFGconfig.h>
#include <CFG/CFGconfigLoad.h>
#include <CFG/CFGconfigApi.h>
#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <COL/COLvar.h>
#include <COL/COLweb.h>
#include <FIL/FILdirEnumerator.h>
#include <FIL/FILpathUtils.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
#include <TPJ/TPJupdateTree.h>
COL_LOG_MODULE;

static void TPJenumerateDirectory(const COLstring& Directory, const COLstring& ComponentDir, const COLvar& OpenDirMap, COLvar* pOutput);

static void TPJmakeDirFrags(const COLstring& Path, COLvar& FragArray, COLstring& PartialPath) {
   COL_FUNCTION(TPJmakeDirFrags);
   COLstring  RestOfPath, LastPartOfPath;
   const bool SplitSuccess = Path.split(RestOfPath, LastPartOfPath, FIL_DIR_SEPARATOR);
   PartialPath             = FILpathAppend(PartialPath, RestOfPath);
   FragArray.push_back(PartialPath);
   if(SplitSuccess) { return TPJmakeDirFrags(LastPartOfPath, FragArray, PartialPath); } // Still more in the path, call the function again
}

static void TPJfileInformation(const COLstring& FileName, const COLstring& Directory, const COLstring& ComponentDir, const COLvar& OpenDirMap, 
                               const FILdirEnumerator& Enum, const bool& Hidden, COLvar* pOutput){
   COL_FUNCTION(TPJfileInformation);
   COL_VAR(FileName);
   COLstring PathRelativeToProjectDir = FILpathAppend(Directory, FileName);
   PathRelativeToProjectDir.replace(ComponentDir + FIL_DIR_SEPARATOR,"");
   COLvar CurrentFileOrDir;
   CurrentFileOrDir["name"]     = FileName;
   CurrentFileOrDir["contents"].setArrayType();
   CurrentFileOrDir["filepath"] = Directory + FIL_DIR_SEPARATOR + FileName;
   CurrentFileOrDir["path"]     = PathRelativeToProjectDir;
   if (Enum.isDirectory() && !Hidden) {
      CurrentFileOrDir["type"] = "dir";
      COL_VAR2(Directory, FileName);
      if (OpenDirMap.exists(Directory + FIL_DIR_SEPARATOR + FileName)) {
         TPJenumerateDirectory(Directory + FIL_DIR_SEPARATOR + FileName, ComponentDir, OpenDirMap, &(CurrentFileOrDir["contents"]));
         CurrentFileOrDir["state"]    = "open";
      } else {
         CurrentFileOrDir["state"]    = "closed";
      }
   } else {
      CurrentFileOrDir["type"]     = "file";
      CurrentFileOrDir["state"]    = "file";
   }
   pOutput->push_back(CurrentFileOrDir);
}

static void TPJenumerateDirectory(const COLstring& Directory, const COLstring& ComponentDir, const COLvar& OpenDirMap, COLvar* pOutput) {
   COL_FUNCTION(TPJenumerateDirectory);
   COL_VAR2(Directory, ComponentDir);
   const COLstring DirectoryToExpand = FILpathAppend(FILworkingDir(), Directory + FIL_DIR_SEPARATOR);
   COL_TRC("Expanding directory: " + DirectoryToExpand);
   FILdirEnumerator Enum(DirectoryToExpand + FIL_ANYFILE, false, true);
   COLstring        FileName;
   Enum.moveToFirst();
   while (Enum.getFile(FileName)) {
      if(FileName == ".gitmodules" || FileName == ".gitignore") { continue; }
      if(FileName == ".git") { continue; }
      TPJfileInformation(FileName, Directory, ComponentDir, OpenDirMap, Enum, false, pOutput);
   }
   FILdirEnumerator HiddenEnum(DirectoryToExpand + ".*", false, true);
   while (HiddenEnum.getFile(FileName)){
      if(FileName == ".gitmodules" || FileName == ".gitignore"){
         TPJfileInformation(FileName, Directory, ComponentDir, OpenDirMap, Enum, true, pOutput);
      }
   }
}

static void TPJrecurseAndPopulateTree(const COLstring& ProjectPath, const COLvar& OpenDirMap, COLvar& Response) {
   COL_FUNCTION(TPJrecurseAndPopulateTree);
   COLstring RestOfPath, LastPartOfPath;
   COL_TRC("Adding root " + ProjectPath);
   COLvar CurrentRoot;
   ProjectPath.rsplit(RestOfPath, LastPartOfPath, FIL_DIR_SEPARATOR);
   CurrentRoot["filepath"]     = ProjectPath;
   CurrentRoot["path"]         = "";
   CurrentRoot["name"]         = LastPartOfPath;
   CurrentRoot["type"]         = "dir";
   CurrentRoot["project_root"] = true;
   CurrentRoot["contents"].setArrayType();
   if(OpenDirMap.exists(ProjectPath)) {
      CurrentRoot["state"] = "open";
      TPJenumerateDirectory(ProjectPath, ProjectPath, OpenDirMap, &(CurrentRoot["contents"]));
   } else {
      CurrentRoot["state"] = "closed";
   }
   Response.push_back(CurrentRoot);
}

static void TPJmakeOpenDirMap(const COLvar& OpenDirList, COLvar& OpenDirMap){
   COL_FUNCTION(TPJmakeOpenDirMap);
   for(int i = 0; i < OpenDirList.size(); i++){
      COLvar Paths;
      COLstring PartialPath;
      COLstring PathToSplit = OpenDirList[i];
      FILcorrectPathSeparators(PathToSplit);
      TPJmakeDirFrags(PathToSplit, Paths, PartialPath);
      for (int j = 0; j < Paths.size(); j++){
         if (!OpenDirMap.exists(Paths[j].asString())) OpenDirMap[Paths[j].asString()] = 0;
      }
   }
}

static bool TPJgetSourceFromConfig(const COLstring& Component, COLstring* pPath, COLstring* pError, const bool IsDev) {
   COL_FUNCTION(TPJgetSourceFromConfig);
   CFGconfig Config;
   if(!CFGconfigLoad(Component, &Config)) {
      *pError = "Failed to load configuration file.";
      return false;
   }
   *pPath = CFGresolveRepoFile(Config.CoreConfig.Guid, "", IsDev);
   *pPath = pPath->substr(FILworkingDir().size());
   *pPath = FILpathSepRemove(*pPath);
   return true;
}

// /ide/project_tree
void TPJupdateProjectTree(const COLwebRequest& Request){
   COL_FUNCTION(TPJupdateProjectTree);
   const COLstring Component = Request.Data["component"];
   const bool      isDev     = Request.Data["development"];
   const ROLid     Id        = isDev ? EDIT_SCRIPT : VIEW_SCRIPT;
   COLstring ProjectFilePath, Error;
   if (!PRMcheckRoleAccess(Request.FromTranslator, Request.User, Component, Id, &Error)){
      return COLrespondError(Request.Address, PRMsetError(Id));
   }
   // First figure out the tree roots using the project file
   if (!TPJgetSourceFromConfig(Component, &ProjectFilePath, &Error, isDev)) {
      return COLrespondError(Request.Address, "Unable to determine project folder: " + Error);
   }
   if(!FILfileExists(ProjectFilePath)) {  // The top-level directory was not found, skip it
      return COLrespondError(Request.Address, "Project folder was not found");
   }

   // Convert the open_dirs input array into a map, combining fragments
   // eg: ["foo/bar/baz"] --> { "foo" : 0, "foo/bar" : 0, "foo/bar/baz" : 0 }
   COLvar OpenDirMap;
   TPJmakeOpenDirMap(Request.Data["open_dirs"], OpenDirMap);
   COL_VAR(OpenDirMap.json(1));

   COLvar Response;
   TPJrecurseAndPopulateTree(ProjectFilePath, OpenDirMap, Response);
   COLrespondSuccess(Request.Address, Response);
}