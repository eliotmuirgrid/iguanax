// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBDmigration
//
// Description:
//
// Implementation
//
// Author: Tyler Brown
// Date:   Friday 27 October 2023 - 12:36PM
// ---------------------------------------------------------------------------
#include <CFG/CFGconfigLoad.h>
#include <COL/COLhashmap.h>
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <DBD/DBDmigration.h>
#include <DIR/DIRpath.h>
#include <DIR/DIRpathOld.h>
#include <FIL/FILdirEnumerator.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <GIT/GITexecute.h>
#include <GIT/GITremote.h>
#include <GIT/GITutils.h>
#include <GITU/GITUconfigGlobal.h>
#include <GITC/GITCkeyPathReset.h>
#include <INI/INIdata.h>
#include <IWEB/IWEBconfig.h>
#include <IWEB/IWEBload.h>
#include <IWEB/IWEBsave.h>
#include <PASS/PASScreateSalt.h>
#include <PASS/PASSword.h>
#include <SERV/SERVworkingDir.h>
COL_LOG_MODULE;

static void DBDcreateDir(const COLstring& WorkingDir, const COLstring& SubDir) {
	COL_FUNCTION(DBDcreateDir);
	try { FILmakeFullDir(FILpathAppend(WorkingDir, SubDir), FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX); }
	catch(const COLerror& E) { COL_ERR(E.description()); }
}

static COLstring DBDoldServerInstanceDir(){
   COL_FUNCTION(DBDoldServerInstanceDir);
   FILdirEnumerator DirEnumerator(DIRoldconfigDirDefault() + "*", true, true);
   COLstring Dir;
   while (DirEnumerator.getFile(Dir)){
      if ((Dir[0] != '_')){
         return DIRoldconfigDirDefault() + Dir + FIL_DIR_SEPARATOR;
      } else {
         COL_TRC("Ignoring " << Dir);
      }
   }
   return "";
}

static void DBDmigrateFile(const COLstring& OldFile, const COLstring& NewFile){
   COL_FUNCTION(DBDmigrateFile);
	COL_TRC("Migrating " << OldFile << " to " << NewFile);
   if(!FILfileExists(OldFile)){
      COL_TRC("File does not exist, no need to move it: " + OldFile);
      return;
   }
   if(FILfileExists(NewFile)){
      COL_TRC("New file already exists: " + NewFile);
      return;
   }
   if(FILisDirectory(OldFile)){
      COL_TRC("File provided is a directory: " + OldFile);
      return;
   }
   FILrenameFile(OldFile, NewFile);
}

static void DBDremoveItem(const COLstring& Item){
   COL_FUNCTION(DBDremoveItem);
   if(!FILfileExists(Item)) { return; }
   COLstring RemoveError;
   COL_TRC("Attempting to remove old Iguana data in: " + Item);
   if(FILisDirectory(Item) && !FILremoveFullDirNewSafe(Item, &RemoveError)){ 
      COL_ERR("Could not remove: " + Item + ". " + RemoveError);
   } else { 
      FILremove(Item); 
   }
}

static void DBDrenameDir(const COLstring& OldDirPath, const COLstring& NewDirPath){
   COL_FUNCTION(DBDrenameDir);
   FILrenameDirectory(OldDirPath, NewDirPath, FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX, true);
}

// we need to copy:
//  - logt -> logt
//  - configurations -> configurations
//  - crashreport -> crashreport
//  - IguanaEnv.txt -> IguanaEnv.txt
//
//  - components/repository -> repos
//  - components/run -> run
//  - components/configurations -> configurations/components
//  - components/configurations/fields -> configurations/fields - (then we have to remove the components/configurations/fields folder)

