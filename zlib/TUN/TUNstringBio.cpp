// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNstringBio
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Monday 12 June 2023 - 03:30PM
// ---------------------------------------------------------------------------

#include <TUN/TUNstringBio.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <openssl/bio.h>

static int TUNwriteString(BIO* b, const char *in, int inl) {
   COLstring* pString = (COLstring *)BIO_get_data(b);
   pString->write(in, inl);
   return inl;
}

TUNstringBio::TUNstringBio(COLstring* pString){
   COL_METHOD(TUNstringBio::TUNstringBio);
   m_pBioMethod = BIO_meth_new(BIO_TYPE_SOURCE_SINK, "COLstring bio");
   BIO_meth_set_write(m_pBioMethod, &TUNwriteString);
   m_pBio       = BIO_new(m_pBioMethod);
   // Attach your custom object to the BIO
   BIO_set_data(m_pBio, pString);
}

TUNstringBio::~TUNstringBio(){
   COL_METHOD(TUNstringBio::~TUNstringBio);
   BIO_free(m_pBio);
   BIO_meth_free(m_pBioMethod);
}

TUNstringReadBio::TUNstringReadBio(const COLstring& String){
   COL_METHOD(TUNstringReadBio::TUNstringReadBio);
   m_pBio = BIO_new_mem_buf(String.c_str(), String.size());
}

TUNstringReadBio::~TUNstringReadBio(){
   COL_METHOD(TUNstringReadBio::~TUNstringReadBio);
   BIO_free(m_pBio);
}
