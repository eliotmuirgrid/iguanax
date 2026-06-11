#ifndef __LCR_API_H__
#define __LCR_API_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2016 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LCRapi
//
// Description:
//
// Cryptographic digest, sign, and verify functions for the translator
//
// Author:  Akshay Ganeshen, Jonathan Marshall
// Date:    Tue Jan 26 18:35:47 EST 2016
//---------------------------------------------------------------------------

#include <LUA/LUAheader.h>

class COLstring;

extern const COLstring transLK4;
extern const COLstring transIVB2;
extern const COLstring transKB4;
extern const COLstring transBK3;  

void LCRaddCryptoNamespace(lua_State* L);

// TODO redo unit tests in context of an LCRexample
// Exposed for unit testing:
//int TNDLaesEncrypt(lua_State* L);
//int TNDLaesDecrypt(lua_State* L);

#endif // end of defensive include
