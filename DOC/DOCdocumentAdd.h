#ifndef __DOC_DOCUMENT_ADD_H__
#define __DOC_DOCUMENT_ADD_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DOCdocumentAdd
//
// Description:
//
// TODO
//
// Author: Stephen Piccone 
// Date:   Friday 26 September 2025 - 12:06PM
// ---------------------------------------------------------------------------

class COLwebRequest;
class COLstring;
class COLvar;

void DOCdocumentAdd(const COLvar& Data, COLvar* pResult);

void DOCwebDocumentUpload(const COLwebRequest& Request);
#endif // end of defensive include
