#ifndef __SDB_NUMBER_H__
#define __SDB_NUMBER_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBnumber
//
// Description:
//
// Routine to format number
//
// Author: Eliot Muir
// Date:   Thursday 16 February 2023 - 04:47PM
// ---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

class COLstring;

// returns the number of bytes (between 1 and 10) needed to store the serialized Value
int SDBnumberSize(COLuint64 Value);
// caller's responsibility to ensure pBuffer is big enough
int SDBwriteNumber(COLuint64 Value, char* pBuffer);
// returns -1 on failure
int SDBreadNumber(COLuint64* pValue, const char* pBuffer, int BufSize);

#endif // end of defensive include