void DBDmigration(const COLstring& WorkingDir){
   COL_FUNCTION(DBDmigration);
   COLstring OldWorkingDir = DBDoldServerInstanceDir();
   DIRoldconfigRootDirDefault();
   if(OldWorkingDir.is_null()){
      COL_TRC("There is no old IguanaX directory to migrate");
      return;
   }
   COL_TRC("There is an old IguanaX directory to migrate: " + OldWorkingDir);
   COL_TRC("Migrating to new working directory: " + WorkingDir);
   DBDrenameDir(OldWorkingDir + "logt",           WorkingDir + "logt");
   DBDrenameDir(OldWorkingDir + "configurations", WorkingDir + "configurations");
   DBDrenameDir(OldWorkingDir + "crashreport",    WorkingDir + "crashreport");
   DBDrenameDir(OldWorkingDir + "components" + FIL_DIR_SEPARATOR + "repository",     WorkingDir + "repos");
   DBDrenameDir(OldWorkingDir + "components" + FIL_DIR_SEPARATOR + "run",            WorkingDir + "run");
   DBDrenameDir(OldWorkingDir + "components" + FIL_DIR_SEPARATOR + "configurations", WorkingDir + "configurations" + FIL_DIR_SEPARATOR + "components");
   DBDrenameDir(WorkingDir+ "configurations" + FIL_DIR_SEPARATOR + "components" + FIL_DIR_SEPARATOR + "fields",  WorkingDir + "configurations" + FIL_DIR_SEPARATOR + "fields");
   DBDmigrateFile(OldWorkingDir + "IguanaEnv.txt", WorkingDir + "IguanaEnv.txt");
   DBDmigrateFile(OldWorkingDir + ".encrypt", WorkingDir + ".encrypt");

   COLstring IniWorkingDirSet = SERVworkingDir();
   if(!IniWorkingDirSet.is_null()){
      COL_TRC("Do not remove old working dir if it is set with the ini file");
      return;
   }

   COLstring OldRoot = DIRoldconfigRootDirDefault();
	DBDremoveItem(OldWorkingDir);
	DBDremoveItem(OldRoot);
}

static bool DBDdecryptFile(const COLstring& File, COLvar* pContent) {
	COL_FUNCTION(DBDdecryptFile);
	COL_VAR(File);
	if(!FILfileExists(File)) { return false; }
	try {
		COLstring Encrypted, Decrypted;
		FILreadFile(File, &Encrypted);
		if(!PASSdecrypt(Encrypted, &Decrypted)) { Decrypted = Encrypted; }
		if(!pContent->parse(Decrypted)) { return false; }
		return true;
	} catch(const COLerror& E) {
		COL_TRC(E.description());
		return false;
	}
}

static bool DBDremoveSuffix(COLstring* pComponent) {
	COL_FUNCTION(DBDremoveSuffix);
	const COLstring Suffix = "_fields";
	if(pComponent->size() >= Suffix.size() && pComponent->rfind(Suffix) == pComponent->size() - Suffix.size()) {
		pComponent->remove(pComponent->size() - Suffix.size());
		return true;
	} else {
		COL_TRC(*pComponent << " not a fields file -- continue");
		return false;
	}
}

static void DBDmigrateReports(const COLstring& WorkingDir) {
	COL_FUNCTION(DBDmigrateReports);
	const COLstring CrashReports = FILpathAppend(WorkingDir, "crashreport/");
	FILcopyDirectoryContents(CrashReports, DIRcrashDir(), FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX);
}

static bool DBDparseField(const COLvar& FieldData, const COLvar& ConfigData, COLvar* pComponentData) {
	COL_FUNCTION(DBDparseField);
	COL_VAR3(ConfigData.json(1), pComponentData->json(1), FieldData.json(1));
	if(!FieldData.isMap() || !FieldData["fields"].isMap() || !ConfigData.isMap() || !ConfigData["fields"].isArray()) {
		COL_TRC("Failed to parse fields");
		return false;
	}
	COLhashmap<COLstring, bool> FieldMap;  // maps custom field names to whether they need to be encrypted
	for(const auto& it : ConfigData["fields"].array()) { FieldMap.add(it["name"], it["type"].asString() != "password"); }
	for(const auto& it : FieldData["fields"].map()) {
		if(FieldMap.count(it.first) == 0) { continue; } // skip values not defined in config.json
		COLstring Value = it.second;
		if(FieldMap[it.first]) { PASSencrypt(it.second, &Value); }  // encrypt all values expect passwords since they are already encrypted
		(*pComponentData)["fields"][it.first] = Value;
	}
	COL_TRC("Successfully parsed fields");
	return true;
}

