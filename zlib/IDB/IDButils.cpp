// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: IDButils
//
// Description: Implementation
//
// Author: Vismay Shah
// Date:   Tuesday, April 6th, 2021 @ 02:52:42 PM
//
//---------------------------------------------------------------------------
#include "IDButils.h"
#include <COL/COLlog.h>
#include <FIL/FILpathUtils.h> 
COL_LOG_MODULE;

COLstring IDBfileCorrectFileName(COLstring FileName){
   COL_FUNCTION(IDBfileCorrectFileName);
   if (!FileName.is_null()){
      // Strip leading and trailing quotes as the path might have been received from user input
      FileName = FileName.strip(COLstring::Both, '\"');
      FileName = FileName.strip(COLstring::Both, '\'');
      FileName = FILpathSepToNative(FileName);
   }
   return FileName;
}
