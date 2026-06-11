// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITsystemPath
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Thursday 09 March 2023 - 11:19AM
// ---------------------------------------------------------------------------

#include <COL/COLlog.h>
#include <COL/COLsplit.h>
#include <COL/COLvar.h>
#include <COL/COLweb.h>
#include <FIL/FILutils.h>
#include <GIT/GITsystemPath.h>
#include <stdlib.h>
COL_LOG_MODULE;

// /system/path
void GITsystemPath(const COLwebRequest& Request){
   COL_FUNCTION(GITsystemPath);
   COLstring Result = getenv("PATH");
   COLvar List;
   COLsplit(&List, Result, FIL_PATH_SEPARATOR);   
   COLrespondSuccess(Request.Address, List);
}
