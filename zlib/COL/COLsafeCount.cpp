// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: COLsafeCount
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Saturday 21 January 2023 - 08:09PM
// ---------------------------------------------------------------------------

#include <COL/COLsafeCount.h>

#include <COL/COLlocker.h>
#include <COL/COLmutex.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static COLmutex s_COLcountMutex;
int s_COLcount;

void COLincCount(){
   COLlocker Lock(s_COLcountMutex);
   s_COLcount++;
   COL_VAR(s_COLcount);
}

void COLdecCount(){
   COLlocker Lock(s_COLcountMutex);
   s_COLcount--;
   COL_VAR(s_COLcount);
}