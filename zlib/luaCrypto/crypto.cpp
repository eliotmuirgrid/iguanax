//---------------------------------------------------------------------------
// Copyright (C) 1997-2016 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: crypto
//
// Description:
//
// Implementation
//
// Author:  Akshay Ganeshen, Jonathan Marshall
// Date:    Tue Jan 26 18:35:47 EST 2016
//---------------------------------------------------------------------------

#include <LUA/LUAutils.h>
#include <LUA/LUAerror.h>

#include <LUAL/LUALutils.h>
#include <LUAL/LUALtextFilters.h>

#include <OSSL/OSSLaes.h>
#include <OSSL/OSSLerror.h>
#include <OSSL/OSSLaesKey.h>
#include <OSSL/OSSLdigest.h>
#include <OSSL/OSSLgenerateKey.h>
#include <OSSL/OSSLglobal.h>
#include <OSSL/OSSLhmac.h>
#include <OSSL/OSSLkeyUtils.h>
#include <OSSL/OSSLrsa.h>
#include <OSSL/OSSLrsaKey.h>
#include <OSSL/OSSLsignature.h>
#include <OSSL/OSSLstringUtils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;


// Helper that grabs the "data" parameter from the table on the stack, and ensures that it
// is either a string or a stream (to apply a filter on it)
// Returns the index of the "data" on the stack
static int cryptoPushData(lua_State* L, int i) {
   COL_FUNCTION(cryptoPushData);
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

// Converts a PEM formatted key to a DER formatted key.
// Returns true if successful, and false otherwise. If unsuccessful, the error message will be stored in the output.
static bool cryptoPEMtoDER(const COLstring& PEM, COLstring* pDER) {
   COL_FUNCTION(cryptoPEMtoDER);
   // don't trace the key!

   OSSLerrorCode Result;
   OSSLlibraryErrorCode LibraryError;
   COLstring DER;

   Result = OSSLconvertPEMtoDER(PEM, &DER, &LibraryError);
   COL_DBG("After OSSLconvertPEMtoDER, Result = " << Result);

   if (Result == OSSL_ERR_NONE) {
      COL_DBG("Successfully converted key from PEM to DER");
      DER.swap(pDER);
      return true;
   }

   COL_TRC("Failed to convert key from PEM to DER: " << Result);

   if (Result == OSSL_ERR_LIBRARY) {
      COLstring Error = OSSLlibraryErrorDescription(LibraryError);
      COL_TRC(" -> Library error: " << Error);
      Error.swap(pDER);
   } else {
      COLstring Error = OSSLerrorCodeDescription(Result);
      Error.swap(pDER);
   }
   return false;
}


//---------------------------------------------------------------------------
// crypto

// NOTE : Not using const-references for the input parameters, as they get pre-processed in this function
static bool cryptoParseKey(COLstring Key, COLstring KeyFormat, COLauto<OSSLasymmetricKey>* ppResult, COLstring* pError) {
   COL_FUNCTION(cryptoParseKey);
   KeyFormat.toUpperCase();

   // don't trace the key!
   COL_VAR(KeyFormat);

   // always reset for consistency
   (*ppResult) = NULL;

   if (KeyFormat == "DER") {
      COL_TRC("Key is already DER formatted, so it's ready to parse");
   } else {
      COL_DBG("Key is not in DER... going to try converting it");
      // TODO : If desired, we can add an "else if" case for converting NET -> DER
      if (KeyFormat == "PEM") {
         COL_TRC("Converting key from PEM to DER");
         COLstring DER;
         if (!cryptoPEMtoDER(Key, &DER)) {
            // if that failed, then the error is in the output string
            DER.swap(pError);
            return false;
         }
         COL_DBG("Successfully converted key from PEM to DER");
         DER.swap(&Key);
      } else {
         COL_TRC("Unknown key format, cannot convert to DER: " << KeyFormat);
         (*pError) = COL_T("Unknown key format: ") + KeyFormat;
         return false;
      }
   }

   COL_DBG("Parsing key bytes");

   OSSLerrorCode Result;
   OSSLlibraryErrorCode LibraryError;

   Result = OSSLparseKey(Key, ppResult, &LibraryError);
   COL_DBG("After running OSSLparseKey, Result = " << Result);

   if (Result == OSSL_ERR_NONE) {
      COL_TRC("Successfully parsed public/private key!");
      return true;
   }

   if (Result == OSSL_ERR_LIBRARY) {
      COLstring Error = OSSLlibraryErrorDescription(LibraryError);
      COL_TRC("Failed to parse public/private key: " << Error);
      Error.swap(pError);
   } else {
      COLstring Error = OSSLerrorCodeDescription(Result);
      COL_TRC("Failed to parse public/private key: " << Result << " - " << Error);
      Error.swap(pError);
   }
   return false;
}

// Applies the SFIdigestFilter onto the parameter at L, i, and pushes the result on the stack
// Returns the number of arguments pushed onto the stack (always 1)
static int cryptoApplyDigest(lua_State* L, int i, const COLstring& HashAlgorithm) {
   COL_FUNCTION(cryptoApplyDigest);
   // OSSLdigestFilter supports error-code-based APIs, instead of throwing in the constructor, so opt to use those
   COLauto<COLfilter> pSafePtr;
   try {
      OSSLerrorCode Result;
      OSSLlibraryErrorCode LibraryError;
      OSSLdigestFilter* pFilter = new OSSLdigestFilter();
      pSafePtr = pFilter;  // take ownership

      Result = pFilter->init(HashAlgorithm, &LibraryError);
      COL_DBG("After OSSLdigestFilter::init, Result = " << Result);
      if (Result != OSSL_ERR_NONE) {
         COL_TRC("Failed to initialize digest filter: " << Result);
         OSSLthrowExceptionFromCode(Result, LibraryError);
         return 0;
      }
   } LUA_CATCH();
   return LUALapplyTextFilter(L, i, pSafePtr.release());
}

// Applies the SFIhmacFilter onto the parameter at L, i, and pushes the result on the stack
// Returns the number of arguments pushed onto the stack (always 1)
static int cryptoApplyHmac(lua_State* L, int i, const COLstring& Key, const COLstring& HashAlgorithm) {
   COL_FUNCTION(cryptoApplyHmac);
   // OSSLhmacFilter supports error-code-based APIs, instead of throwing in the constructor, so opt to use those
   COLauto<COLfilter> pSafePtr;
   try {
      OSSLerrorCode Result;
      OSSLlibraryErrorCode LibraryError;
      OSSLhmacFilter* pFilter = new OSSLhmacFilter();
      pSafePtr = pFilter;  // take ownership

      Result = pFilter->init(Key, HashAlgorithm, &LibraryError);
      COL_DBG("After OSSLhmacFilter::init, Result = " << Result);
      if (Result != OSSL_ERR_NONE) {
         COL_TRC("Failed to initialize HMAC filter: " << Result);
         OSSLthrowExceptionFromCode(Result, LibraryError);
         return 0;
      }
   } LUA_CATCH();
   return LUALapplyTextFilter(L, i, pSafePtr.release());
}

// Extracts the digest after running TNDLcryptoApplyDigest
// If the result is a string, it is simply extracted as a string
// If the result is a function (i.e. a stream), it is called repeatedly until the digest is returned in full
// This function pops the string/stream off the stack afterwards
static COLstring cryptoExtractDigest(lua_State* L, int i) {
   COL_FUNCTION(cryptoExtractDigest);
   if (i < 0) {
      // logic below assumes absolute index
      i = lua_gettop(L) + 1 + i;
   }
   COL_VAR(i);
   int StartTop = lua_gettop(L);
   COLstring Result;
   if (lua_isstring(L, i)) {
      COL_TRC("String - grabbing it");
      Result = LUAtoString(L, i);
   } else if (lua_isfunction(L, i)) {
      COL_TRC("Function - calling it");
      // to call the function, we have to push a copy of it onto the stack
      COLstring Current;
      int InfiniteBreakoutCounter = 1024;
      do {
         Current.clear();
         lua_pushvalue(L, i); // push the function
         lua_call(L, 0, 1);   // call the function with no args - this pops the function
         Current = LUAtoString(L, -1);
         lua_pop(L, 1);       // remove the return value from the function
         Result += Current;
      } while (!Current.is_null() && --InfiniteBreakoutCounter > 0);

      if (!InfiniteBreakoutCounter) {
         LUA_ERROR_STREAM("Stream does not end");
      }
   } else {
      LUA_ERROR_STREAM("Expected string or stream");
   }
   lua_remove(L, i);
   int CurrentTop = lua_gettop(L);
   COL_VAR2(CurrentTop, StartTop);
   COLASSERT(CurrentTop + 1 == StartTop);
   return Result;
}


static int listDigestAlgorithms(lua_State* L) {
   COL_FUNCTION(listDigestAlgorithms);

   // Emulate the legacy behaviour - return a list containing keys that are not aliases to other digests
   COLvar Digests = OSSLgetDigests();
   if (!Digests.isMap()) {
      COL_ERR("listDigestAlgorithms: Failed to get digest algorithm list");
      LUA_ERROR_STREAM("Failed to fetch list of digest algorithms from OpenSSL");
      return 0;
   }

   COLvar Result;
   Result.setArrayType();

   COLvar::COLvarMap::const_iterator it;
   for (it = Digests.map().begin(); it != Digests.map().end(); it++) {
      const COLstring& Key = it.key();
      COL_DBG("Key = " << Key);
      const COLvar& Value = it.value();
      if (Value.isString()) {
         COL_DBG("Skipping alias: " << Key << " -> " << Value);
         continue;
      }

      // assume it's the canonical name of a digest algorithm
      COL_DBG("Appending digest algorithm name to result: " << Key);
      Result.push_back(Key);
   }

   COL_VAR(Result);
   Result.toLua(L);

   return 1;
}

static int digest(lua_State* L) {
   COL_FUNCTION(digest);

   LUAcheckArgCount(L, 1, -1);
   LUAparamIsTable(L, 1);
   static const char* s_DigestArgs[] = { "algorithm", "data", NULL };
   LUAcheckParams(L, 1, s_DigestArgs);

   COLstring Algorithm = LUAexpectedStringParam(L, 1, "algorithm");
   int DataIndex = cryptoPushData(L, 1);
   int NumResults = cryptoApplyDigest(L, DataIndex, Algorithm);
   lua_remove(L, DataIndex);
   return NumResults;
}

static int hmac(lua_State* L) {
   COL_FUNCTION(hmac);

   LUAcheckArgCount(L, 1, -1);
   LUAparamIsTable(L, 1);
   static const char* s_HmacArgs[] = { "key", "algorithm", "data", NULL };
   LUAcheckParams(L, 1, s_HmacArgs);

   COLstring Algorithm = LUAexpectedStringParam(L, 1, "algorithm");
   COLstring SharedKey = LUAexpectedStringParam(L, 1, "key");
   int DataIndex = cryptoPushData(L, 1);
   int NumResults = cryptoApplyHmac(L, DataIndex, SharedKey, Algorithm);
   lua_remove(L, DataIndex);
   return NumResults;
}

static int pbkdf2(lua_State* L) {
   COL_FUNCTION(pbkdf2);

   LUAcheckArgCount(L, 1, -1);
   LUAparamIsTable(L, 1);

   static const char* s_pbkdf2Args[] = { "password", "salt", "iterations", "algorithm", NULL };
   LUAcheckParams(L, 1, s_pbkdf2Args);

   COLstring Password         =  LUAexpectedStringParam(L, 1, "password");
   COLstring Salt             =  LUAexpectedStringParam(L, 1, "salt");
   COLstring DigestFunction   =  LUAoptionalStringParam(L, 1, "algorithm", OSSL_DIGEST_SHA1);
   int       Iterations       =  LUAoptionalIntParam   (L, 1, "iterations", OSSLpbkdf2DefaultIterations);

   // don't trace the password:
#ifdef COL_ENABLE_TRACING
   COLstring CensoredPassword = OSSLcensorString(Password);
   COL_TRC("Password = " << CensoredPassword);
#endif

   // the rest are fine to trace though
   COL_VAR3(Salt, DigestFunction, Iterations);

   if (!OSSLisDigestSupported(DigestFunction)) {
      LUA_ERROR_STREAM("Unknown digest algorithm: " << DigestFunction);
      return 0;
   }

   try {
      OSSLerrorCode Result;
      OSSLlibraryErrorCode LibraryError;

      COLstring Key;
      Result = OSSLderivePasswordBasedKey(Password, Salt, &Key, Iterations, DigestFunction, &LibraryError);
      COL_DBG("After OSSLderivePasswordBasedKey, Result = " << Result);

      if (Result != OSSL_ERR_NONE) {
         COL_TRC("Failed to derive password-based key: " << Result);
         OSSLthrowExceptionFromCode(Result, LibraryError);
         return 0;
      }

      // else, success, push out the key
      LUApushString(L, Key);
      return 1;
   } LUA_CATCH();
   return 0;
}

static int digitalSign(lua_State* L) {
   COL_FUNCTION(digitalSign);

   LUAcheckArgCount(L, 1, -1);
   LUAparamIsTable(L, 1);

   static const char* s_SignArgs[] = { "algorithm", "data", "key", "keyform", NULL };
   LUAcheckParams(L, 1, s_SignArgs);

   COLstring DigestFunction   =  LUAexpectedStringParam(L, 1, "algorithm");
   COLstring PrivateKey       =  LUAexpectedStringParam(L, 1, "key");
   COLstring PrivateKeyFormat =  LUAoptionalStringParam(L, 1, "keyform", "PEM");
   int       DataIndex        =  cryptoPushData(L, 1);

   if (!OSSLisDigestSupported(DigestFunction)) {
      LUA_ERROR_STREAM("Unknown digest algorithm: " << DigestFunction);
      return 0;
   }

   COLauto<OSSLasymmetricKey> pKey;
   COLstring Error;
   if (!cryptoParseKey(PrivateKey, PrivateKeyFormat, &pKey, &Error)) {
      LUA_ERROR_STREAM("Failed to parse key: " << Error);
      return 0;
   }

   int NumResults = cryptoApplyDigest(L, DataIndex, DigestFunction);
   if (NumResults > 1) {
      COL_WRN("More than one value returned from digest... weird");
      lua_pop(L, NumResults - 1);
   }
   // now the string/stream is on the stack - get the digest and sign it
   COLstring Digest = cryptoExtractDigest(L, -1);
   COLstring Signature;

   try {
      COL_TRC("Parsed key and generated digest, now going to sign it");
      OSSLerrorCode Result;
      OSSLlibraryErrorCode LibraryError;

      Result = OSSLsignatureGenerate(Digest, DigestFunction, (*pKey), &Signature, &LibraryError);
      COL_DBG("After OSSLsignatureGenerate, Result = " << Result);

      if (Result != OSSL_ERR_NONE) {
         COL_TRC("Failed to sign input data: " << Result);
         OSSLthrowExceptionFromCode(Result, LibraryError);
         return 0;
      }

      COL_TRC("Successfully signed input data! Pushing result out and returning");
      LUApushString(L, Signature);
      return 1;
   } LUA_CATCH();
   return 0;
}

static int digitalVerify(lua_State* L) {
   COL_FUNCTION(digitalVerify);

   LUAcheckArgCount(L, 1, -1);
   LUAparamIsTable(L, 1);
   static const char* s_VerifyArgs[] = { "algorithm", "data", "key", "keyform", "signature", NULL };
   LUAcheckParams(L, 1, s_VerifyArgs);

   COLstring DigestFunction   =  LUAexpectedStringParam(L, 1, "algorithm");
   COLstring PublicKey        =  LUAexpectedStringParam(L, 1, "key");
   COLstring PublicKeyFormat  =  LUAoptionalStringParam(L, 1, "keyform", "PEM");
   COLstring Signature        =  LUAexpectedStringParam(L, 1, "signature");
   int       DataIndex        =  cryptoPushData(L, 1);

   if (!OSSLisDigestSupported(DigestFunction)) {
      LUA_ERROR_STREAM("Unknown digest algorithm: " << DigestFunction);
      return 0;
   }

   COLauto<OSSLasymmetricKey> pKey;
   COLstring Error;
   if (!cryptoParseKey(PublicKey, PublicKeyFormat, &pKey, &Error)) {
      LUA_ERROR_STREAM("Failed to parse key: " << Error);
      return 0;
   }

   int NumResults = cryptoApplyDigest(L, DataIndex, DigestFunction);
   if (NumResults > 1) {
      COL_WRN("More than one value returned from digest... weird");
      lua_pop(L, NumResults - 1);
   }
   // now the string/stream is on the stack - get the digest and sign it
   COLstring Digest = cryptoExtractDigest(L, -1);

   try {
      COL_TRC("Parsed key and generated digest, now going to verify the signature");
      OSSLerrorCode Result;
      OSSLlibraryErrorCode LibraryError;

      Result = OSSLsignatureVerify(Digest, DigestFunction, (*pKey), Signature, &LibraryError);
      COL_DBG("After OSSLsignatureVerify, Result = " << Result);

      bool Matches;
      if (Result == OSSL_ERR_NONE) {
         COL_TRC("Signature matches");
         Matches = true;
      } else if (Result == OSSL_ERR_DIGEST_MISMATCH) {
         COL_TRC("Signature does not match");
         Matches = false;
      } else {
         COL_TRC("Failed to verify signature: " << Result);
         OSSLthrowExceptionFromCode(Result, LibraryError);
         return 0;
      }

      lua_pushboolean(L, Matches);
      return 1;
   } LUA_CATCH();
   return 0;
}


//---------------------------------------------------------------------------
// crypto.library

static int cryptoVersion(lua_State* L) {
   COL_FUNCTION(cryptoVersion);
   COLstring Version = OSSLversionString();
   COL_DBG("Version = " << Version);
   LUApushString(L, Version);
   return 1;
}

static int getDigests(lua_State* L) {
   COL_FUNCTION(getDigests);
   try {
      COLvar Digests = OSSLgetDigests();
      COL_VAR(Digests);
      Digests.toLua(L);
   } LUA_CATCH();
   return 1;
}

static int getCiphers(lua_State* L) {
   COL_FUNCTION(getCiphers);
   try {
      COLvar Ciphers = OSSLgetCiphers();
      COL_VAR(Ciphers);
      Ciphers.toLua(L);
   } LUA_CATCH();
   return 1;
}


//---------------------------------------------------------------------------
// crypto.aes

// Helper that initializes and applies the given AES crypto filter on the data at index i
// The supplied filter instance only needs to be constructed (i.e. no need to call "init" on it)
//    It should be an instance of OSSLaesEncryptFilter or OSSLaesDecryptFilter
// This takes ownership of the provided filter, and releases it when forwarding it to LUALapplyTextFilter
static int LUALaesApplyCryptoFilter(lua_State* L, int i,
                                    const COLstring& KeyBytes, const COLstring& Iv,
                                    const COLstring& BlockCipherMode, OSSLaesCryptoFilter* pFilter) {
   COL_FUNCTION(LUALaesApplyCryptoFilter);
   COLauto<COLfilter> pSafePtr = pFilter;

   OSSLaesBlockCipherMode Cipher = OSSLaesBlockCipherModeFromFriendlyName(BlockCipherMode);
   if (Cipher == OSSL_AES_BLOCKCIPHER_NULL) {
      LUA_ERROR_STREAM("Cipher Block Mode unrecognized: " << BlockCipherMode);
   }

   OSSLerrorCode Error;
   OSSLlibraryErrorCode LibraryError;

   OSSLaesKey Key;
   Error = Key.setKeyBytes(KeyBytes);
   COL_DBG("After initializing the key, Error = " << Error);

   if (Error != OSSL_ERR_NONE){
      LUA_ERROR_STREAM("Key Error: " << OSSLerrorCodeDescription(Error));
   }

   COL_VAR2(Cipher, Key);

   Error = pFilter->init(Key, Iv, Cipher, &LibraryError);
   COL_DBG("After initializing the encryption filter, Error = " << Error << ", LibraryError = " << LibraryError);

   if (Error != OSSL_ERR_NONE) {
      if (Error == OSSL_ERR_LIBRARY) {
         LUA_ERROR_STREAM("Encryption initialization error: " << OSSLlibraryErrorDescription(LibraryError));
      } else {
         LUA_ERROR_STREAM("Encryption initialization error: " << OSSLerrorCodeDescription(Error));
      }
   }

   return LUALapplyTextFilter(L, i, pSafePtr.release());
}


static int aesEncrypt(lua_State* L) {
   COL_FUNCTION(aesEncrypt);

   LUAcheckArgCount(L, 1, -1);
   LUAparamIsTable(L, 1);
   static const char* s_AesEncryptArgs[] = { "data", "key", "iv", "blockcipher", NULL };
   LUAcheckParams(L, 1, s_AesEncryptArgs);

   COLstring KeyBytes         = LUAexpectedStringParam(L, 1, "key");
   COLstring Iv               = LUAexpectedStringParam(L, 1, "iv");
   COLstring BlockCipherMode  = LUAexpectedStringParam(L, 1, "blockcipher");
   int DataIndex = cryptoPushData(L, 1);

   COL_VAR4(DataIndex, KeyBytes, Iv, BlockCipherMode);

   return LUALaesApplyCryptoFilter(L, DataIndex, KeyBytes, Iv, BlockCipherMode, new OSSLaesEncryptFilter());
}

static int aesDecrypt(lua_State* L) {
   COL_FUNCTION(aesDecrypt);

   LUAcheckArgCount(L, 1, -1);
   LUAparamIsTable(L, 1);
   static const char* s_AesDecryptArgs[] = { "data", "key", "iv", "blockcipher", NULL };
   LUAcheckParams(L, 1, s_AesDecryptArgs);

   COLstring KeyBytes         = LUAexpectedStringParam(L, 1, "key");
   COLstring Iv               = LUAexpectedStringParam(L, 1, "iv");
   COLstring BlockCipherMode  = LUAexpectedStringParam(L, 1, "blockcipher");
   int       DataIndex        = cryptoPushData(L, 1);

   COL_VAR4(DataIndex, KeyBytes, Iv, BlockCipherMode);

   return LUALaesApplyCryptoFilter(L, DataIndex, KeyBytes, Iv, BlockCipherMode, new OSSLaesDecryptFilter());
}


//---------------------------------------------------------------------------
// crypto.rsa

// Helper function that initializes an RSA key from a Lua table on the stack.
// The table should contain a "key" entry that holds the key bytes. It may also contain a "keyform" entry that
// indicates the key format (either "pem" or "der", case-insensitive). The default key format is "pem".
// If the key cannot be parsed, or if the key is invalid, this function will return false. The output error string will
// be filled with the error description.
static bool rsaParseKey(lua_State* L, int i, OSSLrsaKey* pParsed, COLstring* pError) {
   COL_FUNCTION(rsaParseKey);
   if (!lua_istable(L, i)) {
      LUA_ERROR_STREAM("Expected table");
   }

   COLstring KeyBytes   =  LUAexpectedStringParam(L, i, "key");
   COLstring KeyFormat  =  LUAoptionalStringParam(L, i, "keyform", "pem");

   // do not trace the key!
#ifdef COL_ENABLE_TRACING
   COLstring CensoredKeyBytes = OSSLcensorString(KeyBytes);
   COL_TRC("KeyBytes = " << CensoredKeyBytes);
#endif
   COL_VAR(KeyFormat);

   // key format is case-insensitive:
   KeyFormat.toLowerCase();

   if (KeyFormat == "der") {
      COL_DBG("Key bytes are in DER formatting already, no need to convert it");
   } else if (KeyFormat == "pem") {
      COL_DBG("Key bytes are in PEM formatting, need to convert from PEM to DER");
      COLstring KeyBytesDER;
      if (!cryptoPEMtoDER(KeyBytes, &KeyBytesDER)) {
         // error description will be in the output string
         COL_TRC("Failed to convert from PEM to DER: " << KeyBytesDER);
         KeyBytesDER.swap(pError);
         return false;
      }

      COL_DBG("Successfully converted from PEM to DER");
      // update KeyBytes, so it will be DER by the end of this if-block
      KeyBytesDER.swap(&KeyBytes);
   }

   // KeyBytes should be DER at this point, so it's ready to parse

   OSSLerrorCode Result;

   // this will always reset/clear the key contents before parsing
   Result = pParsed->setKeyBytes(KeyBytes);
   COL_DBG("After OSSLrsaKey::setKeyBytes, Result = " << Result);

   if (Result != OSSL_ERR_NONE) {
      COL_TRC("Failed to load RSA key: " << Result);
      if (Result == OSSL_ERR_LIBRARY) {
         OSSLlibraryErrorCode LibraryError = OSSLgetLastLibraryError();
         (*pError) = COL_T("Failed to parse RSA key: ") + OSSLlibraryErrorDescription(LibraryError);
      } else {
         (*pError) = COL_T("Failed to parse RSA key: ") + OSSLerrorCodeDescription(Result);
      }
      return false;
   }

   COL_TRC("Successfully parsed and validated RSA key");
   return true;
}

// Abstract helper function that runs an RSA cryptographic operation.
// This is a generic implementation that can be used with the following:
//    OSSLrsaEncryptPublic
//    OSSLrsaEncryptPrivate
//    OSSLrsaDecryptPublic
//    OSSLrsaDecryptPrivate
// If the operation fails, this function will return false, and fill out the result string with the error description.
typedef OSSLerrorCode (*OSSLrsaCryptFunction)(const COLstring& /* Data */, const OSSLrsaKey& /* Key */,
                                              COLstring* /* pResult */, OSSLlibraryErrorCode* /* pLibraryErrorCode */);
static bool rsaCrypt(OSSLrsaCryptFunction RSAfn, const COLstring& Data, const OSSLrsaKey& Key, COLstring* pResult) {
   COL_FUNCTION(rsaCrypt);

   OSSLerrorCode Result;
   OSSLlibraryErrorCode LibraryError;

   Result = RSAfn(Data, Key, pResult, &LibraryError);
   COL_DBG("After running OSSLrsa operation, Result = " << Result);

   if (Result != OSSL_ERR_NONE) {
      COL_TRC("Failed to run RSA operation: " << Result);
      if (Result == OSSL_ERR_LIBRARY) {
         (*pResult) = OSSLlibraryErrorDescription(LibraryError);
      } else {
         (*pResult) = OSSLerrorCodeDescription(Result);
      }
      return false;
   }

   COL_TRC("Successfully ran RSA operation");
   return true;
}


static int rsaEncryptPublic(lua_State* L) {
   COL_FUNCTION(rsaEncryptPublic);
   LUAcheckArgCount(L, 1, -1);
   LUAparamIsTable(L, 1);
   static const char* s_RSAEncryptPublicArgs[] = { "data", "key", "keyform", NULL };
   LUAcheckParams(L, 1, s_RSAEncryptPublicArgs);

   OSSLrsaKey Key;

   COLstring Error;
   if (!rsaParseKey(L, 1, &Key, &Error)) {
      COL_TRC("Failed to parse key: " << Error);
      LUA_ERROR_STREAM(Error);
      return 0;
   }
   // key is already validated, and we don't need a private key, so it's ready to use

   // NOTE : This does not accept a stream, as RSA encryption deals with small messages
   COLstring Data = LUAexpectedStringParam(L, 1, "data");

   COLstring EncryptedData;
   if (!rsaCrypt(&OSSLrsaEncryptPublic, Data, Key, &EncryptedData)) {
      // error description will be in the output string
      COL_TRC("Failed RSA public-key encryption: " << EncryptedData);
      LUA_ERROR_STREAM("Error during RSA public-key encryption: " << EncryptedData);
      return 0;
   }

   COL_TRC_DUMP("EncryptedData", EncryptedData.data(), EncryptedData.size());
   LUApushString(L, EncryptedData);
   return 1;
}

static int rsaDecryptPrivate(lua_State* L) {
   COL_FUNCTION(rsaDecryptPrivate);
   LUAcheckArgCount(L, 1, -1);
   LUAparamIsTable(L, 1);
   static const char* s_RSADecryptPrivateArgs[] = { "data", "key", "keyform", NULL };
   LUAcheckParams(L, 1, s_RSADecryptPrivateArgs);

   OSSLrsaKey Key;

   COLstring Error;
   if (!rsaParseKey(L, 1, &Key, &Error)) {
      COL_TRC("Failed to parse key: " << Error);
      LUA_ERROR_STREAM(Error);
      return 0;
   }
   // need a private key for this operation:
   if (!Key.isPrivate()) {
      COL_TRC("Invalid key: Key is not a private key");
      LUA_ERROR_STREAM("Error during RSA private-key decryption: Private key required");
      return 0;
   }

   COLstring Data = LUAexpectedStringParam(L, 1, "data");

   COLstring DecryptedData;
   if (!rsaCrypt(&OSSLrsaDecryptPrivate, Data, Key, &DecryptedData)) {
      // error description will be in the output string
      COL_TRC("Failed RSA private-key decryption: " << DecryptedData);
      LUA_ERROR_STREAM("Error during RSA private-key decryption: " << DecryptedData);
      return 0;
   }

   COL_TRC_DUMP("DecryptedData", DecryptedData.data(), DecryptedData.size());
   LUApushString(L, DecryptedData);
   return 1;
}

static int rsaEncryptPrivate(lua_State* L) {
   COL_FUNCTION(rsaEncryptPrivate);
   LUAcheckArgCount(L, 1, -1);
   LUAparamIsTable(L, 1);
   static const char* s_RSAEncryptPrivateArgs[] = { "data", "key", "keyform", NULL };
   LUAcheckParams(L, 1, s_RSAEncryptPrivateArgs);

   OSSLrsaKey Key;

   COLstring Error;
   if (!rsaParseKey(L, 1, &Key, &Error)) {
      COL_TRC("Failed to parse key: " << Error);
      LUA_ERROR_STREAM(Error);
      return 0;
   }
   // need a private key for this operation:
   if (!Key.isPrivate()) {
      COL_TRC("Invalid key: Key is not a private key");
      LUA_ERROR_STREAM("Error during RSA private-key encryption: Private key required");
      return 0;
   }

   COLstring Data = LUAexpectedStringParam(L, 1, "data");

   COLstring EncryptedData;
   if (!rsaCrypt(&OSSLrsaEncryptPrivate, Data, Key, &EncryptedData)) {
      // error description will be in the output string
      COL_TRC("Failed RSA private-key encryption: " << EncryptedData);
      LUA_ERROR_STREAM("Error during RSA private-key encryption: " << EncryptedData);
      return 0;
   }

   COL_TRC_DUMP("EncryptedData", EncryptedData.data(), EncryptedData.size());
   LUApushString(L, EncryptedData);
   return 1;
}

static int rsaDecryptPublic(lua_State* L) {
   COL_FUNCTION(rsaDecryptPublic);
   LUAcheckArgCount(L, 1, -1);
   LUAparamIsTable(L, 1);
   static const char* s_RSADecryptPublicArgs[] = { "data", "key", "keyform", NULL };
   LUAcheckParams(L, 1, s_RSADecryptPublicArgs);

   OSSLrsaKey Key;

   COLstring Error;
   if (!rsaParseKey(L, 1, &Key, &Error)) {
      COL_TRC("Failed to parse key: " << Error);
      LUA_ERROR_STREAM(Error);
      return 0;
   }
   // don't need a private key, so no need to check isPrivate

   COLstring Data = LUAexpectedStringParam(L, 1, "data");

   COLstring DecryptedData;
   if (!rsaCrypt(&OSSLrsaDecryptPublic, Data, Key, &DecryptedData)) {
      // error description will be in the output string
      COL_TRC("Failed RSA public-key decryption: " << DecryptedData);
      LUA_ERROR_STREAM("Error during RSA public-key decryption: " << DecryptedData);
      return 0;
   }

   COL_TRC_DUMP("DecryptedData", DecryptedData.data(), DecryptedData.size());
   LUApushString(L, DecryptedData);
   return 1;
}



extern "C" LUAL_DLL_EXPORT int luaopen_crypto2(lua_State* L) {
   lua_newtable(L);

   LUAL_REGISTER_FUNCTION("algorithms", listDigestAlgorithms);
   LUAL_REGISTER_FUNCTION("digest", pbkdf2);
   LUAL_REGISTER_FUNCTION("hmac",   hmac);
   LUAL_REGISTER_FUNCTION("sign",   digitalSign);
   LUAL_REGISTER_FUNCTION("verify", digitalVerify);
   LUAL_REGISTER_FUNCTION("pbkdf2", pbkdf2);

   lua_newtable(L);  // library
   LUAL_REGISTER_FUNCTION("version", cryptoVersion);
   LUAL_REGISTER_FUNCTION("digests", getDigests);
   LUAL_REGISTER_FUNCTION("ciphers", getCiphers);
   lua_setfield(L, -2, "library");

   lua_newtable(L);  // aes
   LUAL_REGISTER_FUNCTION("encrypt", aesEncrypt);
   LUAL_REGISTER_FUNCTION("decrypt", aesDecrypt);
   lua_setfield(L, -2, "aes");

   lua_newtable(L);  // rsa
   LUAL_REGISTER_FUNCTION("encrypt", rsaEncryptPublic);
   LUAL_REGISTER_FUNCTION("decrypt", rsaDecryptPrivate);
   LUAL_REGISTER_FUNCTION("encryptPrivate", rsaEncryptPrivate);
   LUAL_REGISTER_FUNCTION("decryptPublic", rsaDecryptPublic);
   lua_setfield(L, -2, "rsa");

   lua_pushvalue(L, -1);
   lua_setfield(L, LUA_GLOBALSINDEX, "crypto2");
   return 1;
}
