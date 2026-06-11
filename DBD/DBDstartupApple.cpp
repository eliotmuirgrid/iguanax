// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBDstartupApple
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Thursday 17 August 2023 - 10:47AM
// ---------------------------------------------------------------------------

#include <DBD/DBDstartupApple.h>

#include <CMD/CMDlineParser.h>

#include <DBD/DBDpreStart.h>
#include <FIL/FILutils.h>

#include <PRO/PROlaunch.h>

#include <USR/USRpermissions.h>

#include <SERV/SERVutils.h>

#include <APP/APPmain.h>

#include <stdlib.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

#ifndef __APPLE__
// create dummy functions
void APPcocoaMain() {}
#endif 

// external in Objective C
void APPcocoaMain();

int DBDappleRun(DBDworld* pWorld, const COLstring& WorkingDir){
   COL_FUNCTION(DBDappleRun);
   DBDprepWorkingDir(WorkingDir, false);
   COL_TRC("Remove old upgrade.");
   SERVremoveOld();
   USRensureAdminUserExists();
   return DBDrun(pWorld);
}

int DBDappleRunAsController(const COLstring& WorkingDir){
   COL_FUNCTION(DBDappleRunAsController);
   DBDprepWorkingDir(WorkingDir, false);
   COL_TRC("Remove old upgrade.");
   SERVremoveOld();
   COLcout << "Running as controller" << newline;
   APPcocoaMain();
   return EXIT_SUCCESS;
}

int DBDstartupApple(const CMDlineParser& LineParser, const COLstring& WorkingDir, DBDworld* pWorld){
   COL_FUNCTION(DBDstartupApple);
   if (LineParser.isFlagPresent("controller")){
      COL_TRC("Forcing to run as controller");
      return DBDappleRunAsController(WorkingDir);      
   }
   if (LineParser.isFlagPresent("run")){
      COL_TRC("Forcing run");
      return DBDappleRun(pWorld, WorkingDir);
   }
   // If we running in the context of an app bundle and have not been launched by launchd APPlaunchAsController will be true.
   if (APPlaunchAsController()){
      return DBDappleRunAsController(WorkingDir);
   }
   if(!APPisControllerRunning()) {
      int Code = PROlaunch(FILexecutableDir(), "./" + FILexecutableFileName() + " --controller");
      COL_TRC("Controller launched with code " << Code << newline);
   }
   return DBDappleRun(pWorld, WorkingDir);
}