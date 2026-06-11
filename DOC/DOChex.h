#ifndef __DOC_HEX_H__
#define __DOC_HEX_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DOChex
//
// Description:
//
// HEX Conversion routines.
//
// Author: Eliot Muir 
// Date:   Tuesday 19 August 2025 - 11:52AM
// ---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>
#include <COL/COLweb.h>

class COLstring;

// TODO - come back to this interfaces.
COLuint64 DOChexStringToInt(const char* hexStr);
COLuint64 DOChexStringToInt(const COLstring& hexStr);

void DOCwebHexStringToInt(const COLwebRequest& R);


COLstring DOCtoHex(COLuint64 Number);


COLstring DOCpaddHex(const COLstring& Hex);

int DOChexCharToInt(char c);

#endif // end of defensive include
