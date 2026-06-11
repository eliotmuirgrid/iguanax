#ifndef __DOC_TAG_H__
#define __DOC_TAG_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DOCtag
//
// Description:
//
// Functions to translate tags into a directory locations.
//
// Author: Eliot Muir 
// Date:   Wednesday 20 August 2025 - 12:21PM
// ---------------------------------------------------------------------------

class COLstring;
// Takes tag without # in front of it.
COLstring DOCtagDir(const COLstring& Tag, const COLstring& Id);

COLstring DOCtagIndexDir(const COLstring& ComponentId);

#endif // end of defensive include
