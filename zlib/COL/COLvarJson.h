#ifndef __COL_VAR_JSON_H__
#define __COL_VAR_JSON_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: COLvarJson
//
// Description:
//
// This is a new parser for JSON.
//
// Author: Eliot Muir 
// Date:   Sunday 26 January 2025 - 02:13PM
// ---------------------------------------------------------------------------

class COLstring;
class COLvar;

bool COLparseJson(const char* pData, COLvar* pOutput);
bool COLparseJson(const COLstring& Data, COLvar* pOutput);

#endif // end of defensive include
