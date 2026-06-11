// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBTcrypto
//
// Description:
//
// Implementation
//
// Author: John Q
// Date:   Tue Oct  1 10:36:34 EDT 2024
// ---------------------------------------------------------------------------

#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

#include <SDBT/SDBTcrypto.h>

#include <COL/COLerror.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;


static int SDBcollectError(const char *str, size_t len, void *u) {
   COL_FUNCTION(SDBcollectError);
   COLstring* pError = static_cast<COLstring*>(u);
   pError->append("\n");
   pError->append(str, len);
   return 1;
}

static void SDBgetErrors() {
   COL_FUNCTION(SDBgetErrors);
   COLstring Error;
   ERR_print_errors_cb(&SDBcollectError, &Error);
   COL_ERR(Error);
}

bool SDBgenerateIV(COLstring* pIV) {
   COL_FUNCTION(SDBgenerateIV);
   pIV->setCapacity(SDB_AES_GCM_SIZE_IV);
   pIV->setSize(SDB_AES_GCM_SIZE_IV);
   if (RAND_bytes((unsigned char*) pIV->get_buffer(), pIV->size()) != 1 ) {
      SDBgetErrors();
      return false;
   }
   return true;
}

bool SDBderiveEncryptionKey(const COLstring& SourceKey, const COLstring& Salt,
                            COLstring* pEncryptionKey,
                            int Iterations) {
   COL_FUNCTION(SDBderiveEncryptionKey);
   pEncryptionKey->setCapacity(SDB_AES_GCM_SIZE_KEY);
   pEncryptionKey->setSize(SDB_AES_GCM_SIZE_KEY);
   if (!PKCS5_PBKDF2_HMAC(SourceKey.data(), SourceKey.size(), (const unsigned char*) Salt.data(), Salt.size(), SDB_PBKDF2_ITERATION, EVP_sha256(), SDB_AES_GCM_SIZE_KEY, (unsigned char*) pEncryptionKey->get_buffer())) {
      COL_ERR("Encryption key derivation failed");
      return false;
   }
   return true;
}

bool SDBTencrypt(const COLstring& Plaintext, const COLstring& IV, const COLstring& Key,
                COLstring* pCiphertext, COLstring* pTag,
                const COLstring& AAD) {
   COL_FUNCTION(SDBTencrypt);
   if (SDB_AES_GCM_SIZE_IV != IV.size()) { return false; }
   do {
      int Length = 0;
      EVP_CIPHER_CTX* Ctx = nullptr;
      if (!(Ctx = EVP_CIPHER_CTX_new())) { break; }

      if (EVP_EncryptInit_ex(Ctx, EVP_aes_256_gcm(), nullptr, nullptr, nullptr) != 1) { break; }

      if (EVP_CIPHER_CTX_ctrl(Ctx, EVP_CTRL_GCM_SET_IVLEN, SDB_AES_GCM_SIZE_IV, nullptr) != 1) { break; }

      if (EVP_EncryptInit_ex(Ctx, nullptr, nullptr, (const unsigned char*) Key.data(), (const unsigned char*) IV.data()) != 1) { break; }

      if (!AAD.is_null() && EVP_EncryptUpdate(Ctx, nullptr, &Length, (const unsigned char*) AAD.data(), AAD.size()) != 1) { break; }

      pCiphertext->setCapacity(Plaintext.size());
      if (EVP_EncryptUpdate(Ctx, (unsigned char*) pCiphertext->get_buffer(), &Length, (const unsigned char*) Plaintext.data(), Plaintext.size()) != 1) { break; }

      int AmountWritten = Length;
      if (EVP_EncryptFinal_ex(Ctx, (unsigned char*) pCiphertext->get_buffer() + AmountWritten, &Length) != 1) { break; }
      AmountWritten += Length;

      COLASSERT(Plaintext.size() == AmountWritten + Length);
      pCiphertext->setSize(AmountWritten);

      pTag->setCapacity(SDB_AES_GCM_SIZE_TAG);
      if (EVP_CIPHER_CTX_ctrl(Ctx, EVP_CTRL_GCM_GET_TAG, SDB_AES_GCM_SIZE_TAG, (void*) pTag->get_buffer()) != 1) { break; }
      pTag->setSize(SDB_AES_GCM_SIZE_TAG);

      EVP_CIPHER_CTX_free(Ctx);
      return true;
   } while (false);

   SDBgetErrors();
   return false;
}

