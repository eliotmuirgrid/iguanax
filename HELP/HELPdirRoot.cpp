// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: HELPdirRoot
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Tuesday 25 April 2023 - 11:59AM
// ---------------------------------------------------------------------------
#include <HELP/HELPdirRoot.h>
#include <HELP/HELPutils.h>

#include <CACHE/CACHEfile.h>

#include <COL/COLweb.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

// /help/dir/root
void HELPdirRoot(const COLwebRequest& Request) {
   COL_FUNCTION(HELPdirRoot);
   COLvar Out;
   if(CACHEexists()) {
      Out["dir"] = CACHEhelpHash();
   } else {
      Out["dir"] = HELPglobalDir();
   }
   COLrespondSuccess(Request.Address, Out);
}