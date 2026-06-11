//---------------------------------------------------------------------------
// Copyright (C) 1997-2016 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LCRapi
//
// Description:
//
// Implementation
//
// Author:  Akshay Ganeshen, Jonathan Marshall
// Date:    Tue Jan 26 18:35:47 EST 2016
//---------------------------------------------------------------------------
#include "LCRapi.h"

#include <LUAL/LUALutils.h>

#include <LUA/LUAerror.h>
#include <LUA/LUAtable.h>
#include <LUA/LUAutils.h>
#include <LUA/LUAvar.h>

#include <LUA/LUAcheck.h>

#include <openssl/pem.h>
#include <openssl/hmac.h>
#include <openssl/err.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/bn.h>

#include <COL/COLstring.h>
#include <COL/COLvar.h>
#include <COL/COLerror.h>
#include <COL/COLvarUtils.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

//t30588 - split up portion of encryption keys
//Check TNDLdecrpytionContext.cpp for more details
const COLstring transLK4 = "592DB136";
const COLstring transIVB2 = "5A452BEB";
const COLstring transKB4 = "FA108742";
const COLstring transBK3 = "B54CCBE7";

// Helper that grabs the "data" parameter from the table on the stack, and ensures that it
// is either a string or a stream (to apply a filter on it)
// Returns the index of the "data" on the stack
static int LCRcryptoPushData(lua_State* L, int i) {
   COL_FUNCTION(LCRcryptoPushData);
   lua_getfield(L, i, "data");
   int DataIndex = lua_gettop(L);
   COL_VAR(DataIndex);
   if (lua_isnil(L, DataIndex)) {
      LUA_ERROR_STREAM("Parameter 'data' is required");
   } else if (lua_type(L, DataIndex) == LUA_TNUMBER ||
              !(lua_isstring(L, DataIndex) || lua_isfunction(L, DataIndex))) {
      // The order of the above test is important. Both lua_isstring and lua_isnumber
      // return true for number that look like strings and strings that look like numbers.
      // We need to first make sure that it's not a number, then if it's a string or a function.
      LUA_ERROR_STREAM("Parameter 'data' should be of type string or stream");
   }
   return DataIndex;
}

int LCRdigest(lua_State* L) {
   COL_FUNCTION(LCRdigest);
   LUAcheckArgCount(L, 1, -1);
   LUAparamIsTable(L, 1);
   static const char* s_DigestArgs[] = { "algorithm", "data", NULL };
   LUAcheckParams(L, 1, s_DigestArgs);

   COLstring Algorithm = LUAexpectedStringParam(L, 1, "algorithm");
   int DataIndex = LCRcryptoPushData(L, 1);

   EVP_MD_CTX *pContext = EVP_MD_CTX_new();
	EVP_DigestInit_ex(pContext, EVP_get_digestbyname(Algorithm.c_str()), NULL);

   size_t DataSize;
   const unsigned char* Data = (const unsigned char*) lua_tolstring(L, DataIndex, &DataSize);
	EVP_DigestUpdate(pContext, Data, DataSize);

   unsigned char* DigestBuff;
   size_t Size = EVP_MD_size(EVP_get_digestbyname(Algorithm.c_str()));
	DigestBuff = (unsigned char*) OPENSSL_malloc(Size);

   unsigned int DigestSize;
	EVP_DigestFinal_ex(pContext, DigestBuff, &DigestSize);
	EVP_MD_CTX_free(pContext);

   COLstring Digested = COLstring((char *) DigestBuff, DigestSize);
   free(DigestBuff);

   lua_remove(L, DataIndex);
   LUApushString(L, Digested);
   return 1;
}

