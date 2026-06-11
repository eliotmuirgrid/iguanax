// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: WEBcheck
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Tuesday 15 August 2023 - 01:17PM
// ---------------------------------------------------------------------------

#include <WEB/WEBcheck.h>

#include <CACHE/CACHEfile.h>

#include <FIL/FILutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

bool WEBcheck(){
   COL_FUNCTION(WEBcheck);
   if (!CACHEexists()){
      COLstring FileName = FILexecutableDir() + ".." + FIL_DIR_SEPARATOR + "web" + FIL_DIR_SEPARATOR + "index.html";
      COL_VAR(FileName);
      return FILfileExists(FileName);
   }
   COLstring Content;
   bool Success = CACHEreadFile("/web/index.html", &Content);
   COL_VAR(Success);
   return Success;
}