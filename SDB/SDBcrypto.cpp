// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBcrypto
//
// Description:
//
// Implementation
//
// Author: John Q
// Date:   Mon Oct 28 14:14:58 EDT 2024
// ---------------------------------------------------------------------------
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

#include <SDB/SDBcrypto.h>

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

static void SDBgetErrors(COLstring* pError) {
   COL_FUNCTION(SDBgetErrors);
   ERR_print_errors_cb(&SDBcollectError, pError);
   COL_ERR(*pError);
}

static bool SDBgenerateIV(COLstring* pIV, COLstring* pError = nullptr) {
   COL_FUNCTION(SDBgenerateIV);
   pIV->setCapacity(SDB_AES_GCM_SIZE_IV);
   pIV->setSize(SDB_AES_GCM_SIZE_IV);
   if (RAND_bytes((unsigned char*) pIV->get_buffer(), pIV->size()) != 1 ) {
      if (pError) { SDBgetErrors(pError); }
      return false;
   }
   return true;
}

// EVP_CIPHER_CTX* SDBencryptInitialize(const COLstring& Key, COLstring& IV) {
//    COL_FUNCTION(SDBencryptInitialize);
//    if (IV.is_null() && !SDBgenerateIV(&IV)) {
//       return nullptr;
//    }
//    if (SDB_AES_GCM_SIZE_IV != IV.size()) {
//       return nullptr;
//    }
//    EVP_CIPHER_CTX* Ctx = nullptr;
//    do {
//       if (!(Ctx = EVP_CIPHER_CTX_new())) { break; }
//       if (EVP_EncryptInit_ex(Ctx, EVP_aes_256_gcm(), nullptr, nullptr, nullptr) != 1) { break; }
//       if (EVP_CIPHER_CTX_ctrl(Ctx, EVP_CTRL_GCM_SET_IVLEN, SDB_AES_GCM_SIZE_IV, nullptr) != 1) { break; }
//       if (EVP_EncryptInit_ex(Ctx, nullptr, nullptr, (const unsigned char*) Key.data(), (const unsigned char*) IV.data()) != 1) { break; }
//    } while (false);
//    SDBgetErrors();
// }

// void SDBencryptData(EVP_CIPHER_CTX* pContext, const COLstring& Plaintext, COLstring* pCiphertext) {
//    COL_FUNCTION(SDBencryptData);
//    int Length = 0;
//    int NewCiphertextSize = pCiphertext->size() + Plaintext.size();
//    pCiphertext->setCapacity(NewCiphertextSize);
//    unsigned char* pBuffer = (unsigned char*) pCiphertext->get_buffer() + pCiphertext->size();
//    if (EVP_EncryptUpdate(pContext, pBuffer, &Length, (const unsigned char*) Plaintext.data(), Plaintext.size()) != 1) {
//       SDBgetErrors();
//    } else {
//       COLASSERT(Plaintext.size() == Length);
//       pCiphertext->setSize(NewCiphertextSize);
//    }
// }

// void SDBencryptFinalize(EVP_CIPHER_CTX* pContext, COLstring* pTag) {
//    COL_FUNCTION(SDBencryptFinalize);
//    int Length = 0;
//    COLstring Ciphertext;
//    do {
//       if (EVP_EncryptFinal_ex(pContext, (unsigned char*) Ciphertext.get_buffer(), &Length) != 1) { break; }
//       COLASSERT(0 == Length);
//       pTag->setCapacity(SDB_AES_GCM_SIZE_TAG);
//       if (EVP_CIPHER_CTX_ctrl(pContext, EVP_CTRL_GCM_GET_TAG, SDB_AES_GCM_SIZE_TAG, (void*) pTag->get_buffer()) != 1) { break; }
//       pTag->setSize(SDB_AES_GCM_SIZE_TAG);
//       EVP_CIPHER_CTX_free(pContext);
//       return;
//    } while (false);
//    SDBgetErrors();
// }


