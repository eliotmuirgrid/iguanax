// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITUconfig
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Saturday 25 March 2023 - 11:20AM
// ---------------------------------------------------------------------------

#include <COL/COLlog.h>
#include <FIL/FILutils.h>
#include <GITU/GITUconfig.h>
#include <GITU/GITUurl.h>
#include <INI/INIdata.h>
COL_LOG_MODULE;

void GITUconfigSetAutoLineEndingConversion(INIdata* pData){
   COL_FUNCTION(GITUconfigSetAutoLineEndingConversion);
   INIdataKeySet(pData, "core", "", "autocrlf", "false");
}

void GITUconfigSetDefaultAuthor(INIdata* pData){
   COL_FUNCTION(GITUconfigSetDefaultAuthor);
   INIdataKeySet(pData, "user", "", "name", "IguanaX");
   INIdataKeySet(pData, "user", "", "email", "iguanax@interfaceware.com");
}

void GITUconfigSetPullRebaseFalse(INIdata* pData){
   COL_FUNCTION(GITUconfigSetPullRebaseFalse);
   INIdataKeySet(pData, "pull", "", "rebase", "false");
}

void GITUconfigSetPushAutoSetupRemote(INIdata* pData){
   COL_FUNCTION(GITUconfigSetPushAutoSetupRemote);
   INIdataKeySet(pData, "push", "", "autoSetupRemote", "true");
}

COLstring GITUconfigRemoteOriginUrl(const INIdata& Config){ return INIdataKeyGet(Config, "remote", "origin","url"); }

void GITUconfigSubmoduleClear(INIdata* pConfig, const COLstring& SubmodulePath){
   COL_FUNCTION(GITUconfigSubmoduleClear);
   INIdataSectionRemove(pConfig, "submodule", SubmodulePath);
}

void GITUconfigRemoteClear(INIdata* pConfig, const COLstring& Branch){
   COL_FUNCTION(GITUconfigRemoteClear);
   INIdataSectionRemove(pConfig, "remote", "origin");     
   INIdataSectionRemove(pConfig, "branch", Branch);     
}

void GITUconfigRemoteOriginUrlSet(INIdata* pConfig, const COLstring& Url){
   COL_FUNCTION(GITUconfigRemoteOriginUrlSet);
   COLstring SetUrl;
   GITUcleanUrl(Url, &SetUrl);
   COL_VAR(SetUrl);
   INIdataKeySet(pConfig, "remote", "origin", "url", SetUrl);
}

void GITUconfigRemoteOriginFetchRuleSet(INIdata* pConfig){
   INIdataKeySet(pConfig, "remote", "origin", "fetch", "+refs/heads/*:refs/remotes/origin/*");
}

COLstring GITUconfigFile(const COLstring& RepoDir){
   COL_FUNCTION(GITUconfigFile);
   COLstring FileName = RepoDir;
   FILaddPathSeparator(FileName);
   FileName += COL_T(".git") + FIL_DIR_SEPARATOR + COL_T("config");
   COL_VAR2(RepoDir, FileName);
   return FileName;
}

bool GITUconfigLoad(const COLstring& RepoDir, INIdata* pData, COLstring* pError) {
   COL_FUNCTION(GITUconfigLoad);
   const COLstring  FileName = GITUconfigFile(RepoDir);
   COLstring  Content;
   if(!FILfileExists(FileName)) {
      *pError = "Could not load config file " + FileName + " since it does not exist";
      COL_TRC(*pError);
      return false;
   }
   try {
      FILreadFile(FileName, &Content);
      INIparse(Content, pData);  // TODO - consider error checking?
      return true;
   } catch(const COLerror& E) {
      COL_TRC(E.description());
      *pError = E.description();
      return false;
   }
}

void GITUconfigSave(const COLstring& RepoDir, const INIdata& Data){
   COL_FUNCTION(GITUconfigSave);
   COLstring FileName = GITUconfigFile(RepoDir);
   COLstring Content;
   INIformat(Data, &Content);
	COLstring Error;
	const bool Success = FILsave(FileName, Content, &Error);
	COL_VAR2(Success, Error);
}
