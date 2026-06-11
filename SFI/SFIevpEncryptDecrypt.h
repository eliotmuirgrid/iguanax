#ifndef __SFI_EVP_H__
#define __SFI_EVP_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SFIevpEncryptDecrypt
//
// Description: 
//
//EVP Symmetric Encryption and Decryption
//
// https://wiki.openssl.org/index.php/EVP_Symmetric_Encryption_and_Decryption#C.2B.2B_Programs
// 
// Author: Shuo Hou
// Date:   Monday, March 28th, 2022
//
//---------------------------------------------------------------------------

#include <COL/COLstring.h>

void SFIevpEncrypt(const COLstring& PlainText, const COLstring& Key, const COLstring& IV, COLstring* pCipherText);
void SFIevpDecrypt(const COLstring& CipherText, const COLstring& Key, const COLstring& IV, COLstring* pPlainText);

#endif // end of defensive include
