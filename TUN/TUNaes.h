#ifndef __TUN_AES_H__
#define __TUN_AES_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNaes
//
// Description:
//
// So with AES we need to:
// 1) Have the secret which we use with our AES encryptor
// 2) Pad the data using PKCS#7
// 3) Process all the 16 byte blocks
//
// Then reverse it for the other way.
//
// Author: Eliot Muir 
// Date:   Friday 16 June 2023 - 10:57AM
// ---------------------------------------------------------------------------

class SFIaesEncryptor;
class SFIaesDecryptor;
class COLstring;

void TUNaesEncrypt(const SFIaesEncryptor& AesEncryptor, COLstring* pData);

void TUNaesDecrypt(const SFIaesDecryptor& AesDecryptor, COLstring* pData);

#endif // end of defensive include
