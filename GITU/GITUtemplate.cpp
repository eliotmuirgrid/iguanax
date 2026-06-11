// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITUtemplate
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Tuesday 14 February 2023 - 12:34PM
// ---------------------------------------------------------------------------
#include <SDB/SDBcapi.h>
#include <COL/COLlog.h>
#include <DIR/DIRpath.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <GITU/GITUaddRemote.h>
#include <GIT/GITcheckout.h>
#include <GITU/GITUmodules.h>
#include <GIT/GITpath.h>
#include <GITU/GITUtemplate.h>
COL_LOG_MODULE;

void GITUcreateDirCopy(const COLstring& SourceDir, const COLstring& DestDir){
   COL_FUNCTION(GITUcreateDirCopy);
   COL_VAR2(SourceDir, DestDir);
   FILmakeFullDir(DestDir, FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX);
   FILcopyDirectoryContents(SourceDir, DestDir, FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX);
}

bool GITUinitSubmoduleAndUpstream(const COLstring& Dir, const COLmap<COLstring, GITUmodule>& ModuleList, COLstring* pError){
   COL_FUNCTION(GITUinitSubmoduleAndUpstream);
   bool Success = true;
   for (auto i = ModuleList.begin(); i != ModuleList.end(); ++i){
      COLstring Dest = FILpathAppend(Dir, i->first + FIL_DIR_SEPARATOR);
      COL_VAR2(i->first, Dest);
      COLstring GitUrl = i->second.GitUrl;
      COLstring Branch = i->second.Branch;
      COL_VAR2(GitUrl, Branch);
      COLstring Source = GITgitTemplateDir(GitUrl, Branch);
      COL_VAR2(Dest, Source);
      bool Exists = FILfileExists(Source + ".git");
      if(!Exists){
         SDBIlogService(i->first + " has no upstream to pull from. Defaulting to blank.", "#error");
         *pError = "Could not find repository: " + GitUrl + ". Try doing 'Refresh Cache' in the component add screen. ";
         Success = false;
         Source = GITUblankTemplateDir();
      }
      GITUcreateDirCopy(Source, Dest);
      COL_TRC("Copied " << Source << " to " << Dest);
      if(!GITUaddRemote(Dest, GitUrl, pError)) Success = false;
   }
   return Success;
}

void GITUcheckoutSubmoduleCommits(const COLstring& Dir, const COLmap<COLstring, GITUmodule>& ModuleList, COLstring* pError){
   COL_FUNCTION(GITUcheckoutSubmoduleCommits);
   COL_VAR(Dir);
   for (auto i=ModuleList.begin(); i != ModuleList.end(); ++i){
      COLstring ModuleDir = FILpathAppend(Dir, i->first + FIL_DIR_SEPARATOR);
      COL_VAR2(ModuleDir, i->second.CommitId);
      if (!GITcheckout(ModuleDir, i->second.CommitId, pError)){
         COL_TRC("Error: " << *pError);
         *pError = *pError + " Unable to checkout to commit in library: " + i->first;
      }
      COL_TRC("Checked out commit " << i->second.CommitId << " for " << i->first);
   }
}

// this only should set up the dev folder, make another function to set up the run dir that we call selectively
bool GITUtemplateCreate(const COLstring& ComponentId, const COLstring& GitUrl, const COLstring& Branch, COLstring* pError){
   COL_FUNCTION(GITUtemplateCreate);
   const COLstring TemplateDir = GITgitTemplateDir(GitUrl, Branch);
   COL_VAR3(GitUrl, Branch, TemplateDir);
   if (!FILfileExists(TemplateDir)){
      COL_ERR("Could not find template dir " << TemplateDir);
      *pError = "Could not find " + GitUrl;
      return false;
   }
   const COLstring ComponentDir = DIRcomponentDevDir(ComponentId);
   GITUcreateDirCopy(TemplateDir, ComponentDir);
   COL_VAR2(ComponentDir, TemplateDir);
   const COLstring FileName = GITUsubmoduleFile(GitUrl, Branch);
   if(!FILfileExists(FileName)) { return true; }
   COLmap<COLstring, GITUmodule> ModuleList;
   GITUmoduleLoad(FileName, &ModuleList);
   COL_TRC("Got module info");
   const bool Success = GITUinitSubmoduleAndUpstream(ComponentDir, ModuleList, pError);
   COL_TRC("Step 4 switch to the right commits.");
   GITUmoduleGetCommits(TemplateDir, "", "", &ModuleList);
   GITUcheckoutSubmoduleCommits(ComponentDir, ModuleList, pError);
   return Success;
}


void GITUintializeSubmodulesSubmodules(const COLstring& Dir, const COLstring& RelPath, COLstring* pError){
   COL_FUNCTION(GITUintializeSubmodulesSubmodules);
   COL_VAR2(Dir, RelPath);
   COLstring FileName = FILpathAppend(Dir, ".gitmodules");
   if(!FILfileExists(FileName)){
      COL_TRC("No submodules to initialize");
      return;
   }
   COLmap<COLstring, GITUmodule> ModuleList;
   GITUmoduleLoad(FileName, &ModuleList);
   GITUinitSubmoduleAndUpstream(Dir, ModuleList, pError);
   GITUmoduleGetCommits(Dir, "", "", &ModuleList);
   GITUcheckoutSubmoduleCommits(Dir, ModuleList, pError);
}