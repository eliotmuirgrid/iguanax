// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INSTisStartup
//
// Description:
//
// Implementation - windows
//
// Author: Eliot Muir 
// Date:   Thursday 10 August 2023 - 04:09PM
// ---------------------------------------------------------------------------

#include <INST/INSTisStartup.h>

#include <FIL/FILutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

bool INSTexeIsStartup(){
   COL_FUNCTION(INSTexeIsStartup);
   COLstring ExeName = FILexecutableFileName();
   ExeName = ExeName.toLowerCase();
   bool IsSetup = ExeName.find("setup") != npos;
   COL_VAR2(ExeName, IsSetup);
   return IsSetup;
}
