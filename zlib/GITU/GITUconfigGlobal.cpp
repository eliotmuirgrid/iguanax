// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITUconfigGlobal
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Saturday 25 March 2023 - 12:00PM
// ---------------------------------------------------------------------------

#include <COL/COLlog.h>
#include <DIR/DIRpath.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <GITU/GITUconfigGlobal.h>
#include <GITU/GITUcopyOldKey.h>
#include <COL/COLvar.h>
#include <INI/INIdata.h>
COL_LOG_MODULE;

COLstring GITUopenSshCommandPrefix(){ return "ssh -o \"StrictHostKeyChecking=no\" -o \"IdentitiesOnly=yes\" -i "; }

COLstring GITUconfigUserName  (const INIdata& Config) { return INIdataKeyGet(Config,"user", "", "name"); }
COLstring GITUconfigUserEmail (const INIdata& Config) { return INIdataKeyGet(Config,"user", "", "email");}
COLstring GITUconfigSshCommand(const INIdata& Config) { return INIdataKeyGet(Config,"core", "", "sshCommand"); }

COLstring GITUconfigSshPath(const INIdata& Config) {
   COL_FUNCTION(GITUconfigSshPath);
   COLstring SshCommand = GITUconfigSshCommand(Config);
   COLstring Prefix = GITUopenSshCommandPrefix();
   if(!COLstringHasPrefix(SshCommand, Prefix)){
      COL_TRC("Prefix does not match");
      return "";
   }
	COLstring Path = COLstringCutPrefix(SshCommand, Prefix);
	return Path;
}

void GITUconfigGlobalSetUserName(INIdata* pConfig, const COLstring& Value) { INIdataKeySet(pConfig, "user", "", "name",        Value); }
void GITUconfigGlobalSetEmail   (INIdata* pConfig, const COLstring& Value) { INIdataKeySet(pConfig, "user", "", "email",       Value); }
void GITUconfigSetSshCommand    (INIdata* pConfig, const COLstring& Value) { INIdataKeySet(pConfig, "core", "", "sshCommand",  Value); }

void GITUconfigSetSshPath(INIdata* pConfig, const COLstring& Path) {
   COL_FUNCTION(GITUconfigSetSshPath);
   COLstring Prefix = GITUopenSshCommandPrefix();
   COLstring Value = Prefix +  "'" + Path + "'";
   COL_VAR(Value);
   GITUconfigSetSshCommand(pConfig, Value);
}

COLstring GITUconfigGlobalFile(){
   COL_FUNCTION(GITUconfigGlobalFile);
   COLstring FileName = FILexpand("~/.gitconfig");
   return FileName;
}

static void GITUconfigUpdateSshPrefix(const COLstring& FileName, INIdata* pData){
   COL_FUNCTION(GITUconfigUpdateSshPrefix);
   if(pData->PrivateData.count("core")){
      if(pData->PrivateData["core"].count("sshCommand")){
         if(pData->PrivateData["core"]["sshCommand"].find("-o \"StrictHostKeyChecking=no\"") == npos){
            COL_TRC("Old prefix detected, updating with -o option.");
            COLstring NewSshPrefix = pData->PrivateData["core"]["sshCommand"];
            NewSshPrefix.replace("ssh -i ", GITUopenSshCommandPrefix());
            pData->PrivateData["core"]["sshCommand"] = NewSshPrefix;
            COLstring Out;
            INIformat(*pData, &Out);
            FILwriteFile(FileName, Out);
         }
      }
   }
}

bool GITUconfigLoad(const COLstring& FileName, INIdata* pData){
   COL_FUNCTION(GITUconfigLoad);
   COLstring Content;
   if (!FILfileExists(FileName)){
      COL_TRC("We have a fresh install of GIT with no " << FileName << " file.");
      return false;
   } try {
      FILreadFile(FileName, &Content);
      INIparse(Content, pData);  // TODO - consider error checking?
      GITUconfigUpdateSshPrefix(FileName, pData);
      return true;
   } catch(const COLerror& E) {
      COL_TRC(E.description());
      return false;
   }
}

