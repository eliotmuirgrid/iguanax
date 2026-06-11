#ifndef __TRN_ANNOTATION_CLEANUP_H__
#define __TRN_ANNOTATION_CLEANUP_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNannotationCleanup
//
// Description:
//
// Functions to clean up annotation state
//
// Author: Eliot Muir
// Date:   Friday, November 19th, 2010 @ 10:34:00 AM
//---------------------------------------------------------------------------

#include <COL/COLhashmap.h>
#include <COL/COLlist.h>

class TRNfile;
class TRNcfunction;
class TRNannotationState;

void TRNcleanUpFiles(COLhashmap<COLstring, TRNfile*>& FileList);

// Don't recompile - just make sure Lua functions have no calls.
void TRNresetAnnotationState(TRNannotationState* pHandler);

#endif // end of defensive include
