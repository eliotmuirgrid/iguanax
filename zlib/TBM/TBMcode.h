#ifndef __TBM_CODE_H__
#define __TBM_CODE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2007 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TBMcode
//
// Description:
//
// TBMcode class.
//
// Author: Eliot Muir
// Date:   Friday, February 9th, 2007 @ 07:42:19 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

const char TBMobject = 0;
const char TBMendObject = 1;
const char TBMbooleanFalse = 2;
const char TBMbooleanTrue = 3;
const char TBMuint16OneByte = 4;
const char TBMuint16TwoByte = 5;
const char TBMuint32OneByte = 6;
const char TBMuint32TwoByte = 7;
const char TBMuint32FourByte = 8;
const char TBMstringShort = 9;  // < 256 bytes
const char TBMstringMedium = 10;  // <= 0xFFFF bytes
const char TBMstringLong  = 11;  // < 0xFFFFFFF bytes


//const char TBMuint32ThreeByte = 9;




#endif // end of defensive include
