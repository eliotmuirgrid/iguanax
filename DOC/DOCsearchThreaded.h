#ifndef __DOC_SEARCH_THREADED_H__
#define __DOC_SEARCH_THREADED_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DOCsearchThreaded
//
// Description:
//
// Threaded search - posts results back to the main thread.
//
// Author: Eliot Muir 
// Date:   Tuesday 07 October 2025 - 01:35PM
// ---------------------------------------------------------------------------

#include <COL/COLclosure.h>

class COLvar;
class COLstring;
class COLwebRequest;

void DOCsearchDirectionally(const COLstring& Start, const COLstring& Component, bool SearchForward, const COLstring& Id, COLclosure2<COLstring, bool*>* pCallback);


void DOCwebSearchDirectionally(const COLwebRequest& Request);


#endif // end of defensive include
