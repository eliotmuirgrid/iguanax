#ifndef __DOC_GET_DOCUMENT_H__
#define __DOC_GET_DOCUMENT_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DOCgetDocument
//
// Description:
//
// Reads entire document including markdown
//
// Author: Stephen Piccone 
// Date:   Tuesday 02 September 2025 - 01:08PM
// ---------------------------------------------------------------------------

class COLwebRequest;
class COLstring;
class COLvar;

void DOCdocumentGet(const COLvar& Data, COLvar* pResult);
void DOCwebDocumentGet(const COLwebRequest& Request);

#endif // end of defensive include
