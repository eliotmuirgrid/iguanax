#ifndef __WEBI_INDEX_FILE_H__
#define __WEBI_INDEX_FILE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: WEBIindexFile
//
// Description:
//
// Helpers which I have exported to make tracing easier.
//
// Author: Eliot Muir 
// Date:   Tuesday 11 July 2023 - 02:31PM
// ---------------------------------------------------------------------------

class COLstring;

bool WEBIcssAnchorExists(COLstring* pData, int* pInsertPos);
COLstring WEBIcssInclude(const COLstring& CssFile);
void WEBIinclude(const COLstring& Include, COLstring* pData, int* pInsertPos);
bool WEBIjsAnchorExists(COLstring* pData, int* pInsertPos);
COLstring WEBIjsInclude(const COLstring& JsFile);
bool WEBIresolveFile(COLstring* pEntry);



#endif // end of defensive include