bool SDBTdecrypt(const COLstring& Ciphertext, const COLstring& Tag, const COLstring& IV, const COLstring& Key,
                COLstring* pPlaintext,
                const COLstring& AAD) {
   COL_FUNCTION(SDBTdecrypt);
   if (SDB_AES_GCM_SIZE_TAG != Tag.size()) { return false; }
   if (SDB_AES_GCM_SIZE_IV  != IV.size() ) { return false; }
   EVP_CIPHER_CTX* Ctx = nullptr;
   do {
      int Length = 0;
      if (!(Ctx = EVP_CIPHER_CTX_new())) { break; }

      if (!EVP_DecryptInit_ex(Ctx, EVP_aes_256_gcm(), nullptr, nullptr, nullptr)) { break; }

      if (!EVP_CIPHER_CTX_ctrl(Ctx, EVP_CTRL_GCM_SET_IVLEN, SDB_AES_GCM_SIZE_IV, nullptr)) { break; }

      if (!EVP_DecryptInit_ex(Ctx, nullptr, nullptr, (const unsigned char*) Key.data(), (const unsigned char*) IV.data())) { break; }

      if (!AAD.is_null() && !EVP_DecryptUpdate(Ctx, nullptr, &Length, (const unsigned char*) AAD.data(), AAD.size())) { break; }

      pPlaintext->setCapacity(Ciphertext.size());
      if (!EVP_DecryptUpdate(Ctx, (unsigned char*) pPlaintext->get_buffer(), &Length, (const unsigned char*) Ciphertext.data(), Ciphertext.size())) { break; }

      if (!EVP_CIPHER_CTX_ctrl(Ctx, EVP_CTRL_GCM_SET_TAG, SDB_AES_GCM_SIZE_TAG, (void*) Tag.data())) { break; }

      int AmountWritten = Length;
      if (EVP_DecryptFinal_ex(Ctx, (unsigned char*) pPlaintext->get_buffer() + Length, &Length) != 1) { break; }
      AmountWritten += Length;

      COLASSERT(Ciphertext.size() == AmountWritten + Length);
      pPlaintext->setSize(AmountWritten);

      EVP_CIPHER_CTX_free(Ctx);
      return true;
   } while (false);

   SDBgetErrors();
   EVP_CIPHER_CTX_free(Ctx);
   return false;
}


bool SDBpasswordEncrypt(const COLstring& Plaintext, const COLstring& Password, const COLstring& Salt,
                        COLstring* pCiphertext, COLstring* pTag, COLstring* pIV) {
   COL_FUNCTION(SDBpasswordEncrypt);
   COLstring Key;
   if (!SDBderiveEncryptionKey(Password, Salt, &Key)) { return false; }
   SDBgenerateIV(pIV);
   return SDBTencrypt(Plaintext, *pIV, Key, pCiphertext, pTag);
}

bool SDBpasswordDecrypt(const COLstring& Ciphertext, const COLstring& Tag, const COLstring& IV, const COLstring& Password, const COLstring& Salt,
                        COLstring* pPlaintext) {
   COL_FUNCTION(SDBpasswordDecrypt);
   COLstring Key;
   if (!SDBderiveEncryptionKey(Password, Salt, &Key)) { return false; }
   return SDBTdecrypt(Ciphertext, Tag, IV, Key, pPlaintext);
}
