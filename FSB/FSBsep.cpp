//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: FSBnewFolder
//
// Author: Matthew Nitsopoulos
//
// Description:
//
// Implementation
//---------------------------------------------------------------------------
#include "FSBvolumes.h"

#include <FIL/FILstorageManager.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>

#include <PRO/PROenv.h>

#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLlog.h>
#include <COL/COLweb.h>
COL_LOG_MODULE;

// /file_browser/sep
void FSBfileBrowserSep(const COLwebRequest& Request) {
   COL_FUNCTION(FSBfileBrowserSep);
   COLvar Result;
   Result["file_sep"] = FIL_DIR_SEPARATOR;
   COL_VAR(Result.json(1));  
   COLrespondSuccess(Request.Address, Result);
}