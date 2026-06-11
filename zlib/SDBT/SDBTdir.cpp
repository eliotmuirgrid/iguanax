// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBTdir
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Thursday 02 March 2023 - 01:27PM
// ---------------------------------------------------------------------------
#include <SDBT/SDBTdir.h>

#include <SDBT/SDBTfile.h>

#include <SDB/SDBdir.h>
#include <SDB/SDBepochTime.h>

#include <FIL/FILutils.h>
#include <FIL/FILpathUtils.h>

#include <COL/COLmath.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void SDBTcreateFullRangeDir(const COLstring& Dir){
   COL_FUNCTION(SDBTcreateFullRangeDir);
   COLstring Error;
   if (FILfileExists(Dir)){
      FILremoveFullDirNewSafe(Dir, &Error);
   }
   FILmakeDir(Dir, FIL_USER_RWX);
   for (int i=0; i <= 0xFF; i++){
      COLstring SubDir;
      SDBformatHexByte(i, &SubDir);
      COL_VAR(SubDir);
      FILmakeDir(Dir + FIL_DIR_SEPARATOR + SubDir, FIL_USER_RWX);
   }
}