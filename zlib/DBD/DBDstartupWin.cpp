// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBDstartupWin
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Thursday 17 August 2023 - 10:46AM
// ---------------------------------------------------------------------------

#include <DBD/DBDstartupWin.h>
#include <DBD/DBDpreStart.h>

#include <INST/INSTisStartup.h>

#include <DBG/DBGdownloadPdb.h>
#include <USR/USRpermissions.h>

#include <SERV/SERVutils.h>

#include <CMD/CMDlineParser.h>

#include <stdlib.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

int DBDstartupWin(const CMDlineParser& LineParser, const COLstring& WorkingDir, DBDworld* pWorld, int argc, const char** argv){
   COL_FUNCTION(DBDstartupWin);
   COL_TRC("Create working directory if it does not already exist");
   DBDprepWorkingDir(WorkingDir, false);
   COLstring Error;
   bool Success = DBGextractPdbIfNeeded(&Error);
   COL_TRC("Remove old upgrade.");
   SERVremoveOld();
   if (INSTexeIsStartup() || LineParser.isFlagPresent("install")){
      DBDinstall(pWorld, argc, argv);
      return EXIT_SUCCESS;
   }
   COL_VAR2(Success, Error);
   USRensureAdminUserExists();
   return DBDrun(pWorld);
}