static bool DBDloadFieldsFiles(const COLstring& ConfigFile, const COLstring& CompJsonFile,
												  const COLstring& FieldsFile, COLvar* pConfigData, COLvar* pCompData,
												  COLvar* pFieldData) {
	COL_VAR3(ConfigFile, CompJsonFile, FieldsFile);
	DBDdecryptFile(FieldsFile, pFieldData); // doesnt have to exist
	if(!DBDdecryptFile(ConfigFile, pConfigData)) {
		COL_TRC("Failed to load " << ConfigFile);
		return false;
	}
	if(!DBDdecryptFile(CompJsonFile, pCompData)) {
		COL_TRC("Failed to load " << CompJsonFile);
		return false;
	}
	return true;
}

static void DBDmigrateFields(const COLstring& WorkingDir) {
	COL_FUNCTION(DBDmigrateFields);
	const COLstring FieldsDir	= FILpathAppend(WorkingDir, "configurations/fields/");
	const COLstring NewCompDir = FILpathAppend(WorkingDir, "config/comps/");
	FILdirEnumerator DirEnumerator(FieldsDir + "*", false, true);
	COLstring FieldJson;
	while(DirEnumerator.getFile(FieldJson)) {
		COLstring Component = FILpathNameWithoutLastExt(FieldJson);
		if(!DBDremoveSuffix(&Component)) { continue; }
		COL_VAR(Component);
		const COLstring CompJsonFile	 = FILpathAppend(NewCompDir, Component + "/" + "component.json");
		const COLstring ConfigJsonFile = FILpathAppend(NewCompDir, Component + "/dev/config.json");
		COLvar ConfigData, CompData, FieldData;
		if(!DBDloadFieldsFiles(ConfigJsonFile, CompJsonFile, FILpathAppend(FieldsDir, FieldJson), &ConfigData, &CompData, &FieldData)) { continue; }
		if(!DBDparseField(FieldData, ConfigData, &CompData)) { continue; }
		COL_VAR(CompData.json(1));
		try { FILwriteFile(CompJsonFile, CompData.json(1)); } catch(const COLerror& E) { COL_TRC(E.description()); }
	}
}

static COLstring DBDmigrateComponentsGetRemote(const COLstring& RunDir) {
   COL_FUNCTION(DBDmigrateComponentsGetRemote);
   if(!FILfileExists(RunDir)) { return ""; } // dev mode if no run dir
   COLstring Out, Error;
   if(!GITremote(RunDir, &Out, &Error)) { return ""; }
   COL_VAR(Out);
   return Out;
}

static COLstring DBDmigrateComponentsGetRunCommit(const COLstring& RunDir) {
   COL_FUNCTION(DBDmigrateComponentsGetRunCommit);
   if(!FILfileExists(RunDir)) { 
      COL_TRC("No run dir, assume dev mode");
      return "";
   }
   COLstring Out, Error;
   COLstring Command = "git rev-parse --short HEAD";
   COL_VAR2(RunDir, Command);
   int Code = GITexecuteString(RunDir, Command, &Out, &Error);
   COL_VAR3(Code, Out, Error);
   return Code == 0 ? COLstripWhiteSpace(Out) : "";
}

static void DBDmigrateComponentRunFields(const COLstring& CompsDir) {
   COL_FUNCTION(DBDmigrateComponentRunFields);
	FILdirEnumerator DirEnumerator(CompsDir + FIL_ANYFILE, true, true);
	COLstring Component;
	while(DirEnumerator.getFile(Component)) {
		const COLstring ConfigFile = DIRcomponentConfig(Component);
      COL_VAR(ConfigFile);
      try {
         if(!FILfileExists(ConfigFile)) { continue; }
         if(CFGcompJsonLimit < FILfileSize(ConfigFile)) { continue; } // will get taken care of later in TRNCinit
         COLstring Data;
         FILreadFile(ConfigFile, &Data);
         COLvar Config;
         Config.parse(Data);
         if(Config.exists("run_commit")) { continue; }
         const COLstring CompRunDir = FILpathAppend(CompsDir, Component + "/run/");
         const COLstring RunCommit	= DBDmigrateComponentsGetRunCommit(CompRunDir);
         const COLstring RunRepo		= DBDmigrateComponentsGetRemote(CompRunDir);
         const COLstring NewRunDir	= DIRcomponentRunDir(Component);
         Config["run_commit"] = RunCommit;
         if(!RunRepo.is_null()){ Config["run_repo"] = RunRepo; }
         COL_VAR(Config.json(1));
         FILwriteFile(ConfigFile, Config.json(1));
         if(FILfileExists(CompRunDir)) {
            FILcopyDirectoryContents(CompRunDir, NewRunDir, FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX);
            FILremoveFullDirNew(CompRunDir);
         }
         GITcleanupAllGitDirs(NewRunDir, false);
      } catch(const COLerror& E) { COL_TRC(E.description()); }
	}
}

