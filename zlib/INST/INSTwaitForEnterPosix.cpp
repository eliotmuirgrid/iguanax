// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INSTwaitForEnterPosix
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Wed 08/09/2023 
// ---------------------------------------------------------------------------

#include <stdio.h>

#include <COL/COLerror.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void INSTwaitForEnter(int Seconds){
   COL_FUNCTION(INSTwaitForEnter);
   COLASSERT(1==0); 
   // this would require implementation using select and non blocking IO on stdin
   // Chat GPT will respond to the right prompts on this :-).
}
