// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITnonInteractive
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Thursday 30 March 2023 - 10:13AM
// ---------------------------------------------------------------------------

#include <COL/COLlog.h>
#include <GIT/GITnonInteractive.h>
#include <PRO/PROenv.h>
COL_LOG_MODULE;

void GITsetNonInteractive(){
   COL_FUNCTION(GITsetNonInteractive);
   PROsetenv("GIT_TERMINAL_PROMPT", "0");
   PROsetenv("GCM_GUI_PROMPT", "0");
}
