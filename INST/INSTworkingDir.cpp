// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INSTworkingDir
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Tuesday 08 August 2023 - 01:17PM
// ---------------------------------------------------------------------------

#include <INST/INSTworkingDir.h>

#include <PRO/PROexecute.h>

#include <INI/INIdata.h>

#include <FIL/FILutils.h>
#include <FIL/FILpathUtils.h>

#include <DIR/DIRpath.h>

#include <COL/COLvarUtils.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>
#include <COL/COLweb.h>
COL_LOG_MODULE;

void INSTsaveWorkingDir(const COLstring& IniFile, const COLstring& WorkingDir){
   COL_FUNCTION(INSTsaveWorkingDir);
   INIdata InitData;
   if (FILfileExists(IniFile)) {
      COLstring InitRawData;
      FILreadFile(IniFile, &InitRawData);
      INIparse(InitRawData, &InitData);
   }
   InitData.PrivateData["server"]["working_dir"] = WorkingDir;
   COLstring IniContent;
   INIformat(InitData, &IniContent);
   FILwriteFile(IniFile, IniContent);
}

COLstring INSTgetWorkingDir(const COLstring& ExePath){
   COL_FUNCTION(INSTgetWorkingDir);
   COLstring ExeDir  = FILparentDir(ExePath);
   COLstring ExeName = FILpathNameWithoutLastExt(ExePath);
   COLstring IniPath = FILpathAppend(ExeDir, ExeName + ".ini");
   static COLstring DefaultWorkingDir = DIRconfigDirDefault();
   COL_VAR4(ExePath, ExeDir, IniPath, DefaultWorkingDir);
   if (!FILfileExists(ExeDir) || !FILfileExists(ExePath)) {
      COL_TRC("Fresh install to " << ExeDir);
      return DefaultWorkingDir;
   }
   if (!FILfileExists(IniPath)) {
      COL_TRC("Current installation uses default working dir");
      return DefaultWorkingDir;
   }
   COLstring InitContent;
   FILreadFile(IniPath, &InitContent);
   INIdata InitData;
   INIparse(InitContent, &InitData);
   if (0 == InitData.PrivateData.count("server") || 0 == InitData.PrivateData["server"].count("working_dir")) {
      COL_TRC("Current installation uses default working dir");
      return DefaultWorkingDir;
   }
   COLstring WorkingDir = InitData.PrivateData["server"]["working_dir"];
   FILaddPathSeparator(WorkingDir);
   return WorkingDir;
}

// /install/working_dir
void INSTworkingDir(const COLwebRequest& Request){
   COL_FUNCTION(INSTworkingDir);
#ifndef _WIN32
   return COLrespondSuccess(Request.Address, DIRconfigDirDefault());
#endif
   COLstring InstallPath = COLvarRequiredString(Request.Data, "install_path");
   return COLrespondSuccess(Request.Address, INSTgetWorkingDir(InstallPath));
}