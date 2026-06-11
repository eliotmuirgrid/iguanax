//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: LOGRutils.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   16/07/25 12:00 PM
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <DIR/DIRpath.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <LOGR/LOGRutils.h>
COL_LOG_MODULE;

COLstring LOGRgetDefaultDir(const COLvar& LogConfig) {
   COL_FUNCTION(LOGRgetDefaultDir);
   if (LogConfig.exists("encryption") && LogConfig["encryption"].asBool()) {
      return FILpathAppend(FILworkingDir(), "loge/");
   }
   return FILpathAppend(FILworkingDir(), "logt/");
}

bool LOGRreadConfig(COLvar* pConfig, COLstring* pError) {
   COL_FUNCTION(LOGRreadConfig);
   try {
      const COLstring ConfigFile = DIRfileConfigLog();
      if(!FILfileExists(ConfigFile)) { return true; } // non-existent is ok
      COLstring Config;
      FILreadFile(ConfigFile, &Config);
      if(pConfig->parse(Config)) return true;
      *pError = "Failed to parse log configuration file";
   } catch(const COLerror& Error) {
      *pError = Error.description();
      COL_ERR(*pError);
   }
   return false;
}