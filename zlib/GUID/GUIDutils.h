#ifndef __GUID_UTILS_H__
#define __GUID_UTILS_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2017 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GUIDutils
//
// Description:
//
// Utilities useful for working with the new GUID library
//
// Author:  Akshay Ganeshen
// Date:    Tue Nov 17 10:55:33 EST 2015
//---------------------------------------------------------------------------

#include <COL/COLstring.h>

// Returns an MD5 string with a good amount of entropy in it
// The bytes are not encoded in any way
COLstring GUIDentropyString();

// Encodes a raw byte into a printable character
// Byte must be within the [0, 62) range
char GUIDbase62ValueToCharacter(char Value);

// Encodes a sequence of raw bytes into a base 62 (printable) string
// NOTE : Any bits at the end of the buffer (i.e. modulo 62) are discarded
COLstring GUIDencodeBase62(const COLstring& RawBytes);

#endif // end of defensive include
