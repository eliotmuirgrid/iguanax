#ifndef __TUN_NUMBER_H__
#define __TUN_NUMBER_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNnumber
//
// Description:
//
// Routine to write 32 bit number as 1-5 bytes.  Uses a format similar to varint encoding
// in Google protocols.
//
// Author: Eliot Muir 
// Date:   Wednesday 14 June 2023 - 08:33AM
// ---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

class COLstring;

int TUNencodeNumber(COLuint32 Value, char* pStart);
int TUNdecodeNumber(COLuint32* pValue, const unsigned char* pStart);

void TUNnumberWrite(COLstring* pOut, COLuint32 Value);

#endif // end of defensive include