int LCRhmac(lua_State* L) {
   COL_FUNCTION(LCRhmac);
   LUAcheckArgCount(L, 1, -1);
   LUAparamIsTable(L, 1);
   static const char* s_HmacArgs[] = { "key", "algorithm", "data", NULL };
   LUAcheckParams(L, 1, s_HmacArgs);

   COLstring Algorithm = LUAexpectedStringParam(L, 1, "algorithm");
   COLstring SharedKey = LUAexpectedStringParam(L, 1, "key");
   int DataIndex = LCRcryptoPushData(L, 1);
   size_t DataSize;
   const unsigned char* Data = (const unsigned char*) lua_tolstring(L, DataIndex, &DataSize);

   void* MessageDigest = OPENSSL_malloc(EVP_MAX_MD_SIZE);
   unsigned int MessageDigestActualSize;

   unsigned char* MAC = HMAC(EVP_get_digestbyname(Algorithm.c_str()), SharedKey.get_buffer(),
                     SharedKey.size(), Data, DataSize,
                     (unsigned char*) MessageDigest, &MessageDigestActualSize);

   COLstring Result = COLstring((char *) MessageDigest, MessageDigestActualSize);
   OPENSSL_free(MessageDigest);

   if (NULL == MAC) {
      LUA_ERROR_STREAM("Could not create Message Digest" << ERR_get_error());
      lua_remove(L, DataIndex);
      return 0;
   } else {
      lua_remove(L, DataIndex);
      LUApushString(L, Result);
      return 1;
   }
}

int LCRdigitalSign(lua_State* L) {
   COL_FUNCTION(LCRdigitalSign);

   LUAcheckArgCount(L, 1, -1);
   LUAparamIsTable(L, 1);

   static const char* s_SignArgs[] = { "algorithm", "data", "key", "keyform", NULL };
   LUAcheckParams(L, 1, s_SignArgs);

   COLstring DigestFunction   =  LUAexpectedStringParam(L, 1, "algorithm");
   COLstring PrivateKey       =  LUAexpectedStringParam(L, 1, "key");
   COLstring PrivateKeyFormat =  LUAoptionalStringParam(L, 1, "keyform", "PEM");
   int       DataIndex        =  LCRcryptoPushData(L, 1);

   BIO *pBIO = BIO_new_mem_buf(PrivateKey.c_str(), PrivateKey.size());

   EVP_PKEY *pPrivateKey = PEM_read_bio_PrivateKey(pBIO, NULL, NULL, 0);
   BIO_free(pBIO);

   EVP_MD_CTX* pContext = NULL;
   const EVP_MD* Digest = EVP_get_digestbyname(DigestFunction.c_str());
   // Looks like the memory managment of Digest is taken care of by context

   pContext = EVP_MD_CTX_create();
   if (!pContext) {
      LUA_ERROR_STREAM("Failed to create Context: " << ERR_get_error());
      EVP_MD_CTX_free(pContext);
      EVP_PKEY_free(pPrivateKey);
      return 0;
   }

   if(1 != EVP_DigestSignInit(pContext, NULL, Digest, NULL, pPrivateKey)) {
      LUA_ERROR_STREAM("Failed to initailize digest signing: " << ERR_get_error());
      EVP_MD_CTX_free(pContext);
      EVP_PKEY_free(pPrivateKey);
      return 0;
   }

   size_t DataSize;
   const char* Data = lua_tolstring(L, DataIndex, &DataSize);
   if (1 != EVP_DigestSignUpdate(pContext, Data, DataSize)) {
      LUA_ERROR_STREAM("Failed to Update digest: " << ERR_get_error());
      EVP_MD_CTX_free(pContext);
      EVP_PKEY_free(pPrivateKey);
      return 0;
   }

   size_t Size;
   if (1 != EVP_DigestSignFinal(pContext, NULL, &Size)) {
      LUA_ERROR_STREAM("Failed to finalize digest: " << ERR_get_error());
      EVP_MD_CTX_free(pContext);
      EVP_PKEY_free(pPrivateKey);
      return 0;
   }

   void* Value = OPENSSL_malloc(Size);
   if (NULL == Value) {
      LUA_ERROR_STREAM("Failed to allocate memory.");
      OPENSSL_free(Value);
      EVP_MD_CTX_free(pContext);
      EVP_PKEY_free(pPrivateKey);
      return 0;
   }

   if (1 != EVP_DigestSignFinal(pContext, (unsigned char*) Value, &Size)) {
      LUA_ERROR_STREAM("Failed to finalize digest: " << ERR_get_error());
      OPENSSL_free(Value);
      EVP_MD_CTX_free(pContext);
      EVP_PKEY_free(pPrivateKey);
      return 0;
   }

   COLstring Signature = COLstring((char*) Value, Size);
   OPENSSL_free(Value);
   EVP_MD_CTX_free(pContext);
   EVP_PKEY_free(pPrivateKey);

   COL_TRC("Successfully signed input data! Pushing result out and returning");
   LUApushString(L, Signature);
   return 1;
}

