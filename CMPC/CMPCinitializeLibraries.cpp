//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: CMPCinitializeLibraries.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   28/05/25 2:56 PM
//  ---------------------------------------------------------------------------
#include <CFG/CFGconfig.h>
#include <CMPC/CMPCinitializeLibraries.h>
#include <CMPC/CMPCutils.h>
#include <COL/COLlog.h>
#include <DIR/DIRpath.h>
#include <FIL/FILutils.h>
#include <GIT/GITcheckout.h>
#include <GIT/GITpath.h>
#include <GITU/GITUaddRemote.h>
#include <GITU/GITUcacheRepoFromUrl.h>
#include <GITU/GITUmodules.h>
COL_LOG_MODULE;

bool CMPCinitializeLibrary(const COLstring& Username, const COLstring& ComponentId, const GITUmodule& Library,
                           const COLstring& ModuleName, COLstring* pErrorMessage) {
   COL_FUNCTION(CMPCinitializeLibrary);
   COL_VAR2(ComponentId, ModuleName);
   COL_VAR3(Library.GitUrl, Library.CommitId, Library.Branch);
   COLstring& ErrorMessage = *pErrorMessage;
   COLstring  Dest         = GITUmoduleDir(ComponentId, ModuleName);
   if(FILfileExists(Dest + ".git")) {
      COL_TRC("Library is already initialized");
      return true;
   }
   COLstring GitUrl   = Library.GitUrl;
   COLstring CommitId = Library.CommitId;
   COL_VAR2(GitUrl, CommitId);
   COLstring Output;
   if(GitUrl.isWhitespace()) {
      ErrorMessage = "Trouble parsing the giturl for '" + ModuleName + "'";
      COL_TRC(ErrorMessage);
      return false;
   }
   if(CommitId.isWhitespace()) {
      ErrorMessage = "Trouble parsing the commit id for '" + ModuleName + "'";
      COL_TRC(ErrorMessage);
      return false;
   }
   if(!GITUcacheRepoFromUrl(Username, GitUrl, "", &Output)) {
      ErrorMessage = "Trouble caching '" + GitUrl + "'";
      COL_TRC(ErrorMessage);
      return false;
   }
   COLstring Source = GITgitTemplateDir(GitUrl);
   if(!FILfileExists(Source)) {
      ErrorMessage = "Cannot find library contents to copy from git cache for '" + GitUrl + "' at '" + Source + "'";
      COL_TRC(ErrorMessage);
      return false;
   }
   FILcopyDirectoryContents(Source, Dest, FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX);
   // Needs to set upstream to the right GitUrl scheme
   if(!GITUaddRemote(Dest, GitUrl, &ErrorMessage)) {
      ErrorMessage =
          "Cannot checkout the library the giturl specified '" + GitUrl + "' at '" + Source + "'. " + ErrorMessage;
      COL_TRC(ErrorMessage);
      return false;
   }
   if(!GITcheckout(Dest, CommitId, &ErrorMessage)) {
      COL_TRC(ErrorMessage);
      return false;
   }
   return true;
}

bool CMPCinitializeAllLibraries(const COLstring& ComponentId, const COLstring& CommitId, const COLstring& Username, COLstring* pError) {
   COL_FUNCTION(CMPCinitializeAllLibraries);
   COLstring ComponentDir = DIRcomponentProjectDir(ComponentId, true);
   COL_VAR(ComponentDir);
   COLstring FileName = ComponentDir + ".gitmodules";
   if(!FILfileExists(FileName)) {
      COL_TRC("There is no '" + FileName + "' file and therefore no libraries (so no libraries to initialize).");
      return true;
   }
   COLmap<COLstring, GITUmodule> ModuleList;
   GITUmoduleLoad(FileName, &ModuleList);
   if(ModuleList.empty()) {
      COL_TRC("There are no libraries listed (so no libraries to initialize).");
      return true;
   }
   GITUmoduleGetCommits(ComponentDir, "", CommitId, &ModuleList);
   for(auto i = ModuleList.begin(); i != ModuleList.end(); ++i) {
      COLstring  ModuleName = i->first;
      GITUmodule Library    = i->second;
      if(!CMPCinitializeLibrary(Username, ComponentId, Library, ModuleName, pError)) {
         COL_TRC(*pError);
         return false;
      }
   }
   return true;
}