bool GITUconfigLoadUser(const COLstring& Username, INIdata* pData){
   COL_FUNCTION(GITUconfigLoadUser);
   COLstring FileName = DIRconfigUserFile(Username);
   return GITUconfigLoad(FileName, pData);
}

bool GITUconfigGlobalLoad(INIdata* pData){
   COL_FUNCTION(GITUconfigGlobalLoad);
   COLstring FileName = GITUconfigGlobalFile();
   return GITUconfigLoad(FileName, pData);
}

void GITUconfigSaveFile(const COLstring& FileName, const INIdata& Data){
   COL_FUNCTION(GITUconfigSaveFile);
   COLstring Content;
   INIformat(Data, &Content);
   FILcreateDirectoryTreeToFile(FileName, FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX);
	COLstring Error;
	const bool Success = FILsave(FileName, Content, &Error);
	COL_VAR2(Success, Error);
}

void GITUinitializeUsersPrivateKey(const COLstring& Username, const INIdata& Config){
   COL_FUNCTION(GITUinitializeUsersPrivateKey);
   if(Username != "admin"){
      COL_TRC("Only copy the key on system if we are admin");
      return;
   }
   COLstring PrivateKeyPath = GITUconfigSshPath(Config);
   COLstring PrivateKeyPathFile = GITUstripPathsQuotes(PrivateKeyPath);
   // TODO - IX-3403
   COL_VAR(PrivateKeyPathFile);
   GITUcopyOldKey(PrivateKeyPathFile);
}

bool GITUgetUserEmail(const COLstring& Username, COLstring* pGitUser, COLstring* pGitEmail) {
   COL_FUNCTION(GITUgetUserEmail);
   INIdata Config;
   bool UserSuccess = GITUconfigLoadUser(Username, &Config);
   if(!UserSuccess){
      return false;

   }
   *pGitUser  = GITUconfigUserName(Config);
   *pGitEmail = GITUconfigUserEmail(Config);
   return true;
}

INIdata GITUinitializeConfigUser(const COLstring& Username){
   COL_FUNCTION(GITUinitializeConfigUser);
   INIdata Config;
   if(Username == "admin"){
      COL_TRC("Since we are admin we start with the currently configured values.");
      GITUconfigGlobalLoad(&Config);
   }
   COLstring PrivateKey = DIRconfigUserSshFile(Username);
   GITUconfigSetSshPath(&Config, PrivateKey);
   GITUconfigSaveUser(Username, Config);
   return Config;
}

void GITUconfigSaveUser(const COLstring& Username, const INIdata& Data){
   COL_FUNCTION(GITUconfigSaveUser);
   COLstring FileName = DIRconfigUserFile(Username);
   GITUconfigSaveFile(FileName, Data);
}

void GITUconfigGlobalSave(const INIdata& Data){
   COL_FUNCTION(GITUconfigGlobalSave);
   COLstring FileName = GITUconfigGlobalFile();
   GITUconfigSaveFile(FileName, Data);
}

COLstring GITUconfigPublicKey(const COLstring& PrivateKey) {
	COL_FUNCTION(GITUconfigPublicKey);
	bool LastCharIsQuote = !PrivateKey.is_null() && PrivateKey[PrivateKey.size() - 1] == '"';
	if(!LastCharIsQuote) { return PrivateKey + ".pub"; }
	return PrivateKey.substr(0, PrivateKey.size() - 1) + ".pub" + "\"";
}

COLstring GITUconfigPrivateKey(const COLstring& Username){
	COL_FUNCTION(GITUconfigPrivateKey);
	INIdata Config;
	bool UserSuccess = GITUconfigLoadUser(Username, &Config);
	if(!UserSuccess){
		COL_TRC("Initializing git user configuration file");
		Config = GITUinitializeConfigUser(Username);
		GITUinitializeUsersPrivateKey(Username, Config);
	}
	return GITUconfigSshPath(Config);
}

COLstring GITUstripPathsQuotes(const COLstring& Path){
	COL_FUNCTION(GITUstripPathsQuotes);
   COL_TRC("Stripping quotes");
   COLstring Output = Path.strip(COLstring::Both, '\"');
   Output = Output.strip(COLstring::Both, '\'');
   Output = Output.strip(COLstring::Both, '\"');
   return Output;
}
