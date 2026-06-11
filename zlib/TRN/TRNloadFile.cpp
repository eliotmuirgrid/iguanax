//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNloadFile
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Friday, November 26th, 2010 @ 09:58:21 PM
//
//---------------------------------------------------------------------------
#include "TRNloadFile.h"

#include <FIL/FILutils.h>
#include <FIL/FILfile.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;


bool TRNloadFile(const COLstring& FullFilePath, COLstring* pCode) {
   COL_FUNCTION(TRNloadFile);
   COL_VAR(FullFilePath);
   if (FILfileExists(FullFilePath)){  // TODO - do we need this? (MPRCcaseSensitiveFileExists(FullFilePath)) {
      pCode->clear();
      FILreadFile(FullFilePath, pCode);
      return true;
   } else {      
      COL_TRC("Failed to find " << FullFilePath);
      return false;
   }
}
