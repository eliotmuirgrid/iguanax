#ifndef __DOC_SEARCH_H__
#define __DOC_SEARCH_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DOCsearch
//
// Description:
//
// Search API to produce a linear list of documents.  Used to power doc.search
// and also a web version of the same logic.
//
// Author: Eliot Muir 
// Date:   Thursday 25 September 2025 - 01:40PM
// ---------------------------------------------------------------------------

#include <COL/COLlist.h>

class DOCdocument;
class COLvar;
class COLstring;
class COLwebRequest;

struct lua_State;

void DOCsearchLoad(const COLstring& ComponentId, COLlist<DOCdocument>* pList);
void DOCdocumentListCompileTagSearch(COLlist<DOCdocument>* pDocList);

void DOCsearch(const COLvar& Data, COLvar* pOut);

int DOCluaSearch(lua_State* L);
void DOCwebDocumentSearch(const COLwebRequest& Request);

#endif // end of defensive include