int LCRdigitalVerify(lua_State* L) {
   COL_FUNCTION(LCRdigitalVerify);

   LUAcheckArgCount(L, 1, -1);
   LUAparamIsTable(L, 1);
   static const char* s_VerifyArgs[] = { "algorithm", "data", "key", "keyform", "signature", NULL };
   LUAcheckParams(L, 1, s_VerifyArgs);

   COLstring DigestFunction   =  LUAexpectedStringParam(L, 1, "algorithm");
   COLstring PublicKey        =  LUAexpectedStringParam(L, 1, "key");
   COLstring PublicKeyFormat  =  LUAoptionalStringParam(L, 1, "keyform", "PEM");
   COLstring Signature        =  LUAexpectedStringParam(L, 1, "signature");
   int       DataIndex        =  LCRcryptoPushData(L, 1);

   //Create an openSSL private key
   BIO *pBIO;
   pBIO = BIO_new_mem_buf(PublicKey.c_str(), PublicKey.size()); //pkey is of type char*

   EVP_PKEY* pPublicKey = PEM_read_bio_PUBKEY(pBIO, NULL, NULL, 0);
   BIO_free(pBIO);

   EVP_MD_CTX* pContext = NULL;
   pContext = EVP_MD_CTX_create();
   if (!pContext) {
      LUA_ERROR_STREAM("Failed to create Context: " << ERR_get_error());
      EVP_MD_CTX_free(pContext);
      EVP_PKEY_free(pPublicKey);
      return 0;
   }

   if(1 != EVP_DigestVerifyInit(pContext, NULL, EVP_get_digestbyname(DigestFunction.c_str()), NULL, pPublicKey)) {
      LUA_ERROR_STREAM("Failed to create Context: " << ERR_get_error());
      EVP_MD_CTX_free(pContext);
      EVP_PKEY_free(pPublicKey);
      return 0;
   }

   size_t DataSize;
   const char* Data = lua_tolstring(L, DataIndex, &DataSize);
   if(1 != EVP_DigestVerifyUpdate(pContext, Data, DataSize)) {
      LUA_ERROR_STREAM("Failed Update: " << ERR_get_error());
      EVP_MD_CTX_free(pContext);
      EVP_PKEY_free(pPublicKey);
      return 0;
   }

   bool Matches;
   const unsigned char* ExpSig = (const unsigned char*)Signature.get_buffer();
   int RC = EVP_DigestVerifyFinal(pContext, ExpSig, Signature.size());
   if(1 == RC) {
      Matches = true;
      COL_TRC("Signature matches");
   } else {
      Matches = false;
      COL_TRC("Signature does not match");
   }

   EVP_MD_CTX_free(pContext);
   EVP_PKEY_free(pPublicKey);

   lua_pushboolean(L, Matches);
   return 1;
}

