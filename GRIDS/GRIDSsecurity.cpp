//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GRIDsecurity.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <DIR/DIRpath.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <GRIDS/GRIDSsecurity.h>
#include <SFI/SFIaesDecryptor.h>
#include <SFI/SFIaesEncryptor.h>
#include <TUN/TUNaes.h>
#include <TUN/TUNdiffyHellmanSecret.h>
#include <TUN/TUNkey.h>
#include <TUN/TUNkeyConvert.h>
#include <TUN/TUNkeyEllipticalGen.h>
#include <TUN/TUNkeyLoad.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/kdf.h>
COL_LOG_MODULE;

void GRIDSloadOrGenerateKey(TUNkey* pKey) {
   COL_FUNCTION(GRIDSloadOrGenerateKey);
   const COLstring KeyFile = DIRfileConfigTunnelKey();
   if(FILfileExists(KeyFile)) {
      TUNkeyLoad(KeyFile, pKey);
      COL_TRC("Loaded GRID private key from " << KeyFile);
   } else {
      TUNgenerateEllipticalKey(pKey);
      FILmakeFullDir(FILpathDir(KeyFile), FIL_USER_RWX);
      FILwriteFile(KeyFile, TUNkeyPrivateAsPemString(*pKey));
      COL_TRC("Generated new GRID private key at " << KeyFile);
   }
}

COLstring GRIDSencrypt(const TUNkey& MyPrivateKey, const TUNkey& PeerPublicKey, const COLstring& Plaintext) {
   COL_FUNCTION(GRIDSencrypt);
   const COLstring SharedSecret = TUNdiffyHellmanSecret(MyPrivateKey, PeerPublicKey);
   COL_TRC("Shared secret size: " << SharedSecret.size());
   const COLstring DerivedKey = GRIDSderiveKey(SharedSecret, COLstring(), COLstring("GRIDS-ENCRYPT-v1"));
   COL_TRC("Derived encryption key size: " << DerivedKey.size());
   COLstring             Ciphertext = Plaintext;
   const SFIaesEncryptor Encryptor((const unsigned char*)DerivedKey.c_str(), DerivedKey.size());
   TUNaesEncrypt(Encryptor, &Ciphertext);
   COL_TRC("Encrypted size: " << Ciphertext.size());
   return Ciphertext;
}

COLstring GRIDSdecrypt(const TUNkey& MyPrivateKey, const TUNkey& PeerPublicKey, const COLstring& Ciphertext) {
   COL_FUNCTION(GRIDSdecrypt);
   const COLstring SharedSecret = TUNdiffyHellmanSecret(MyPrivateKey, PeerPublicKey);
   COL_TRC("Shared secret size: " << SharedSecret.size() << ", ciphertext size: " << Ciphertext.size());
   const COLstring DerivedKey = GRIDSderiveKey(SharedSecret, COLstring(), COLstring("GRIDS-ENCRYPT-v1"));
   COL_TRC("Derived decryption key size: " << DerivedKey.size());
   const SFIaesDecryptor Decryptor((unsigned char*)DerivedKey.c_str(), DerivedKey.size());
   COLstring             Plaintext = Ciphertext;
   TUNaesDecrypt(Decryptor, &Plaintext);
   COL_TRC("Decrypted size: " << Plaintext.size());
   return Plaintext;
}

COLstring GRIDSgetPublicKeyPem(const TUNkey& Key) {
   COL_FUNCTION(GRIDSgetPublicKeyPem);
   return TUNkeyPublicAsPemString(Key);
}

void GRIDSloadPublicKeyPem(const COLstring& PemString, TUNkey* pKey) {
   COL_FUNCTION(GRIDSloadPublicKeyPem);
   TUNkeyPublicFromPemString(PemString, pKey);
}

COLstring GRIDSsign(const TUNkey& PrivateKey, const COLstring& Data) {
   COL_FUNCTION(GRIDSsign);
   EVP_MD_CTX* pContext = EVP_MD_CTX_create();
   if(!pContext) {
      COL_TRC("Failed to create MD context");
      return COLstring();
   }
   const EVP_MD* Digest = EVP_sha256();
   if(1 != EVP_DigestSignInit(pContext, NULL, Digest, NULL, PrivateKey.key)) {
      COL_TRC("Failed to initialize digest signing");
      EVP_MD_CTX_free(pContext);
      return COLstring();
   }
   if(1 != EVP_DigestSignUpdate(pContext, Data.c_str(), Data.size())) {
      COL_TRC("Failed to update digest");
      EVP_MD_CTX_free(pContext);
      return COLstring();
   }
   size_t SigLen = 0;
   if(1 != EVP_DigestSignFinal(pContext, NULL, &SigLen)) {
      COL_TRC("Failed to get signature length");
      EVP_MD_CTX_free(pContext);
      return COLstring();
   }
   COLstring Signature;
   Signature.setCapacity(SigLen);
   Signature.setSize(SigLen);
   if(1 != EVP_DigestSignFinal(pContext, (unsigned char*)Signature.get_buffer(), &SigLen)) {
      COL_TRC("Failed to finalize signature");
      EVP_MD_CTX_free(pContext);
      return COLstring();
   }
   Signature.setSize(SigLen);
   EVP_MD_CTX_free(pContext);
   COL_TRC("Signed " << Data.size() << " bytes, signature size: " << Signature.size());
   return Signature;
}

