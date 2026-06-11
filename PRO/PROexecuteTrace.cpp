// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: PROexecuteTrace
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Tuesday 10 October 2023 - 08:47AM
// ---------------------------------------------------------------------------

#include <PRO/PROexecuteTrace.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void PROexecuteTrace(const COLstring& Command){
   COL_TRC(Command);
}
