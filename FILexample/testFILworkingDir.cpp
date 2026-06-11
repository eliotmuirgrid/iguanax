// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testFILworkingDir
//
// Description:
//
// Implementation of tests
//
// Author: Eliot Muir 
// Date:   Thursday 09 November 2023 - 11:18AM
// ---------------------------------------------------------------------------
#include "testFILworkingDir.h"

#include <UNIT/UNITapp.h>

#include <FIL/FILutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void FILinitExecutableNameImp(const COLstring& OriginalExecutableFilePath, COLstring* pExeFilename, COLstring* pExeDir);

static void testFILinitExe(){
   COL_FUNCTION(testFILinitExe);
   COLstring File, Dir;
   FILinitExecutableNameImp("/life/test", &File, &Dir);
   COL_VAR2(File, Dir);
   UNIT_ASSERT_EQUALS("test", File);
   #ifdef _WIN32
   UNIT_ASSERT_EQUALS(COLstring("C:\\life\\").toLowerCase(), COLstring(Dir).toLowerCase());
   #else
   UNIT_ASSERT_EQUALS("/life/", Dir);
   #endif
}

void testFILworkingDir(UNITapp* pApp) {
   pApp->add("test/init/exe", &testFILinitExe);
}
