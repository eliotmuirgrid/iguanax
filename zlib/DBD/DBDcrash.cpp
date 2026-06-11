//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBDcrash
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Thu 25 Feb 2021 11:12:04 EST
//---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <DBD/DBDcrash.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <SDB/SDBcapi.h>
COL_LOG_MODULE;

static bool DBDcrashRequested(const COLstring& CrashType) {
   COL_FUNCTION(DBDcrashRequested);
   COLstring FileName = "Iguana.test." + CrashType;
   COLstring FullName = FILpathAppend(FILworkingDir(), FileName);
   COL_VAR(FullName);
   if(FILfileExists(FullName)) {
      SDBIlogService("Found file " + FileName + " - about to " + CrashType + ".", "#warning");
      FILremoveWithThrow(FullName);
      return true;
   } else {
      COL_TRC(FullName << " not present.  So returning false.");
      return false;
   }
}

void DBDcrashTest(){
   COL_FUNCTION(DBDcrashTest);
   COLstring AbortOnAssertFileName = "Iguana.abort-on-assert";
   COLstring AbortOnAssertFullName = FILpathAppend(FILworkingDir(), AbortOnAssertFileName);
   if (DBDcrashRequested("segfault")){
      COL_TRC("Doing a segfault");
      int* CrashMe = 0;
      *CrashMe = 1;
   }
   if (DBDcrashRequested("abort")){
      abort();
   }
   if (DBDcrashRequested("assert")){
      COLASSERT(false);
   }
// TODO fix or replace now that we're not using MSVC
// #ifdef _WIN32 && defined(_DEBUG)
//    if (DBDcrashRequested("crt-error")){
//       _RPT0(_CRT_ERROR, "Simulated CRT Error...\n");
//    }
//    if (DBDcrashRequested("crt-assert")){
//       _RPT0(_CRT_ASSERT, "Simulated CRT Assert...\n");
//    }
// #endif
}