static void DBDmigrateComponents(const COLstring& WorkingDir) {
	COL_FUNCTION(DBDmigrateComponents);
	const COLstring Repos		= FILpathAppend(WorkingDir, "repos/");
	const COLstring Run			= FILpathAppend(WorkingDir, "run/");
	const COLstring OldCompDir = FILpathAppend(WorkingDir, "configurations/components/");
	const COLstring NewCompDir = FILpathAppend(WorkingDir, "config/comps/");
	FILdirEnumerator DirEnumerator(OldCompDir + FIL_ANYFILE, false, true);
	COLstring CompJson;
	while (DirEnumerator.getFile(CompJson)){
		const COLstring Component	  = FILpathNameWithoutLastExt(CompJson) + FIL_PATH_SEP_NATIVE;
		const COLstring CompDir		  = FILpathAppend(NewCompDir, Component);
		const COLstring CompDevDir	  = FILpathAppend(CompDir, "dev/");
		const COLstring CompRunDir	  = FILpathAppend(CompDir, "run/");
		const COLstring NewJsonPath  = FILpathAppend(CompDir, "component.json");
		COL_VAR4(Component, CompDir, CompDevDir, CompRunDir);
		FILmakeFullDir(CompDir, FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX);
		DBDmigrateFile(OldCompDir + CompJson, NewJsonPath);
		FILcopyDirectoryContents(FILpathAppend(Repos, Component), CompDevDir,  FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX);
		FILcopyDirectoryContents(FILpathAppend(Run, Component), CompRunDir,  FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX);
		COLvar Json;
		if(DBDdecryptFile(NewJsonPath, &Json)) {
			try { FILwriteFile(NewJsonPath, Json.json(1)); } catch(const COLerror& E) { COL_TRC(E.description()); }
		} else {
			COL_TRC("Error getting json data for " << NewJsonPath);
		}
	}
   DBDmigrateComponentRunFields(NewCompDir);
}

static void DBDmoveOwnersFileAndCleanUpScratch(const COLstring& Username) {
   COL_FUNCTION(DBDmoveOwnersFileAndCleanUpScratch);
   // remove config/users/<Username>/owners.json. Iguana will regenerate and save it to resources/.scratch/<Username>/
   COLstring OwnersConfig = FILpathAppend(DIRconfigUserDir(Username), "owners.json");
   if (FILfileExists(OwnersConfig)) { FILremove(OwnersConfig); }
   // remove config/users/<Username>/.scratch/
   COLstring ScratchDir = FILpathAppend(DIRconfigUserDir(Username), ".scratch");
   if (FILfileExists(ScratchDir)) { 
      COLstring RemoveError;
      FILremoveFullDirNewSafe(ScratchDir, &RemoveError); // Not fatal to fail this remove. Need to use the safe version to prevent hanging after a start if we can't remove.
      COL_ERR(RemoveError);
   }
}

// assumes the user's gitconfig is already in the post-migration(new) location
static void DBDupdateUserGitConfigDefaultSshKeyPath(const COLstring& Username) {
   COL_FUNCTION(DBDupdateUserGitConfigDefaultSshKeyPath);
   INIdata Config;
   GITUconfigLoadUser(Username, &Config);
   COLstring KeyPath = GITUstripPathsQuotes(GITUconfigSshPath(Config));
   COLstring OldPath = FILpathResolve("configurations/users/" + Username + "/privatekey");
   if (KeyPath == OldPath) {
      COL_TRC("Update default SSH key path to " + DIRconfigUserSshFile(Username));
      GITUconfigSetSshPath(&Config, DIRconfigUserSshFile(Username));
      GITUconfigSaveUser(Username, Config);
   }
}

