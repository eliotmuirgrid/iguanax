#ifndef __WEB_INDEX_H__
#define __WEB_INDEX_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: WEBindex
//
// Description:
//
// Routines to gather all the CSS and Javascript files for the index
//
// Author: Eliot Muir 
// Date:   Sunday 21 May 2023 - 06:02PM
// ---------------------------------------------------------------------------

class COLstring;

bool WEBIisExternal(const COLstring& JsFile);
void WEBIindexGetCssAndJs(const COLstring& WebDir, COLstring* pData);
void WEBIindexPack(const COLstring& JsFile, const COLstring& CssFile, COLstring* pData);

#endif // end of defensive include
