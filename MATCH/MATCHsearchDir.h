#ifndef __MATCH_SEARCH_DIR_H__
#define __MATCH_SEARCH_DIR_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: MATCHsearchDir
//
// Description:
//
// TODO
//
// Author: Tyler Brown 
// Date:   Tuesday 07 October 2025 - 08:54AM
// ---------------------------------------------------------------------------
class COLstring;
class COLvar;

bool MATCHsearchDir( const COLstring& Query, const COLstring& Dir, bool ExactMatch, COLvar* pResults);

#endif // end of defensive include
