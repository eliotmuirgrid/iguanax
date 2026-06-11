#ifndef __SDB_STRING_H__
#define __SDB_STRING_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBstring
//
// Description:
//
// Helper routines for writing and reading strings.
//
// Author: Eliot Muir
// Date:   Thursday 23 February 2023 - 01:19PM
// ---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>
#include <COL/COLstring.h>

// WARNING - these routines are not coded with safety currently in mind.
int SDBwriteString(const COLstring& In, char* pBuffer);

int SDBreadString(COLstring* pValue, const char* pBuffer, int BufSize);

int SDBstringSize(const COLstring& String);

#endif // end of defensive include
