#ifndef __DOC_DIR_H__
#define __DOC_DIR_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DOCdir
//
// Description:
//
// Routines to work out the paths to documents in our repository structure.
//
// Author: Eliot Muir 
// Date:   Tuesday 19 August 2025 - 12:08PM
// ---------------------------------------------------------------------------

class DOCdocument;
class COLstring;

COLstring DOCrepositoryDir(const COLstring& ComponentId);

COLstring DOCextension(const COLstring& DocSource);

// This will return /00/00/68/a3/
COLstring DOCbase(const DOCdocument& Doc);

// These functions are for doing resolution of a document via GUID
COLstring DOCfileGuid(const DOCdocument& Doc);

COLstring DOCfileBase          (const COLstring& Component, const DOCdocument& Doc);
COLstring DOCfileMeta          (const COLstring& Component, const DOCdocument& Doc);
COLstring DOCfileMd            (const COLstring& Component, const DOCdocument& Doc);

COLstring DOCfileOriginal      (const COLstring& Component, const DOCdocument& Doc);
COLstring DOCfileOriginalLegacy(const COLstring& Component, const DOCdocument& Doc);

#endif // end of defensive include
