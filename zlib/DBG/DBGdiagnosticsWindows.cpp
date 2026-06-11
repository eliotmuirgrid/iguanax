// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBGdiagnosticsWindows
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Saturday 12 August 2023 - 01:00PM
// ---------------------------------------------------------------------------

#include <DBG/DBGdiagnosticsWindows.h>

#include <COL/COLwindows.h>

#include <DbgHelp.h>

#include <COL/COLerror.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;


// Diagnositic functions
BOOL CALLBACK DBGenumModulesCallback(PCSTR ModuleName, DWORD64 BaseOfDll, PVOID UserContext) {
   COL_FUNCTION(DBGenumModulesCallback);
   // Get module information
   IMAGEHLP_MODULE64 moduleInfo = {};
   moduleInfo.SizeOfStruct = sizeof(moduleInfo);
   if (SymGetModuleInfo64(GetCurrentProcess(), BaseOfDll, &moduleInfo)) {
      COL_VAR2(ModuleName, moduleInfo.LoadedPdbName);
   }
   return TRUE;
}

void DBGenumPDB() {
   COL_FUNCTION(DBGenumPDB);
   HANDLE Process = GetCurrentProcess();
   COL_VAR(Process);
   // Enumerate modules and associated PDBs
   if (!SymEnumerateModules64(Process, DBGenumModulesCallback, NULL)) {
     long Err = ::GetLastError();
     COL_ERR("Failed to enumerate PDB files: " << COLerrorString(Err) << " code = " << Err);   
   }
}

BOOL CALLBACK DBGenumSourceFilesCallback(PSOURCEFILE pSourceFile, PVOID UserContext) {
   COL_FUNCTION(DBGenumSourceFilesCallback); 
   COL_TRC("Source:" << pSourceFile->FileName);
   return TRUE;
}

void DBGLdebugEnumSource(){
   COL_FUNCTION(DBGdebugEnumSource);
   HANDLE Process = GetCurrentProcess();
   COL_VAR(Process);
    // Enumerate source files (PDBs)
   BOOL Success = SymEnumSourceFiles(Process, 0, NULL, DBGenumSourceFilesCallback, NULL);
   if (Success != TRUE){
      long Err = ::GetLastError();
      COL_ERR("Failed to enumerate source files: " << COLerrorString(Err) << " code = " << Err);   
   } else {
      COL_TRC("The function succeeded.");
   }
}