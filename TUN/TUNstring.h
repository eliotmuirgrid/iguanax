#ifndef __TUN_STRING_H__
#define __TUN_STRING_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNstring
//
// Description:
//
// Routines to read and write strings
//
// Author: Eliot Muir 
// Date:   Wednesday 14 June 2023 - 03:59PM
// ---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

class COLstring;

void TUNstringWrite(COLstring* pOutBuffer, const COLstring& String);

class TUNcursor{
public:
   TUNcursor(const COLstring& String);
   const unsigned char* pBuffer;
   int            Size;
};

bool TUNstringRead(TUNcursor* pCursor, COLstring* pString);
bool TUNnumberRead(TUNcursor* pCursor, COLuint32* pNumber);

#endif // end of defensive include