int LCRpbkdf2(lua_State* L) {
   COL_FUNCTION(LCRpbkdf2);

   LUAcheckArgCount(L, 1, -1);
   LUAparamIsTable(L, 1);

   static const char* s_pbkdf2Args[] = { "password", "salt", "iterations", "algorithm", NULL };
   LUAcheckParams(L, 1, s_pbkdf2Args);

   COLstring Password         =  LUAexpectedStringParam(L, 1, "password");
   COLstring Salt             =  LUAexpectedStringParam(L, 1, "salt");
   COLstring DigestFunction   =  LUAoptionalStringParam(L, 1, "algorithm", "sha256");
   int       Iterations       =  LUAoptionalIntParam   (L, 1, "iterations", PKCS5_DEFAULT_ITER);

   COL_VAR3(Salt, DigestFunction, Iterations);
   COLstring Key;
   Key.setCapacity(EVP_MAX_KEY_LENGTH);
   Key.setSize(EVP_MAX_KEY_LENGTH);
   unsigned char* pKeyBuffer = (unsigned char*)Key.get_buffer();
   int ReturnCode = PKCS5_PBKDF2_HMAC(Password.c_str(), Password.size(), (unsigned char*) Salt.get_buffer(), Salt.size(), Iterations, EVP_get_digestbyname(DigestFunction.c_str()), EVP_MAX_KEY_LENGTH, pKeyBuffer);
   if (1 == ReturnCode) {
      LUApushString(L, Key);
      return 1;
   } else {
      LUA_ERROR_STREAM("Could not create key" << ERR_get_error());
      return 0;
   }
}

//---------------------------------------------------------------------------
// crypto.library

int LCRcryptoVersion(lua_State* L) {
   COL_FUNCTION(LCRcryptoVersion);
   LUAcheckArgCount(L, 0, -1);

   COLstring Result = OpenSSL_version(OPENSSL_VERSION);
   LUApushString(L, Result);
   return 1;
}

int LCRgetDigests(lua_State* L) {
   COL_FUNCTION(LCRgetDigests);
   LUAcheckArgCount(L, 0, -1);

   COLstring Result = "gost-mac, md4, md5, md_gost94, ripemd160, sha1, sha224, \
   sha256, sha384, sha512, streebog256, streebog512, whirlpool";
   LUApushString(L, Result);
   return 1;
}

int LCRgetCiphers(lua_State* L) {
   COL_FUNCTION(LCRgetCiphers);
   LUAcheckArgCount(L, 0, -1);

   COLstring Result = "aes-128-cbc, aes-128-ecb, aes-192-cbc, aes-192-ecb, aes-256-cbc, aes-256-ecb, \
      base64, bf, bf-cbc, bf-cfb, bf-ecb, bf-ofb, camellia-128-cbc, camellia-128-ecb, camellia-192-cbc, \
      camellia-192-ecb, camellia-256-cbc, camellia-256-ecb, cast, cast-cbc, cast5-cbc, cast5-cfb, cast5-ecb, \
      cast5-ofb, chacha, des, des-cbc, des-cfb, des-ecb, des-ede, des-ede-cbc, des-ede-cfb, des-ede-ofb, \
      des-ede3, des-ede3-cbc, des-ede3-cfb, des-ede3-ofb, des-ofb, des3, desx, rc2, rc2-40-cbc, rc2-64-cbc, \
      rc2-cbc, rc2-cfb, rc2-ecb, rc2-ofb, rc4, rc4-40";
   LUApushString(L, Result);
   return 1;
}

//---------------------------------------------------------------------------
// crypto.rsa

