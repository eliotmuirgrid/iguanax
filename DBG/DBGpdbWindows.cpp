// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBGpdbWindows
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Saturday 12 August 2023 - 08:38AM
// ---------------------------------------------------------------------------

#include <DBG/DBGpdb.h>

#include <FIL/FILutils.h>

#include <COL/COLwindows.h>
#include <DbgHelp.h>

#include <COL/COLerror.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

COLstring DBGprogramDatabase(){
   COL_FUNCTION(DBGprogramDatabase);
   COLstring Error;
   bool Success = COLdebugInit(&Error);
   if (!Success){
      COL_ERR("Unable to load symbols.");
      return "";
   }
   HANDLE Process = GetCurrentProcess();
   COL_VAR2(Process, FILexecutableFileName());
   HMODULE hModule = GetModuleHandle(FILexecutableFileName().c_str());
   COL_VAR(hModule);
   if (hModule){
      // Now get the module information.
      IMAGEHLP_MODULE64 moduleInfo = { sizeof(IMAGEHLP_MODULE64) };
      if (SymGetModuleInfo64(Process, (DWORD64)hModule, &moduleInfo)){
         if (strlen(moduleInfo.LoadedPdbName) == 0){
            COL_TRC("Empty module info - so no PDB");
            Success = COLdebugShutdown(&Error);
            COL_VAR2(Success, Error);
            return "";
         }
         COL_VAR(moduleInfo.LoadedPdbName);
         Success = COLdebugShutdown(&Error);
         COL_VAR2(Success, Error);
         return moduleInfo.LoadedPdbName;
      } else {
        COLstring Error = COLlastErrorAsString();
        COL_ERR(Error);
        Success = COLdebugShutdown(&Error);
        COL_VAR2(Success, Error);
        return "SymGetModuleInfo64 failed " + Error;
      }
   } else {
      Success = COLdebugShutdown(&Error);
      COL_VAR2(Success, Error);
      return "";
   }
}