bool GRIDSverifySignature(const TUNkey& PublicKey, const COLstring& Data, const COLstring& Signature) {
   COL_FUNCTION(GRIDSverifySignature);
   EVP_MD_CTX* pContext = EVP_MD_CTX_create();
   if(!pContext) {
      COL_TRC("Failed to create MD context");
      return false;
   }
   const EVP_MD* Digest = EVP_sha256();
   if(1 != EVP_DigestVerifyInit(pContext, NULL, Digest, NULL, PublicKey.key)) {
      COL_TRC("Failed to initialize digest verification");
      EVP_MD_CTX_free(pContext);
      return false;
   }
   if(1 != EVP_DigestVerifyUpdate(pContext, Data.c_str(), Data.size())) {
      COL_TRC("Failed to update digest");
      EVP_MD_CTX_free(pContext);
      return false;
   }
   const int Result = EVP_DigestVerifyFinal(pContext, (const unsigned char*)Signature.c_str(), Signature.size());
   EVP_MD_CTX_free(pContext);
   if(Result == 1) {
      COL_TRC("Signature verification successful");
      return true;
   }
   COL_TRC("Signature verification failed");
   return false;
}

// Derives a 32-byte key using HKDF-SHA256 from a shared secret, optional salt, and optional context info (OpenSSL EVP)
// Useful for generating secure session or encryption keys from shared secrets.
COLstring GRIDSderiveKey(const COLstring& SharedSecret, const COLstring& Salt, const COLstring& Info) {
   COL_FUNCTION(GRIDSderiveKey);
   EVP_PKEY_CTX* pContext = EVP_PKEY_CTX_new_id(EVP_PKEY_HKDF, NULL);
   if(!pContext) {
      COL_TRC("Failed to create HKDF context");
      return COLstring();
   }
   if(1 != EVP_PKEY_derive_init(pContext)) {
      COL_TRC("Failed to initialize HKDF");
      EVP_PKEY_CTX_free(pContext);
      return COLstring();
   }
   if(1 != EVP_PKEY_CTX_set_hkdf_md(pContext, EVP_sha256())) {
      COL_TRC("Failed to set HKDF hash function");
      EVP_PKEY_CTX_free(pContext);
      return COLstring();
   }
   if(1 != EVP_PKEY_CTX_set1_hkdf_key(pContext, (const unsigned char*)SharedSecret.c_str(), SharedSecret.size())) {
      COL_TRC("Failed to set HKDF key");
      EVP_PKEY_CTX_free(pContext);
      return COLstring();
   }
   if(Salt.size() > 0) {
      if(1 != EVP_PKEY_CTX_set1_hkdf_salt(pContext, (const unsigned char*)Salt.c_str(), Salt.size())) {
         COL_TRC("Failed to set HKDF salt");
         EVP_PKEY_CTX_free(pContext);
         return COLstring();
      }
   }
   if(Info.size() > 0) {
      if(1 != EVP_PKEY_CTX_add1_hkdf_info(pContext, (const unsigned char*)Info.c_str(), Info.size())) {
         COL_TRC("Failed to set HKDF info");
         EVP_PKEY_CTX_free(pContext);
         return COLstring();
      }
   }
   const size_t OutLen = 32;
   COLstring    DerivedKey;
   DerivedKey.setCapacity(OutLen);
   DerivedKey.setSize(OutLen);
   size_t ActualLen = OutLen;
   if(1 != EVP_PKEY_derive(pContext, (unsigned char*)DerivedKey.get_buffer(), &ActualLen)) {
      COL_TRC("Failed to derive key");
      EVP_PKEY_CTX_free(pContext);
      return COLstring();
   }
   DerivedKey.setSize(ActualLen);
   EVP_PKEY_CTX_free(pContext);
   COL_TRC("Derived key size: " << DerivedKey.size());
   return DerivedKey;
}

// // Ensures message integrity and authenticity, detecting tampering by a man-in-the-middle without the shared secret key.
COLstring GRIDScomputeHMAC(const COLstring& Key, const COLstring& Data) {
   COL_FUNCTION(GRIDScomputeHMAC);
   EVP_MD_CTX* pContext = EVP_MD_CTX_create();
   if(!pContext) {
      COL_TRC("Failed to create MD context");
      return COLstring();
   }
   EVP_PKEY* pKey = EVP_PKEY_new_mac_key(EVP_PKEY_HMAC, NULL, (const unsigned char*)Key.c_str(), Key.size());
   if(!pKey) {
      COL_TRC("Failed to create HMAC key");
      EVP_MD_CTX_free(pContext);
      return COLstring();
   }
   const EVP_MD* Digest = EVP_sha256();
   if(1 != EVP_DigestSignInit(pContext, NULL, Digest, NULL, pKey)) {
      COL_TRC("Failed to initialize HMAC");
      EVP_PKEY_free(pKey);
      EVP_MD_CTX_free(pContext);
      return COLstring();
   }
   if(1 != EVP_DigestSignUpdate(pContext, Data.c_str(), Data.size())) {
      COL_TRC("Failed to update HMAC");
      EVP_PKEY_free(pKey);
      EVP_MD_CTX_free(pContext);
      return COLstring();
   }
   size_t MacLen = 0;
   if(1 != EVP_DigestSignFinal(pContext, NULL, &MacLen)) {
      COL_TRC("Failed to get HMAC length");
      EVP_PKEY_free(pKey);
      EVP_MD_CTX_free(pContext);
      return COLstring();
   }
   COLstring Mac;
   Mac.setCapacity(MacLen);
   Mac.setSize(MacLen);
   if(1 != EVP_DigestSignFinal(pContext, (unsigned char*)Mac.get_buffer(), &MacLen)) {
      COL_TRC("Failed to finalize HMAC");
      EVP_PKEY_free(pKey);
      EVP_MD_CTX_free(pContext);
      return COLstring();
   }
   Mac.setSize(MacLen);
   EVP_PKEY_free(pKey);
   EVP_MD_CTX_free(pContext);
   COL_TRC("Computed HMAC size: " << Mac.size());
   return Mac;
}
