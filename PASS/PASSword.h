#ifndef __PASS_WORD_H__
#define __PASS_WORD_H__
//-----------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module:  PASSword 
//
// Description
//
// Arbitrary cipher routine to decrypt and encrpyt password in readable format.
//
// Author: Shuo Hou
//-----------------------------------------------------------------------------

class COLstring;

void PASSsetKeyIV(const COLstring& Key, const COLstring& IV); // set these before calling helper functions
// Check for false if it fails, pResult will have an error message.
bool PASSencrypt(const COLstring& Password, COLstring* pResult);
bool PASSdecrypt(const COLstring& Cipher, COLstring* pResult);

bool PASSdecryptCustom(const COLstring& Cipher, COLstring* pResult);
bool PASSencryptCustom(const COLstring& Content, COLstring* pResult);

#endif