static void DBDmigrateUsers(const COLstring& WorkingDir) {
	COL_FUNCTION(DBDmigrateUsers);
   // move users dirs for v10.1.102 and below
	COLvar OldUserData;
	try {
		const COLstring OldUsersJson = FILpathAppend(WorkingDir, "configurations/users/users.json");
		COLstring UserData;
      if(FILfileExists(OldUsersJson)){
         FILreadFile(OldUsersJson, &UserData);
         OldUserData.parse(UserData);
      }
	} catch(const COLerror& E) { COL_TRC(E.description()); }
	COL_TRC(OldUserData.json(1));
	const COLstring OldUserDir = FILpathAppend(WorkingDir, "configurations/users/");
	FILdirEnumerator DirEnumerator(OldUserDir + "*", true, true);
	COLstring UserDir;
	while (DirEnumerator.getFile(UserDir)){
		COL_TRC(UserDir);
		const COLstring NewUserDir = DIRconfigUserDir(UserDir);
		DBDrenameDir(OldUserDir + UserDir, NewUserDir);
   }
	OldUserData.setMapType();
	for(const auto& it : OldUserData.map()) {
		COL_VAR(it.second.json(1));
		it.second["roles"] = it.second["tags"];
		it.second.erase("tags");
		try {
			const COLstring UserConfigDir = DIRconfigUserDir(it.first);
			if(!FILfileExists(UserConfigDir)) { FILmakeFullDir(UserConfigDir, FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX); }
			FILwriteFile(FILpathAppend(UserConfigDir, "user.config"), it.second.json(1));
		} catch(const COLerror& E) { COL_TRC(E.description()); }
	}
   // move owners.json,remove .scratch, update gitconfig for v10.1.103
   COLstring NewUserDir = DIRusersDir();
   FILdirEnumerator DirEnum(NewUserDir + "*", true, true);
   COLstring Username;
   while (DirEnum.getFile(Username)){
      COL_TRC(Username);
      DBDmoveOwnersFileAndCleanUpScratch(Username);
      DBDupdateUserGitConfigDefaultSshKeyPath(Username);
   }
}

// in case user put their own SSL cert key in configurations/web/, move them to the new folder.
// must be called after Iguana web files have been migrated.
static void DBDmoveWebFiles(const COLstring& WorkingDir) {
   COL_FUNCTION(DBDmoveWebFiles);
   COLstring OldWebDir = FILpathAppend(WorkingDir, "configurations/web");
   if (FILisDirectoryEmpty(OldWebDir)) { return; }
   FILcopyDirectoryContents(OldWebDir, DIRotherDir(), FIL_USER_RWX);
}

static void DBDupdateDefaultCertKeyPath(IWEBconfig& Config, bool& SaveConfig) {
   COL_FUNCTION(DBDupdateDefaultCertKeyPath);
   if (Config.httpsCertificateKeyFile() == FILpathSepToNative("configurations/web/cert.pem")) {
      COL_TRC("Update default SSL certificate path");
      Config.setHttpsCertificateKeyFile(DIRfileConfigWebCert());
      SaveConfig = true;
   }
   if (Config.httpsPrivateKeyFile() == FILpathSepToNative("configurations/web/key.pem")) {
      COL_TRC("Update default SSL certificate path");
      Config.setHttpsPrivateKeyFile(DIRfileConfigWebKey());
      SaveConfig = true;
   }
}

static bool DBDisFileInDirectory(const COLstring& FilePath, const COLstring& DirPath) {
   COL_FUNCTION(DBDisFileInDirectory);
   COLstring AbsoluteFilePath = FILpathResolve(FilePath);
   COLstring AbsoluteDirPath  = FILpathResolve(DirPath);
   COL_VAR2(AbsoluteFilePath, AbsoluteDirPath);
   bool FileInDir = AbsoluteFilePath.find(AbsoluteDirPath) != npos;
   COL_VAR(FileInDir);
   return FileInDir;
}

