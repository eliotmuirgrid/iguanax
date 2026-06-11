// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SERVutils
//
// Description:
//
// Implementation
//
// Author: Vismay Shah
// Date:   Friday 01 September 2023 - 01:35PM
// ---------------------------------------------------------------------------
#ifdef __linux__
#include <unistd.h>
#include <cstdio>
#endif
#ifdef _WIN32
#include <Windows.h>
#endif

#include <SERV/SERVutils.h>

#include <FIL/FILutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

static bool SERVisWindowsAdmin() {
   COL_FUNCTION(SERVremoveOld);
#ifdef _WIN32
   BOOL isAdmin = FALSE;
   HANDLE tokenHandle = NULL;
   if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &tokenHandle)) {
      TOKEN_ELEVATION elevation;
      DWORD size = sizeof(TOKEN_ELEVATION);
      if (GetTokenInformation(tokenHandle, TokenElevation, &elevation, sizeof(elevation), &size)) {
         isAdmin = elevation.TokenIsElevated;
      }
      CloseHandle(tokenHandle);
   }
   return isAdmin == TRUE;
#endif
   return false;
}

void SERVremoveOld(){
   COL_FUNCTION(SERVremoveOld);
   // As part of the upgrade process on windows, we rename iguana.exe to SERV_OLD_TEMP, so we clean this up here.
   if (FILfileExists(FILexecutableDir() + SERV_OLD_TEMP)) {
      COL_TRC(SERV_OLD_TEMP << " found");
      int Result = FILremove(FILexecutableDir() + SERV_OLD_TEMP);
      COL_VAR(Result);
   } else {
      COL_TRC(FILexecutableDir() << SERV_OLD_TEMP << " not found on startup.1");
   }
}

bool SERVisSudo() {
   COL_FUNCTION(SERVisSudo);
   bool IsAdmin = true;
#ifdef __linux__
   IsAdmin = geteuid() == 0;
#elif _WIN32
   IsAdmin = SERVisWindowsAdmin();
#endif
   if(!IsAdmin) { COLcout << "Failed to execute service command. Please make sure you have admin privilege." << newline; }
   return IsAdmin;
}