int LCRrsaEncryptPublic(lua_State* L) {
   COL_FUNCTION(LCRrsaEncryptPublic);
   LUAcheckArgCount(L, 1, -1);
   LUAparamIsTable(L, 1);
   static const char* s_RSAEncryptPublicArgs[] = { "data", "key", "keyform", NULL };
   LUAcheckParams(L, 1, s_RSAEncryptPublicArgs);

   COLstring Key              = LUAexpectedStringParam(L, 1, "key");
   COLstring KeyForm          = LUAoptionalStringParam(L, 1, "keyform", "PEM");\
   int       DataIndex        = LCRcryptoPushData(L, 1);
   size_t DataSize;
   const unsigned char* Data = (const unsigned char*) lua_tolstring(L, DataIndex, &DataSize);

   BIO *pBio = NULL;
   pBio = BIO_new_mem_buf(Key.c_str(), Key.size()); //pkey is of type char*
   if (NULL == pBio) {
      LUA_ERROR_STREAM("An Error occured reading the Key into a BIO structure" << hex << ERR_get_error() << flush);
      BIO_free(pBio);
      return 0;
   }

   EVP_PKEY* pPublicKey = PEM_read_bio_PUBKEY(pBio, NULL, NULL, NULL);
   if (NULL == pPublicKey) {
      LUA_ERROR_STREAM("An Error occured reading the Key into an EVP structure" << hex << ERR_get_error() << flush);
      BIO_free(pBio);
      EVP_PKEY_free(pPublicKey);
      return 0;
   }
   BIO_free(pBio); //Now the key is in EVP_PKEY we don't need the BIO representation

   EVP_PKEY_CTX* pContext = EVP_PKEY_CTX_new(pPublicKey, NULL);
   if (!pContext) {
      LUA_ERROR_STREAM("An Error occured creating the context: " << hex << ERR_get_error() << flush);
      EVP_PKEY_CTX_free(pContext);
      EVP_PKEY_free(pPublicKey);
      return 0;
   }

   if (0 >= EVP_PKEY_encrypt_init(pContext)) {
      LUA_ERROR_STREAM("An Error occured initializing the context: " << hex << ERR_get_error() << flush);
      EVP_PKEY_CTX_free(pContext);
      EVP_PKEY_free(pPublicKey);
      return 0;
   }

   if (0 >= EVP_PKEY_CTX_set_rsa_padding(pContext, RSA_PKCS1_OAEP_PADDING)) {
      LUA_ERROR_STREAM("An Error occured setting the padding: " << hex << ERR_get_error() << flush);
      EVP_PKEY_CTX_free(pContext);
      EVP_PKEY_free(pPublicKey);
      return 0;
   }

   size_t OutSize;
   if (EVP_PKEY_encrypt(pContext, NULL, &OutSize, Data, DataSize) <= 0) {
      LUA_ERROR_STREAM("An Error occured getting the output size: " << hex << ERR_get_error() << flush);
      EVP_PKEY_CTX_free(pContext);
      EVP_PKEY_free(pPublicKey);
      return 0;
   }

   unsigned char* Output = (unsigned char*) OPENSSL_malloc(OutSize);
   if (!Output) {
      LUA_ERROR_STREAM("An Error occured allocating memory for output: " << hex << ERR_get_error() << flush);
      EVP_PKEY_CTX_free(pContext);
      EVP_PKEY_free(pPublicKey);
      return 0;
   }

   if (0 >= EVP_PKEY_encrypt(pContext, Output, &OutSize, Data, DataSize)) {
      LUA_ERROR_STREAM("An Error occured encrypting the data" << hex << ERR_get_error() << flush);
      EVP_PKEY_CTX_free(pContext);
      EVP_PKEY_free(pPublicKey);
      return 0;
   }

   COLstring Result = COLstring((char*) Output, OutSize);
   LUApushString(L, Result);
   EVP_PKEY_CTX_free(pContext);
   EVP_PKEY_free(pPublicKey);
   return 1;
}