// migrates the ssl file if it's in configurations/ because configurations/ will be deleted
static bool DBDmigrateSslFile(const COLstring FilePath, COLstring* pNewPath) {
   COL_FUNCTION(DBDmigrateSslFile);
   COL_VAR(FilePath);
   if (! FILfileExists(FilePath))                           { return false; }
   if (! DBDisFileInDirectory(FilePath, "configurations"))  { return false; }

   COLstring Filename = FILpathName(FilePath);
   COLstring NewPath  = FILpathAppend(DIRotherDir(), Filename);
   FILcopyFile(FilePath, NewPath);
   *pNewPath = FILpathSepToNative("config/other/" + Filename);
   return true;
}

static void DBDupdateCertKeyPathInConfigDir(IWEBconfig& Config, bool& SaveConfig) {
   COL_FUNCTION(DBDupdateCertKeyPathInConfigDir);
   COLstring NewPath;
   if (DBDmigrateSslFile(Config.httpsCertificateKeyFile(), &NewPath)) {
      Config.setHttpsCertificateKeyFile(NewPath);
      SaveConfig = true;
   }
   if (DBDmigrateSslFile(Config.httpsPrivateKeyFile(), &NewPath)) {
      Config.setHttpsPrivateKeyFile(NewPath);
      SaveConfig = true;
   }
}

static void DBDmigrateWeb(const COLstring& WorkingDir) {
   COL_FUNCTION(DBDmigrateWeb);
   // move web config for v10.1.102 and below. it's already in the right place for v10.1.103
   DBDmigrateFile(FILpathAppend(WorkingDir, "configurations/web/web.json"), DIRfileConfigWeb());
   // move cert and key for v10.1.102 and below
   DBDmigrateFile(FILpathAppend(WorkingDir, "configurations/web/cert.pem"), DIRfileConfigWebCert());
   DBDmigrateFile(FILpathAppend(WorkingDir, "configurations/web/key.pem"),  DIRfileConfigWebKey());
   // move cert and key for v10.1.103
   DBDmigrateFile(FILpathAppend(WorkingDir, "config/settings/webcert.pem"), DIRfileConfigWebCert());
   DBDmigrateFile(FILpathAppend(WorkingDir, "config/settings/webkey.pem"),  DIRfileConfigWebKey());
   // update web config if it uses the default cert and key
   // must be done after moving web.json to the new location
   IWEBconfig Config;
   COLstring Error;
   if (!IWEBload(&Config, &Error)) { return; }
   // v10.1.103 moved the cert and key to config/settings/ without updating web.json so there is no
   // need to check for config/settings/web[cert|key].pem in web.json
   bool SaveConfig = false;
   DBDupdateDefaultCertKeyPath(Config, SaveConfig);
   DBDupdateCertKeyPathInConfigDir(Config, SaveConfig);
   if (SaveConfig) { IWEBsave(Config); }
   DBDmoveWebFiles(WorkingDir);
}

static void DBDmigrateTunnel(const COLstring& WorkingDir) {
   COL_FUNCTION(DBDmigrateTunnel);
   // move tunnel private key for v10.1.102 and below
   DBDmigrateFile(FILpathAppend(WorkingDir, "configurations/tunnel/private_key.pem"), DIRfileConfigTunnelKey());
   // move tunnel private key for v10.1.103
   DBDmigrateFile(FILpathAppend(WorkingDir, "config/settings/tunnelprivate_key.pem"), DIRfileConfigTunnelKey());
}

static void DBDmigrateRoles(const COLstring& WorkingDir) {
   COL_FUNCTION(DBDmigrateRoles);
   const COLstring NewRoleFile = DIRfileConfigRoles();
   // move roles file for v10.1.102 and below
   DBDmigrateFile(FILpathAppend(WorkingDir, "configurations/users/roles"), NewRoleFile);
   // roles became unencrypted after v10.1.103
   if (!FILfileExists(NewRoleFile)) { return; }
   try {
      COLstring RawData, Config;
      FILreadFile(NewRoleFile, &RawData);
      if (!PASSdecrypt(RawData, &Config)) {
         COL_ERR("Could not decrypt roles. It's probably already decrypted");
         return;
      }
      // convert to COLvar so we can pretty print the json
      COLvar RoleMapVar;
      if (!RoleMapVar.parse(Config)) {
         COL_ERR("Could not parse roles.json.");
         return;
      }
      // save roles config in plaintext
      FILwriteFile(NewRoleFile, RoleMapVar.json(1));
   } catch (const COLerror& Error) {
      COL_ERR("Failed to decrypt roles: " + Error.description());
   }
}

