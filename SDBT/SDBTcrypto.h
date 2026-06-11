#ifndef __SDBT_CRYPTO_H__
#define __SDBT_CRYPTO_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBTcrypto
//
// Description:
//
// Encrypt and decrypt functions for logs
//
// Author: John Q
// Date:   Tue Oct  1 10:36:34 EDT 2024
// ---------------------------------------------------------------------------
#include <COL/COLstring.h>

#define SDB_AES_GCM_SIZE_IV   12       // bytes
#define SDB_AES_GCM_SIZE_TAG  16       // bytes
#define SDB_AES_GCM_SIZE_KEY  32       // bytes
#define SDB_PBKDF2_SIZE_SALT  16       // bytes
#define SDB_PBKDF2_ITERATION  100000

// Output   - generated <pIV> (12B)
bool SDBgenerateIV(COLstring* pIV);

// Input    - a key derivation key (<KeyDerivationKey>); the <Salt>;
// Output   - the encryption key <pEncryptionKey>
// Optional - number of <Iterations> for PBKDF2
bool SDBderiveEncryptionKey(const COLstring& KeyDerivationKey, const COLstring& Salt,
                            COLstring* pEncryptionKey,
                            int Iterations = SDB_PBKDF2_ITERATION);

// Input    - the <Plaintext> to be encrypted; the encryption <IV> (12B); the encryption <Key>
// Output   - encrypted <pCiphertext>; authentication <pTag> (16B)
// Optional - Additional Authenticated Data (<AAD>)
bool SDBTencrypt(const COLstring& Plaintext, const COLstring& IV, const COLstring& Key,
                COLstring* pCiphertext, COLstring* pTag,
                const COLstring& AAD = "");

// Input    - encrypted <Ciphertext>; authentication <Tag> (16B); the encryption <IV> (12B); the encryption <Key>
// Output   - decrypted <pPlaintext>
// Optional - Additional Authenticated Data (<AAD>)
bool SDBTdecrypt(const COLstring& Ciphertext, const COLstring& Tag, const COLstring& IV, const COLstring& Key,
                COLstring* pPlaintext,
                const COLstring& AAD = "");

// Same as SDBencrypt and SDBdecrypt but with <Password> and <Salt> instead of <Key>. <IV> is auto-generated.
bool SDBpasswordEncrypt(const COLstring& Plaintext, const COLstring& Password, const COLstring& Salt,
                        COLstring* pCiphertext, COLstring* pTag, COLstring* pIV);
bool SDBpasswordDecrypt(const COLstring& Ciphertext, const COLstring& Tag, const COLstring& IV, const COLstring& Password, const COLstring& Salt,
                        COLstring* pPlaintext);

#endif // end of defensive include
