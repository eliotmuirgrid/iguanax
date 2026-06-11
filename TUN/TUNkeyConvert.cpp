// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNkeyConvert
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Tuesday 13 June 2023 - 10:38AM
// ---------------------------------------------------------------------------

#include <TUN/TUNkeyConvert.h>
#include <TUN/TUNkey.h>
#include <TUN/TUNstringBio.h>
#include <TUN/TUNerror.h>

#include <openssl/pem.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

COLstring TUNkeyPrivateAsPemString(const TUNkey& Key){
   COL_FUNCTION(TUNkeyPrivateAsPemString);
   COLstring Result;
   TUNstringBio Bio(&Result);
   if (PEM_write_bio_PrivateKey(Bio.m_pBio, Key.key, NULL, NULL, 0, NULL, NULL) != 1) { TUNthrowError();}
   return Result;
}

COLstring TUNkeyPublicAsPemString(const TUNkey& Key){
   COL_FUNCTION(TUNkeyPublicAsPemString);
   COLstring Result;
   TUNstringBio Bio(&Result);
   if (PEM_write_bio_PUBKEY(Bio.m_pBio, Key.key) != 1) { TUNthrowError();}
   return Result;
}

void TUNkeyPublicFromPemString(const COLstring& pem_key, TUNkey* pKey) {
   COL_FUNCTION(TUNkeyPublicFromPemString);
   TUNstringReadBio Bio(pem_key);
   if (NULL == (pKey->key = PEM_read_bio_PUBKEY(Bio.m_pBio, NULL, NULL, NULL))) { TUNthrowError();}
}

void TUNkeyPrivateFromPemString(const COLstring& pem_key, TUNkey* pKey) {
   COL_FUNCTION(TUNkeyPrivateFromPemString);
   COL_VAR(pem_key);
   TUNstringReadBio Bio(pem_key);
   if (NULL == (pKey->key = PEM_read_bio_PrivateKey(Bio.m_pBio, NULL, NULL, NULL))) { TUNthrowError();}
}

COLstring TUNkeyPublicAsDer(const TUNkey& Key){
   COL_FUNCTION(TUNkeyPublicAsDer);
   COLstring Result;
   TUNstringBio Bio(&Result);
   if (i2d_PUBKEY_bio(Bio.m_pBio, Key.key) <= 0){
      TUNthrowError();
   }
   return Result;
}

COLstring TUNkeyPrivateAsDer(const TUNkey& Key){
   COL_FUNCTION(TUNkeyPublicAsDer);
   COLstring Result;
   TUNstringBio Bio(&Result);
   if (i2d_PrivateKey_bio(Bio.m_pBio, Key.key) <= 0){
      TUNthrowError();
   }
   return Result;
}

void TUNkeyPublicFromDer(const COLstring& DerKey, TUNkey* pKey){
   COL_FUNCTION(TUNkeyPublicFromDer);
   const unsigned char* pDer = (const unsigned char*)DerKey.data();
   pKey->key = d2i_PUBKEY(NULL, &pDer, DerKey.size());
   if (pKey->key == NULL){
      TUNthrowError();
   }
}

void TUNkeyPrivateFromDer(const COLstring& DerKey, TUNkey* pKey){
   COL_FUNCTION(TUNkeyPrivateFromDer);
   TUNstringReadBio Bio(DerKey);
   pKey->key = d2i_PrivateKey_bio(Bio.m_pBio, NULL);
   if (pKey->key == NULL){
      TUNthrowError();
   }
}