// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNkeyEllipticalGen
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Tuesday 13 June 2023 - 10:38AM
// ---------------------------------------------------------------------------

#include <TUN/TUNkeyEllipticalGen.h>
#include <TUN/TUNkey.h>
#include <TUN/TUNerror.h>

#include <openssl/evp.h>
#include <openssl/ec.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void TUNgenerateEllipticalKey(TUNkey* pKey){
   TUNkeyContext KeyContext;
   TUNkeyContext ParamsContext;
   TUNkey Params;
   if(NULL == (KeyContext.pContext = EVP_PKEY_CTX_new_id(EVP_PKEY_EC, NULL)))           {TUNthrowError();} // Create a context for the parameter generation
   if (EVP_PKEY_paramgen_init                (KeyContext.pContext) <= 0)                {TUNthrowError();} // Initialize the parameter generation
   if (EVP_PKEY_CTX_set_ec_paramgen_curve_nid(KeyContext.pContext, NID_secp256k1) <= 0) {TUNthrowError();} // Set the curve for parameter generation
   if (!EVP_PKEY_paramgen                    (KeyContext.pContext, &Params.key))        {TUNthrowError();} // Create the parameter object params   
   if(NULL == (ParamsContext.pContext = EVP_PKEY_CTX_new(Params.key, NULL)))            {TUNthrowError();} // Create the context for the key generation
   if(EVP_PKEY_keygen_init(ParamsContext.pContext) <= 0)           { TUNthrowError(); } // init context for key generation.
   if(EVP_PKEY_keygen     (ParamsContext.pContext, &pKey->key) <= 0) { TUNthrowError(); } // Generate the key
}