static void DBDmigrateSettings(const COLstring& WorkingDir) {
	COL_FUNCTION(DBDmigrateSettings);
	DBDmigrateFile(FILpathAppend(WorkingDir, "configurations/database/database.json"), DIRfileConfigDatabase());
	DBDmigrateFile(FILpathAppend(WorkingDir, "configurations/git/catalogue.json"), DIRfileConfigGitCatalogue());
	DBDmigrateFile(FILpathAppend(WorkingDir, "configurations/git/servers.json"), DIRfileConfigServers());
	DBDmigrateFile(FILpathAppend(WorkingDir, "configurations/notifications/notifications.json"), DIRfileConfigNotifications());
	DBDmigrateFile(FILpathAppend(WorkingDir, "configurations/logging/logging.json"), DIRfileConfigLogging());
	DBDmigrateFile(FILpathAppend(WorkingDir, "configurations/logging/log.json"), DIRfileConfigLog());
	DBDmigrateFile(FILpathAppend(WorkingDir, "configurations/auto_upgrade.json"), DIRfileConfigAutoUpgrade());

	DBDmigrateFile(FILpathAppend(WorkingDir, "mac_controller.pid"), FILpathAppend(WorkingDir, "resources/mac_controller.pid"));
	DBDmigrateFile(FILpathAppend(WorkingDir, "cacert.pem"), FILpathAppend(WorkingDir, "resources/cacert.pem"));
	DBDmigrateFile(FILpathAppend(WorkingDir, "IguanaEnv.txt"), DIRfileConfigEnvironment());
	COLstring PdbDir = FILpathAppend(WorkingDir, "pdb");
	if (FILfileExists(PdbDir) && FILisDirectory(PdbDir)) {
      COLstring RemoveError;
		FILremoveFullDirNewSafe(PdbDir, &RemoveError);	// Iguana will recreate the pdb file in resources/. Okay if the remove fails.
	}
}

// see IX-3360
void DBDmigration2(const COLstring& WorkingDir) {
	COL_FUNCTION(DBDmigration2);
	COL_VAR(WorkingDir);
	if(WorkingDir.is_null() || WorkingDir.isWhitespace()) { return; }
        // we use FILpathAppend instead of our DIR functions because FILworkingDir isnt setup at this point
	DBDcreateDir(WorkingDir, FILpathAppend("config", "other"));
	DBDcreateDir(WorkingDir, FILpathAppend("config", "settings"));
	DBDcreateDir(WorkingDir, FILpathAppend("config", "users"));
	DBDcreateDir(WorkingDir, FILpathAppend("resources", "reports"));
	DBDcreateDir(WorkingDir, FILpathAppend("config", "comps"));

	DBDmigrateFile(FILpathAppend(WorkingDir, "IguanaLicense"), DIRfileConfigLicense());
	DBDmigrateFile(FILpathAppend(WorkingDir, ".encrypt"), DIRfileConfigEncrypt());
	PASScreateSalt();  // We need to load the password file for when we migrate fields

	DBDmigrateSettings(WorkingDir);
	DBDmigrateWeb(WorkingDir);
	DBDmigrateTunnel(WorkingDir);
   DBDmigrateRoles(WorkingDir);
	DBDmigrateUsers(WorkingDir);
	DBDmigrateComponents(WorkingDir);
	DBDmigrateFields(WorkingDir);
	DBDmigrateReports(WorkingDir);

	// old files
	DBDmigrateFile(DIRoldfileConfigPermissions(), FILpathAppend(WorkingDir, "config/settings/permissions.json"));
	DBDremoveItem(FILpathAppend(WorkingDir, "git_cache/"));
	// DBDremoveItem(FILpathAppend(WorkingDir, "run/"));
	DBDremoveItem(FILpathAppend(WorkingDir, "repos/"));
	DBDremoveItem(FILpathAppend(WorkingDir, "configurations/"));
	DBDremoveItem(FILpathAppend(WorkingDir, "crashreport/"));
}