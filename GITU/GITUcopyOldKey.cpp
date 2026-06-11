// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITUcopyOldKey
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Thursday 16 November 2023 - 12:52PM
// ---------------------------------------------------------------------------

#include <COL/COLlog.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <GITU/GITUconfigGlobal.h>
#include <GITU/GITUcopyOldKey.h>
#include <GIT/GITgenerateKeyPair.h>
COL_LOG_MODULE;


const char* const GITCdefaultSSHkeyName[] = {"id_ed25519", "id_rsa", "id_dsa", "id_cdsa", "id_ecdsa_sk", "id_ed25519_sk"};

// Similar to GITUcopyPrivateKey but working on strings directly.
bool GITUwritePrivateKey(const COLstring& PrivateKeyPath, const COLstring& PrivateKeyData, COLstring* pOut){
   COL_FUNCTION(GITUwritePrivateKey);
   if (FILfileExists(PrivateKeyPath)) { 
      try {
         FILremove(PrivateKeyPath);
         COL_TRC("Removed existing key file");
      } catch(const COLerror& E) {
         COL_TRC("Failed to remove");
         COL_TRC(E.description());
      }
   }
   // We have to do this since on windows there are file permission issues
   // - the private key must have tight permissions or else it is rejected 
   //   so we generate a new key and modify the contents of it to the current key
   const bool Success = GITgenerateKeyPair(PrivateKeyPath, pOut);
   if(!Success) {
      COL_TRC(*pOut);
      return false;
   }
   try {
      FILwriteFile(PrivateKeyPath, PrivateKeyData);
   } catch(const COLerror& E) {
      COL_TRC(E.description());
      pOut->append(E.description());
      return false;
   }
   return true;
}

// Call after GITUwritePrivateKey
bool GITUwritePublicKey(const COLstring& PrivateKeyPath, const COLstring& PublicKeyData, COLstring* pOut){
   COL_FUNCTION(GITUwritePublicKey);
   const COLstring PublicKeyPath = GITUconfigPublicKey(PrivateKeyPath);
   try {
      FILremove(PublicKeyPath);
      FILwriteFile(PublicKeyPath, PublicKeyData);
   } catch(const COLerror& E) {
      COL_TRC(E.description());
      pOut->append(E.description());
      return false;
   }
   return true;
}

void GITUcopyPrivateKey(const COLstring& OldPrivateKey, const COLstring& NewPrivateKey){
   COL_FUNCTION(GITUcopyPrivateKey);
   COLstring Output;
   // We have to do this since on windows there are file permission issues
   // - the private key must have tight permissions or else it is rejected 
   //   so we generate a new key and modify the contents of it to the current key
   const bool Success = GITgenerateKeyPair(NewPrivateKey, &Output);
   if(!Success) {
      COL_TRC(Output);
      return;
   }
   try {
      COLstring Content;
      FILreadFile(OldPrivateKey, &Content);
      FILwriteFile(NewPrivateKey, Content);
   } catch(const COLerror& E) {
      COL_TRC(E.description());
   }
}

void GITUcopyOldKey(const COLstring& NewKeyPrivate){
   COL_FUNCTION(GITUcopyOldKey);
   COL_VAR(NewKeyPrivate);
   COLstring SSHdir = FILpathAppend(FILexpand("~"), ".ssh");
   COLstring OldKeyPrivate;
   COLstring OldKeyPublic;
   for (auto i = 0; i < sizeof(GITCdefaultSSHkeyName)/sizeof(GITCdefaultSSHkeyName[0]); i++) {
      COLstring PrivateKey = FILpathAppend(SSHdir, GITCdefaultSSHkeyName[i]);
      COLstring PublicKey  = GITUconfigPublicKey(PrivateKey);
      if (FILfileExists(PrivateKey) && FILfileExists(PublicKey)) {
         OldKeyPrivate = PrivateKey;
         OldKeyPublic  = PublicKey;
         break;
      }
   }
   if(OldKeyPrivate.is_null() || OldKeyPublic.is_null()){
      COL_TRC("Could not find an ssh file to copy");
      return;
   }
   FILcreateDirectoryTreeToFile(NewKeyPrivate, 0);
   GITUcopyPrivateKey(OldKeyPrivate, NewKeyPrivate);
   COLstring NewKeyPublic = GITUconfigPublicKey(NewKeyPrivate);
   FILremove(NewKeyPublic);
   FILcopyFile(OldKeyPublic,  NewKeyPublic);
}