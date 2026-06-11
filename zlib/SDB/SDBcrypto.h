#ifndef __SDB_CRYPTO_H__
#define __SDB_CRYPTO_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBcrypto
//
// Description:
//
// Encrypt and decrypt functions for logs
//
// Author: John Q
// Date:   Mon Oct 28 14:14:58 EDT 2024
// ---------------------------------------------------------------------------
#include <COL/COLstring.h>

#define SDB_AES_GCM_SIZE_IV   12       // bytes
#define SDB_AES_GCM_SIZE_TAG  16       // bytes
#define SDB_AES_GCM_SIZE_KEY  32       // bytes
#define SDB_PBKDF2_SIZE_SALT  16       // bytes
#define SDB_PBKDF2_ITERATION  100000


// Create AES GCM encryption context using the provided encryption <Key> and <IV>
// If <IV> is an empty, it will be automatically generated and populated.
// EVP_CIPHER_CTX* SDBencryptInitialize(const COLstring& Key, COLstring& IV);

// Encrypt a block of <Plaintext> data using the provided AES GCM <pContext>
// void SDBencryptData(EVP_CIPHER_CTX* pContext, const COLstring& Plaintext, COLstring* pCiphertext);

// Finalize AES GCM encryption and get the authentication <pTag> (16B)
// void SDBencryptFinalize(EVP_CIPHER_CTX* pContext, COLstring* pTag);


bool SDBencrypt(const COLstring& Plaintext, const COLstring& Key,
                COLstring& IV,
                COLstring* pCiphertext, COLstring* pTag,
                const COLstring& AAD = "");

bool SDBdecrypt(const COLstring& Ciphertext, const COLstring& Tag, const COLstring& IV, const COLstring& Key,
                COLstring* pPlaintext,
                const COLstring& AAD = "");

#endif // end of defensive include
