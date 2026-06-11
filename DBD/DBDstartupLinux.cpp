// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBDstartupLinux
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Thursday 17 August 2023 - 10:46AM
// ---------------------------------------------------------------------------

#include <CMD/CMDlineParser.h>
#include <DBD/DBDpreStart.h>
#include <DBD/DBDstartupLinux.h>
#include <GITU/GITUdetect.h>
#include <GUID/GUIDid.h>
#include <SERV/SERVutils.h>
#include <USR/USRgenerateAdminIfEmpty.h>
#include <USR/USRpermissions.h>
#include <stdlib.h>
#ifdef __linux__
#include <unistd.h>
#endif

#include <COL/COLlog.h>
COL_LOG_MODULE;

int DBDstartupLinux(const CMDlineParser& LineParser, const COLstring& WorkingDir, DBDworld* pWorld){
   COL_FUNCTION(DBDstartupLinux);
#ifdef __linux__
   if (geteuid() == 0) {
      COLcout << "Cannot run normal process as root" << newline;
      return EXIT_FAILURE;
   }
#endif
   COLstring GitPath;
   if(!GITUinstalled(&GitPath)){
      GITUnotInstalled(COLcout);
      return EXIT_FAILURE;
   }
   DBDprepWorkingDir(WorkingDir, false);
   COL_TRC("Remove old upgrade.");
   SERVremoveOld();
   return DBDrun(pWorld);
}
