// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBGpdbPosix
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Saturday 12 August 2023 - 08:38AM
// ---------------------------------------------------------------------------

#include <DBG/DBGpdb.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

COLstring DBGprogramDatabase(){
   COL_FUNCTION(DBDprogramDatabase);
   return "Loaded.";  // haha always since there are no dumb PDB files.
}

