#ifndef __DOC_INDEX_H__
#define __DOC_INDEX_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DOCindex
//
// Description:
//
// Routine to take a document and write out the index files.
//
// Author: Eliot Muir 
// Date:   Thursday 21 August 2025 - 08:49AM
// ---------------------------------------------------------------------------

class COLstring;
class DOCdocument;

void DOCindex(const DOCdocument& Doc, const COLstring& Id);

#endif // end of defensive include