int LCRrsaDecryptPrivate(lua_State* L) {
   COL_FUNCTION(LCRrsaDecryptPrivate);
   LUAcheckArgCount(L, 1, -1);
   LUAparamIsTable(L, 1);
   static const char* s_RSADecryptPrivateArgs[] = { "data", "key", "keyform", NULL };
   LUAcheckParams(L, 1, s_RSADecryptPrivateArgs);

   COLstring Key              = LUAexpectedStringParam(L, 1, "key");
   COLstring KeyForm          = LUAoptionalStringParam(L, 1, "keyform", "PEM");\
   int       DataIndex        = LCRcryptoPushData(L, 1);
   size_t DataSize;
   const unsigned char* Data = (const unsigned char*) lua_tolstring(L, DataIndex, &DataSize);

   BIO *pBio = NULL;
   pBio = BIO_new_mem_buf(Key.c_str(), Key.size()); //pkey is of type char*
   if (NULL == pBio) {
      LUA_ERROR_STREAM("An Error occured reading the Key into a BIO structure" << hex << ERR_get_error() << flush);
      BIO_free(pBio);
      return 0;
   }

   EVP_PKEY* pPrivateKey = PEM_read_bio_PrivateKey(pBio, NULL, NULL, NULL);
   if (NULL == pPrivateKey) {
      LUA_ERROR_STREAM("An Error occured reading the Key into an EVP structure" << hex << ERR_get_error() << flush);
      BIO_free(pBio);
      EVP_PKEY_free(pPrivateKey);
      return 0;
   }
   BIO_free(pBio); //Now the key is in EVP_PKEY we don't need the BIO representation

   EVP_PKEY_CTX* pContext = EVP_PKEY_CTX_new(pPrivateKey, NULL);
   if (!pContext) {
      LUA_ERROR_STREAM("An Error occured creating the context: " << hex << ERR_get_error() << flush);
      EVP_PKEY_CTX_free(pContext);
      EVP_PKEY_free(pPrivateKey);
      return 0;
   }

   if (0 >= EVP_PKEY_decrypt_init(pContext)) {
      LUA_ERROR_STREAM("An Error occured initializing the context: " << hex << ERR_get_error() << flush);
      EVP_PKEY_CTX_free(pContext);
      EVP_PKEY_free(pPrivateKey);
      return 0;
   }

   if (0 >= EVP_PKEY_CTX_set_rsa_padding(pContext, RSA_PKCS1_OAEP_PADDING)) {
      LUA_ERROR_STREAM("An Error occured setting the padding: " << hex << ERR_get_error() << flush);
      EVP_PKEY_CTX_free(pContext);
      EVP_PKEY_free(pPrivateKey);
      return 0;
   }

   size_t OutSize;
   if (EVP_PKEY_decrypt(pContext, NULL, &OutSize, Data, DataSize) <= 0) {
      LUA_ERROR_STREAM("An Error occured getting the output size: " << hex << ERR_get_error() << flush);
      EVP_PKEY_CTX_free(pContext);
      EVP_PKEY_free(pPrivateKey);
      return 0;
   }

   unsigned char* Output = (unsigned char*) OPENSSL_malloc(OutSize);
   if (!Output) {
      LUA_ERROR_STREAM("An Error occured allocating memory for output: " << hex << ERR_get_error() << flush);
      EVP_PKEY_CTX_free(pContext);
      EVP_PKEY_free(pPrivateKey);
      return 0;
   }

   if (0 >= EVP_PKEY_decrypt(pContext, Output, &OutSize, Data, DataSize)) {
      LUA_ERROR_STREAM("An Error occured decrypting the data" << hex << ERR_get_error() << flush);
      EVP_PKEY_CTX_free(pContext);
      EVP_PKEY_free(pPrivateKey);
      return 0;
   }

   COLstring Result = COLstring((char*) Output, OutSize);
   LUApushString(L, Result);
   EVP_PKEY_CTX_free(pContext);
   EVP_PKEY_free(pPrivateKey);
   return 1;
}

//---------------------------------------------------------------------------
// crypto.aes

static const EVP_CIPHER* LCRtranslateCipher(const COLstring& Key, COLstring Mode) {
   COL_FUNCTION(LCRtranslateCipher);
   int Size = Key.size() * 8;    // in Bits
   COL_VAR(Size);
   if (Size != 128 && Size != 192 && Size != 256) { return NULL; }
   Mode.toLowerCase();
   COL_VAR(Mode);
   if (Mode != "ecb" && Mode != "cbc" && Mode != "cfb" && Mode != "ofb" && Mode != "ctr") { return NULL; }
   COLstring CipherName = "aes-" + COLintToString(Size) + "-" + Mode;
   COL_VAR(CipherName);
   return EVP_get_cipherbyname(CipherName.c_str());
}

