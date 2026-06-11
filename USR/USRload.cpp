// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: USRload
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Sunday 18 September 2022 - 06:00PM
// ---------------------------------------------------------------------------

#include <USR/USRload.h>

#include <FIL/FILatomic.h>
#include <FIL/FILdirEnumerator.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>

#include <DIR/DIRpath.h>

#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;


static void USRmapUsersFromVar(COLmap<COLstring, USRuser>*  pUserMap, const COLvar& Input){
   COL_FUNCTION(USRmapUsersFromVar);
   if(Input.isNull() || !Input.isMap()) { return; }
   COLvar::COLvarMap::const_iterator i = Input.map().cbegin();
   for(; i != Input.map().cend(); i++) {
      COLvar UserVar = i->second;
      COL_VAR(i->second);
      COLstring Username = "";
      COLstring Password = "";
      if     (UserVar.exists("username"))    { Username    = UserVar["username"];}
      else if(UserVar.exists("Username"))    { Username    = UserVar["Username"];}
      if(Username.isWhitespace())            { continue;}
      if     (UserVar.exists("password"))    { Password    = UserVar["password"];}
      else if(UserVar.exists("Password"))    { Password    = UserVar["Password"];}
      USRuser User;
      User.setUsername(Username);
      User.setPlainPassword(Password);
      User.setRoles(UserVar.exists("tags") ? UserVar["tags"] : UserVar["roles"]);
      User.setProvider(UserVar.exists("provider")? UserVar["provider"] : "Local");
      User.setDisplayName(UserVar["display_name"].asString());
      if(UserVar.exists("external") && UserVar["external"].asBool()) { User.setExternal(); }
      (*pUserMap)[Username] = User;
   }
}

static bool USRdetectOldFormat(){
   COL_FUNCTION(USRdetectOldFormat);
   return FILfileExists(DIRfileConfigUsers());
}

static void USRrenameOldFormatFile(){
   COL_FUNCTION(USRrenameOldFormatFile);
   try {
      FILrenameFile(DIRfileConfigUsers(), DIRfileConfigUsers() + ".old");
   } catch (const COLerror& Error){
      COL_TRC("Error renaming users.json to users.json.old");
   }
}

static void USRloadOldFormat(COLmap<COLstring, USRuser>* pUserMap){
   COL_FUNCTION(USRloadOldFormat);
   COLvar UserMapVar;
   if(!FILserializeLoadConfig(&UserMapVar, DIRfileConfigUsers())){
      COL_TRC("Could not load users.  File probably not created yet.");
      return; 
   }
   USRmapUsersFromVar(pUserMap, UserMapVar);
}

static void USRsaveAll(COLmap<COLstring, USRuser>* pUserMap){
   COL_FUNCTION(USRsaveAll);
   for (const auto& it : *pUserMap){
      USRuser User = it->second;
      COLvar Content = User.toVar(true);
      USRmodifyVarForSave(Content);
      COLstring JsonOut = Content.json(1);
      COLstring FileName = DIRconfigUserDir(User.username()) + "user.config";
      try { 
         FILcreateDirectoryTreeToFile(FileName, FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX);
         FILwriteFile(FileName, JsonOut); 
      }
      catch (const COLerror& Error) {
         COL_TRC("Failed to save user " + it->first);
      }
   }
}

static void USRupdateUserFormat(COLmap<COLstring, USRuser>* pUserMap){
   COL_FUNCTION(USRupdateUserFormat);
   USRloadOldFormat(pUserMap);
   USRsaveAll(pUserMap);
   USRrenameOldFormatFile();
}

static void USRloadImpl(COLmap<COLstring, USRuser>* pUserMap){
   COL_FUNCTION(USRloadImpl);
   COLvar Users;
	const COLstring Root = DIRusersDir();
   FILdirEnumerator Enum(Root + "*", true);
   COLstring DirName;
   while(Enum.getFile(DirName)) {
      if (!Enum.isDirectory()) { continue; }
      COL_VAR(DirName);
      COLstring UserConfig = FILpathAppend(Root, DirName);
      UserConfig = FILpathAppend(UserConfig, "user.config");
      if (FILfileExists(UserConfig)){
         COL_TRC("FOUND USER " + DirName);
         Users[DirName];
         FILserializeLoadConfig(&Users[DirName], UserConfig);
      }
   }
   USRmapUsersFromVar(pUserMap, Users);
}

void USRload(COLmap<COLstring, USRuser>* pUserMap){
   COL_FUNCTION(USRload);
   if (USRdetectOldFormat()) {
      COL_TRC("Detected old format configuration file. Will load and save all users in new format");
      USRupdateUserFormat(pUserMap);
   } else {
      COL_TRC("No old format detected.");
      USRloadImpl(pUserMap);
   }
}
