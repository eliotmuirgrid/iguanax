//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: LOGRsetDir.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   14/07/25 1:27 PM
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <FIL/FILutils.h>
#include <PRM/PRMcallbacks.h>
#include <LOGR/LOGRsetDir.h>
#include <LOGR/LOGRutils.h>
COL_LOG_MODULE;

static void LOGRsetLogDir(const COLstring& Key, const COLstring& LogDir, COLvar& LogConfig, COLvar& Result) {
   COL_FUNCTION(LOGRsetLogDir);
   Result[Key] = LogDir;
   if(LogDir.is_null() || LogDir.isWhitespace()) { return LogConfig.erase(Key); }
   LogConfig[Key] = LogDir;
}

static bool LOGRisValidLocation(const COLstring& Path) {
   COL_FUNCTION(LOGRisValidLocation);
   return Path.find(DIRconfigDir()) == npos && Path.find(DIRresourcesDir()) == npos &&
          Path.find(DIRcomponentRunDir("")) == npos;
}

static bool LOGRsetDirApply(const COLvar& Data, const COLstring& Key, COLvar& Config, COLvar& Result) {
   COL_FUNCTION(LOGRsetDirApply);
   if(Data.exists(Key)) {
      COLstring Path = Data[Key];
      FILaddPathSeparator(Path);
      if(!LOGRisValidLocation(Path)) { return false; }
      LOGRsetLogDir(Key, Path, Config, Result);
   }
   return true;
}

// /log/directory/set
void LOGRsetDir(const COLwebRequest& Request) {
   COL_FUNCTION(LOGRsetDir);
   COLstring Error;
   if(!PRMcheckRoleAccess(Request.FromTranslator, Request.User, "", CHANGE_LOG_FOLDER, &Error)) {
      return COLrespondError(Request.Address, PRMsetError(CHANGE_LOG_FOLDER));
   }
   COLvar Result, Config;
   if(!LOGRreadConfig(&Config, &Error)) { return COLrespondError(Request.Address, Error); }
   Config.setMapType();
   if(!LOGRsetDirApply(Request.Data, "log_dir", Config, Result)) {
      return COLrespondError(Request.Address, "Log directories cannot be within a configuration directory");
   }
   if(!LOGRsetDirApply(Request.Data, "alt_dir", Config, Result)) {
      return COLrespondError(Request.Address, "Log directories cannot be within a configuration directory");
   }
   if(FILsave(DIRfileConfigLog(), Config.json(1), &Error)) {
      COLrespondSuccess(Request.Address, Result);
   } else {
      COLrespondError(Request.Address, Error);
   }
}