int LCRaesEncrypt(lua_State* L) {
   COL_FUNCTION(LCRaesEncrypt);

   LUAcheckArgCount(L, 1, -1);
   LUAparamIsTable(L, 1);
   static const char* s_AesEncryptArgs[] = { "data", "key", "iv", "blockcipher", NULL };
   LUAcheckParams(L, 1, s_AesEncryptArgs);

   COLstring KeyBytes         = LUAexpectedStringParam(L, 1, "key");
   COLstring Iv               = LUAexpectedStringParam(L, 1, "iv");
   COLstring BlockCipherMode  = LUAexpectedStringParam(L, 1, "blockcipher");
   int DataIndex = LCRcryptoPushData(L, 1);

   COL_VAR4(DataIndex, KeyBytes, Iv, BlockCipherMode);

   unsigned char* Key = (unsigned char*) KeyBytes.get_buffer();
   unsigned char* IV = (unsigned char*) Iv.get_buffer();
   size_t DataSize;
   const unsigned char* Data = (const unsigned char*) lua_tolstring(L, DataIndex, &DataSize);

   unsigned char CipherText[128];
   int CipherTextLength;

   EVP_CIPHER_CTX* pCipherContext = NULL;

   if (!(pCipherContext = EVP_CIPHER_CTX_new())) {
      LUA_ERROR_STREAM("Failed to create Context: " << ERR_get_error());
      EVP_CIPHER_CTX_free(pCipherContext);
      return 0;
   }

   const EVP_CIPHER* pCipherType = LCRtranslateCipher(KeyBytes, BlockCipherMode);
   if (pCipherType == NULL) {
      LUA_ERROR_STREAM("No AES block cipher available for supplied key/mode.");
      EVP_CIPHER_CTX_free(pCipherContext);
      return 0;
   }

   if (1 != EVP_EncryptInit_ex(pCipherContext, pCipherType, NULL, Key, IV)) {
      LUA_ERROR_STREAM("Failed to initialize Cipher: " << ERR_get_error());
      EVP_CIPHER_CTX_free(pCipherContext);
      return 0;
   }

   if (1 != EVP_CIPHER_CTX_set_padding(pCipherContext, EVP_PADDING_PKCS7)) {
      LUA_ERROR_STREAM("Failed to set padding to zero: " << hex << ERR_get_error() << flush);
      EVP_CIPHER_CTX_free(pCipherContext);
      return 0;
   }

   int Length;
   if (1 != EVP_EncryptUpdate(pCipherContext, CipherText, &Length, Data, DataSize)) {
      LUA_ERROR_STREAM("Failed to Apply Cipher: " << ERR_get_error());
      EVP_CIPHER_CTX_free(pCipherContext);
      return 0;
   }
   CipherTextLength = Length;

   if(1 != EVP_EncryptFinal_ex(pCipherContext, CipherText + CipherTextLength, &Length)) {
      LUA_ERROR_STREAM("Failed to Finalize Cipher: " << ERR_get_error());
      EVP_CIPHER_CTX_free(pCipherContext);
      return 0;
   }

   CipherTextLength += Length;
   COLstring Result = COLstring((char*) CipherText, CipherTextLength);
   EVP_CIPHER_CTX_free(pCipherContext);
   LUApushString(L, Result);
   return 1;
}

