#ifndef __TRN_LOOP_MARK_H__
#define __TRN_LOOP_MARK_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNloop
//
// Description:
// 
// Routines which mark loop code and a helper to recognize the end of it.
//
// Author: Kevin Senn (originally)
// Date:   Saturday 29 April 2023 - 04:55PM
// ---------------------------------------------------------------------------

#include <COL/COLlist.h>

class TRNframe;
class COLstring;

// Add some "secret functions" to loops defined in the code in order to better track it.
bool TRNloopInsertEndCalls(COLstring* pCode);

// Detects the name of the special function inserted by the TRNloopInsertEndCalls
bool TRNloopIsSecretFunc(const char* pName);

void TRNloopOnEnd(COLlist<TRNframe>& Stack);

#endif // end of defensive include