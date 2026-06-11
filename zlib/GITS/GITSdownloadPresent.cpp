// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITSdownloadPresent
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Thursday 13 July 2023 - 11:50AM
// ---------------------------------------------------------------------------

#include <GITS/GITSdownloadPresent.h>
#include <GITS/GITSinstallInfo.h>

#include <FIL/FILutils.h>

#include <COL/COLweb.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

// /git/download/present
void GITSdownloadPresent(const COLwebRequest& Request){
   COL_FUNCTION(GITSdownloadPresent);
   COLvar Result;
   Result["path"]    = GITSinstallerFile();
   Result["present"] = FILfileExists(GITSinstallerFile());
   COLrespondSuccess(Request.Address, Result);
}
