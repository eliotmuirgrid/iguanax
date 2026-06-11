// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITCsetSshPair
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Thursday 04 December 2025 - 01:12PM
// ---------------------------------------------------------------------------

#include <GITC/GITCsetSshPair.h>

#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
COL_LOG_MODULE;

static void GITCsetSshPairImpl(const COLstring& Username, const COLstring& PublicKey, const COLstring& PrivateKey, const COLaddress& Address) {
   COL_FUNCTION(GITCsetSshPairImpl);
   COL_VAR3(Username, PublicKey, PrivateKey);
   // COLstring PrivateKeyPath = ;

}
// /git/ssh/pair/set
void GITCsetSshPair(const COLwebRequest& Request) {
   COL_FUNCTION(GITCsetSshPair);
   const COLvar& Data = Request.Data;
   COL_VAR(Data);
   COLstring Username = Request.User;
   COLstring PublicKey = COLvarRequiredString(Data, "public");
   COLstring PrivateKey = COLvarRequiredString(Data, "private");
   GITCsetSshPairImpl(Username, PublicKey, PrivateKey, Request.Address);
}

// COL_FUNCTION(GITUcopyOldKey);
// COL_VAR(NewKeyPrivate);
// COLstring SSHdir = FILpathAppend(FILexpand("~"), ".ssh");
// COLstring OldKeyPrivate;
// COLstring OldKeyPublic;
// for (auto i = 0; i < sizeof(GITCdefaultSSHkeyName)/sizeof(GITCdefaultSSHkeyName[0]); i++) {
//    COLstring PrivateKey = FILpathAppend(SSHdir, GITCdefaultSSHkeyName[i]);
//    COLstring PublicKey  = GITUconfigPublicKey(PrivateKey);
//    if (FILfileExists(PrivateKey) && FILfileExists(PublicKey)) {
//       OldKeyPrivate = PrivateKey;
//       OldKeyPublic  = PublicKey;
//       break;
//    }
// }
// if(OldKeyPrivate.is_null() || OldKeyPublic.is_null()){
//    COL_TRC("Could not find an ssh file to copy");
//    return;
// }
// FILcreateDirectoryTreeToFile(NewKeyPrivate, 0);
// GITUcopyPrivateKey(OldKeyPrivate, NewKeyPrivate);
// COLstring NewKeyPublic = GITUconfigPublicKey(NewKeyPrivate);
// FILremove(NewKeyPublic);
// FILcopyFile(OldKeyPublic,  NewKeyPublic);