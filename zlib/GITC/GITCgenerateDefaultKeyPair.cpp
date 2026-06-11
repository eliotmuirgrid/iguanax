// ---------------------------------------------------------------------------
//  Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GITCgenerateDefaultKeyPair
//
//  Description
//
//  Author: Matthew Sobkowski
//  Date:   10/19/22 12:45 PM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <FIL/FILutils.h>
#include <GITU/GITUconfigGlobal.h>
#include <GIT/GITgenerateKeyPair.h>
#include <GITC/GITCgenerateDefaultKeyPair.h>
COL_LOG_MODULE;

void GITCgenerateDefaultKeyPairImpl(const COLstring& Username, const COLstring& KeyType, const COLaddress& Address){
   COL_FUNCTION(GITCgenerateDefaultKeyPairImpl);
   COLvar Result;
   COLstring PrivateKey     = GITUconfigPrivateKey(Username);
   COLstring PrivateKeyFile = GITUstripPathsQuotes(PrivateKey);
   COL_VAR(PrivateKey);
   if(FILfileExists(PrivateKeyFile)){
      COL_TRC("Remove old private key");
      FILremoveWithThrow(PrivateKeyFile);
   }
   COLstring Output;
   if (!GITgenerateKeyPair(PrivateKey, &Output, KeyType)) {
      return COLrespondError(Address, Output);
   }
   Result["private_key"] = PrivateKeyFile;
   Result["info"] = "Generated a new ssh key.  You will need to register it.";
   return COLrespondSuccess(Address, Result);
}

// /git/generate_default_key_pair
void GITCgenerateDefaultKeyPair(const COLwebRequest& Request){
   COL_FUNCTION(GITCgenerateDefaultKeyPair);
   const COLvar& Data = Request.Data;
   COLstring Username = Request.User;
   COLstring KeyType = COLvarGetString(Data, "key_type", "ed25519");
   GITCgenerateDefaultKeyPairImpl(Username, KeyType, Request.Address);
}
