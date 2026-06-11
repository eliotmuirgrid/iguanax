// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: COLos
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Thursday 09 November 2023 - 02:12PM
// ---------------------------------------------------------------------------

#include <COL/COLos.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

COLstring COLos(){
   COLstring Os;
#ifdef __linux__
   Os = "linux";
#elif __APPLE__
   Os = "macos";
#else
   Os = "windows";
#endif
   COL_VAR(Os);
   return Os;
}
