// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNdiffyHellmanSecret
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Tuesday 13 June 2023 - 10:55AM
// ---------------------------------------------------------------------------

#include <TUN/TUNdiffyHellmanSecret.h>
#include <TUN/TUNkey.h>
#include <TUN/TUNerror.h>

#include <openssl/evp.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

COLstring TUNdiffyHellmanSecret(const TUNkey& my_key, const TUNkey& peer_key) {
   COL_FUNCTION(TUNdiffyHellmanSecret);
   TUNkeyContext KeyContext;
   size_t secret_len;
   if(NULL == (KeyContext.pContext = EVP_PKEY_CTX_new(my_key.key, NULL)))       {TUNthrowError();} COL_TRC("Create the context for the shared secret derivation");
   if(1 != EVP_PKEY_derive_init    (KeyContext.pContext))                       {TUNthrowError();} COL_TRC("Initialise the shared secret derivation");
   if(1 != EVP_PKEY_derive_set_peer(KeyContext.pContext, peer_key.key))         {TUNthrowError();} COL_TRC("Provide the peer public key"); 
   if(1 != EVP_PKEY_derive         (KeyContext.pContext, NULL, &secret_len))    {TUNthrowError();} COL_TRC("Find the buffer size");
   COLstring Result;
   Result.setCapacity(secret_len);
   if(1 != EVP_PKEY_derive(KeyContext.pContext, (unsigned char*)Result.c_str(), &secret_len)) {TUNthrowError();} // Create the shared secret 
   Result.setSize(secret_len);
   //COL_VAR(Result);
   return Result;
}