int LCRaesDecrypt(lua_State* L) {
   COL_FUNCTION(LCRaesDecrypt);

   LUAcheckArgCount(L, 1, -1);
   LUAparamIsTable(L, 1);
   static const char* s_AesDecryptArgs[] = { "data", "key", "iv", "blockcipher", NULL };
   LUAcheckParams(L, 1, s_AesDecryptArgs);

   COLstring KeyBytes         = LUAexpectedStringParam(L, 1, "key");
   COLstring Iv               = LUAexpectedStringParam(L, 1, "iv");
   COLstring BlockCipherMode  = LUAexpectedStringParam(L, 1, "blockcipher");
   int       DataIndex        = LCRcryptoPushData(L, 1);

   COL_VAR4(DataIndex, KeyBytes, Iv, BlockCipherMode);

   unsigned char* Key = (unsigned char*) KeyBytes.get_buffer();
   unsigned char* IV = (unsigned char*) Iv.get_buffer();
   size_t DataSize;
   const unsigned char* Data = (const unsigned char*) lua_tolstring(L, DataIndex, &DataSize);
   COL_DUMP("Data: ", Data, DataSize);

   unsigned char Text[128];
   int TextLength;

   EVP_CIPHER_CTX* pCipherContext = NULL;

   if (!(pCipherContext = EVP_CIPHER_CTX_new())) {
      LUA_ERROR_STREAM("Failed to create Context: " << ERR_get_error());
      EVP_CIPHER_CTX_free(pCipherContext);
      return 0;
   }

   const EVP_CIPHER* pCipherType = LCRtranslateCipher(KeyBytes, BlockCipherMode);
   if (pCipherType == NULL) {
      LUA_ERROR_STREAM("No AES block cipher available for supplied key/mode.");
      EVP_CIPHER_CTX_free(pCipherContext);
      return 0;
   }

   if (1 != EVP_DecryptInit_ex(pCipherContext, pCipherType, NULL, Key, IV)) {
      LUA_ERROR_STREAM("Failed to initialize Cipher: " << ERR_get_error());
      EVP_CIPHER_CTX_free(pCipherContext);
      return 0;
   }

   if (1 != EVP_CIPHER_CTX_set_padding(pCipherContext, EVP_PADDING_PKCS7)) {
      LUA_ERROR_STREAM("Failed to set padding to zero: " << hex << ERR_get_error() << flush);
      EVP_CIPHER_CTX_free(pCipherContext);
      return 0;
   }

   int Length;
   if (1 != EVP_DecryptUpdate(pCipherContext, Text, &Length, Data, DataSize)) {
      LUA_ERROR_STREAM("Failed to Apply Cipher: " << ERR_get_error());
      EVP_CIPHER_CTX_free(pCipherContext);
      return 0;
   }
   TextLength = Length;

   if(1 != EVP_DecryptFinal_ex(pCipherContext, Text + Length, &Length)) {
      LUA_ERROR_STREAM("Failed to Finalize Cipher: " << hex << ERR_get_error() << flush);
      EVP_CIPHER_CTX_free(pCipherContext);
      return 0;
   }

   TextLength += Length;
   COLstring Result = COLstring((char*) Text, TextLength);
   EVP_CIPHER_CTX_free(pCipherContext);
   LUApushString(L, Result);
   return 1;
}

//---------------------------------------------------------------------------
// Register APIs

void LCRaddCryptoNamespace(lua_State* L) {
   COL_FUNCTION(LCRaddCryptoNamespace);

   lua_newtable(L); // crypto
   {
      LUAaddMethod(L, "digest", &LCRdigest);
      LUAaddMethod(L, "hmac", &LCRhmac);
      LUAaddMethod(L, "sign", &LCRdigitalSign);
      LUAaddMethod(L, "verify", &LCRdigitalVerify);
      LUAaddMethod(L, "pbkdf2", &LCRpbkdf2);

      // crypto.library
      lua_newtable(L); {
         LUAaddMethod(L, "version", &LCRcryptoVersion);
         LUAaddMethod(L, "digests", &LCRgetDigests);
         LUAaddMethod(L, "ciphers", &LCRgetCiphers);
      } lua_setfield(L, -2, "library");

      // crypto.rsa
      lua_newtable(L); {
         LUAaddMethod(L, "encrypt", &LCRrsaEncryptPublic);
         LUAaddMethod(L, "decrypt", &LCRrsaDecryptPrivate);
         // rsaEncryptPrivate and rsaDecryptPublic have been removed see IE-2302
      } lua_setfield(L, -2, "rsa");

      // crypto.aes
      lua_newtable(L); {
         LUAaddMethod(L, "encrypt", &LCRaesEncrypt);
         LUAaddMethod(L, "decrypt", &LCRaesDecrypt);
      } lua_setfield(L, -2, "aes");
   }
   lua_setglobal(L, "crypto");
}
