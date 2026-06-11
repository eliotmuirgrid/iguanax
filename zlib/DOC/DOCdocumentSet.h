#ifndef __DOC_DOCUMENT_SET_H__
#define __DOC_DOCUMENT_SET_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DOCdocumentSet
//
// Description:
//
// Set a document - will only update the parts given.
//
// Author: Eliot Muir 
// Date:   Wednesday 17 September 2025 - 01:15PM
// ---------------------------------------------------------------------------

class COLvar;
class COLwebRequest;

void DOCdocumentSet(const COLvar& Data, COLvar* pResult);

void DOCwebDocumentSet(const COLwebRequest& Request);

#endif // end of defensive include
