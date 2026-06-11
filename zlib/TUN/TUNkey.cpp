// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNkey
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Tuesday 13 June 2023 - 10:30AM
// ---------------------------------------------------------------------------

#include <TUN/TUNkey.h>

#include <openssl/evp.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

TUNkeyContext::~TUNkeyContext(){
   COL_METHOD(TUNkeyContext::~TUNkeyContext);
   EVP_PKEY_CTX_free(pContext);
}

TUNkey::~TUNkey(){
   COL_METHOD(TUNkey::~TUNkey);
   EVP_PKEY_free(key);
}