// Input    - the <Plaintext> to be encrypted; the encryption <Key>
// InOut    - the encryption <IV> (12B) [auto-generated if empty]
// Output   - encrypted <pCiphertext>; authentication <pTag> (16B)
// Optional - Additional Authenticated Data (<AAD>)
bool SDBencrypt(const COLstring& Plaintext, const COLstring& Key,
                COLstring& IV,
                COLstring* pCiphertext, COLstring* pTag,
                const COLstring& AAD) {
   COL_FUNCTION(SDBencrypt);
   COL_DUMP("plain", Plaintext.data(), Plaintext.size());
   if (IV.is_null() && !SDBgenerateIV(&IV)) {
      COL_ERR("Failed to generate a random IV.");
      return false;
   }
   EVP_CIPHER_CTX* Ctx = nullptr;
   do {
      if (!(Ctx = EVP_CIPHER_CTX_new())) { break; }
      if (EVP_EncryptInit_ex(Ctx, EVP_aes_256_gcm(), nullptr, nullptr, nullptr) != 1) { break; }
      if (EVP_CIPHER_CTX_ctrl(Ctx, EVP_CTRL_GCM_SET_IVLEN, IV.size(), nullptr) != 1) { break; }
      if (EVP_EncryptInit_ex(Ctx, nullptr, nullptr, (const unsigned char*) Key.data(), (const unsigned char*) IV.data()) != 1) { break; }
      int Length = 0;
      if (!AAD.is_null() && EVP_EncryptUpdate(Ctx, nullptr, &Length, (const unsigned char*) AAD.data(), AAD.size()) != 1) { break; }
      pCiphertext->setCapacity(Plaintext.size());
      pCiphertext->setSize(Plaintext.size());
      if (EVP_EncryptUpdate(Ctx, (unsigned char*) pCiphertext->get_buffer(), &Length, (const unsigned char*) Plaintext.data(), Plaintext.size()) != 1) { break; }
      COLASSERT(Plaintext.size() == Length);
      if (EVP_EncryptFinal_ex(Ctx, (unsigned char*) pCiphertext->get_buffer(), &Length) != 1) { break; }
      COLASSERT(0 == Length);
      pTag->setCapacity(SDB_AES_GCM_SIZE_TAG);
      pTag->setSize(SDB_AES_GCM_SIZE_TAG);
      if (EVP_CIPHER_CTX_ctrl(Ctx, EVP_CTRL_GCM_GET_TAG, SDB_AES_GCM_SIZE_TAG, (void*) pTag->get_buffer()) != 1) { break; }
      EVP_CIPHER_CTX_free(Ctx);
      // COL_DUMP("key", Key.data(), Key.size());
      COL_DUMP("iv", IV.data(), IV.size());
      COL_DUMP("cipher", pCiphertext->data(), pCiphertext->size());
      COL_DUMP("tag", pTag->data(), pTag->size());
      return true;
   } while (false);
   COL_ERR("failed to encrypt data");
   if (Ctx) { EVP_CIPHER_CTX_free(Ctx); }
   return false;
}

// Input    - encrypted <Ciphertext>; authentication <Tag> (16B); the encryption <IV> (12B); the encryption <Key>
// Output   - decrypted <pPlaintext>
// Optional - Additional Authenticated Data (<AAD>)
bool SDBdecrypt(const COLstring& Ciphertext, const COLstring& Tag, const COLstring& IV, const COLstring& Key,
                COLstring* pPlaintext,
                const COLstring& AAD) {
   COL_FUNCTION(SDBdecrypt);
   // COL_TRC_DUMP("key", Key.data(), Key.size());
   COL_DUMP("iv", IV.data(), IV.size());
   COL_DUMP("cipher", Ciphertext.data(), Ciphertext.size());
   COL_DUMP("tag", Tag.data(), Tag.size());
   EVP_CIPHER_CTX* Ctx = nullptr;
   do {
      if (!(Ctx = EVP_CIPHER_CTX_new())) { break; }
      if (!EVP_DecryptInit_ex(Ctx, EVP_aes_256_gcm(), nullptr, nullptr, nullptr)) { break; }
      if (!EVP_CIPHER_CTX_ctrl(Ctx, EVP_CTRL_GCM_SET_IVLEN, IV.size(), nullptr)) { break; }
      if (!EVP_DecryptInit_ex(Ctx, nullptr, nullptr, (const unsigned char*) Key.data(), (const unsigned char*) IV.data())) { break; }
      int Length = 0;
      if (!AAD.is_null() && !EVP_DecryptUpdate(Ctx, nullptr, &Length, (const unsigned char*) AAD.data(), AAD.size())) { break; }
      pPlaintext->setCapacity(Ciphertext.size());
      pPlaintext->setSize(Ciphertext.size());
      if (!EVP_DecryptUpdate(Ctx, (unsigned char*) pPlaintext->get_buffer(), &Length, (const unsigned char*) Ciphertext.data(), Ciphertext.size())) { break; }
      COLASSERT(Ciphertext.size() == Length);
      if (!EVP_CIPHER_CTX_ctrl(Ctx, EVP_CTRL_GCM_SET_TAG, Tag.size(), (void*) Tag.data())) { break; }
      if (EVP_DecryptFinal_ex(Ctx, (unsigned char*) pPlaintext->get_buffer(), &Length) != 1) { break; }
      COLASSERT(0 == Length);
      EVP_CIPHER_CTX_free(Ctx);
      COL_DUMP("plain", pPlaintext->data(), pPlaintext->size());
      return true;
   } while (false);
   COL_ERR("failed to decrypt data");
   if (Ctx) { EVP_CIPHER_CTX_free(Ctx); }
   return false;
}
