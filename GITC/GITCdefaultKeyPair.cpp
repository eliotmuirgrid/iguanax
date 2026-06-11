// ---------------------------------------------------------------------------
//  Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GITCdefaultKeyPair
//
//  Description
//
// Author: John Q
// Date:   Tue  6 Dec 2022 14:55:12 EST
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLsplit.h>
#include <COL/COLvar.h>
#include <COL/COLweb.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <GITU/GITUconfigGlobal.h>
#include <GITC/GITCdefaultKeyPair.h>
COL_LOG_MODULE;

static void GITCdetermineKeyAlogrithm(const COLstring& Content, COLvar* pOut){
   COL_FUNCTION(GITCdetermineKeyAlogrithm);
   COLarray<COLstring> Parts;
   COLsplit(&Parts, Content, " "); // Parts[0] holds the algorithm type, ex ssh-rsa
   if (Parts.size() > 0) {
      COLstring Algorithm, Other;
      if (Parts[0].find("ssh-") != npos) { 
         Parts[0].split(Other, Algorithm, "-");
         *pOut = Algorithm;
      } else {
         *pOut = "Could not determine key algorithm";
      }
   } else { 
      *pOut = "Could not determine key algorithm"; 
   }
}

void GITCdefaultKeyPairImpl(const COLstring& Username, const COLaddress& Address) {
   COL_FUNCTION(GITCdefaultKeyPairImpl);
   const COLstring PrivateKey     = GITUconfigPrivateKey(Username);
   const COLstring PrivateKeyFile = GITUstripPathsQuotes(PrivateKey);
   const COLstring PublicKey      = GITUconfigPublicKey(PrivateKeyFile);
   COLvar          Data;
   Data["private_key"]        = PrivateKeyFile;
   Data["public_key"]         = PublicKey;
   Data["private_key_exists"] = FILfileExists(PrivateKeyFile);
   Data["public_key_exists"]  = FILfileExists(PublicKey);
   if(Data["public_key_exists"]) {
      try {
         COLstring Content;
         FILreadFile(PublicKey, &Content);
         Data["public_key_content"] = Content;
         GITCdetermineKeyAlogrithm(Content, &Data["public_key_algorithm"]);
      } catch(const COLerror& E) { COL_TRC(E.description()); }
   }
   COLrespondSuccess(Address, Data);
}

// /git/private_key_path
void GITCdefaultKeyPair(const COLwebRequest& Request) {
   COL_FUNCTION(GITCdefaultKeyPair);
   COLstring Username = Request.User;
   COLvar Result;
   GITCdefaultKeyPairImpl(Username, Request.Address);
}
