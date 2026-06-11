// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testOpenSsl
//
// Description:
//
// Implementation of tests
//
// Author: Eliot Muir 
// Date:   Wednesday 14 June 2023 - 08:44AM
// ---------------------------------------------------------------------------
#include "testOpenSsl.h"

#include <UNIT/UNITapp.h>

#include <TUN/TUNstringBio.h>
#include <TUN/TUNkey.h>
#include <TUN/TUNerror.h>
#include <TUN/TUNkeyEllipticalGen.h>
#include <TUN/TUNkeyConvert.h>
#include <TUN/TUNdiffyHellmanSecret.h>
#include <TUN/TUNiv.h>

#include <SFI/SFIbase64EncodeFilter.h>
#include <SFI/SFIbase64DecodeFilter.h>
#include <SFI/SFIaesEncryptor.h>
#include <SFI/SFIaesDecryptor.h>

#include <openssl/aes.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;



void TUNtest(){
   COL_FUNCTION(TUNtest);
   TUNkey Key1, Key2;
   TUNgenerateEllipticalKey(&Key1);
   TUNgenerateEllipticalKey(&Key2);
   COLcout << "Private Key:" << newline << TUNkeyPrivateAsPemString(Key1);
   COLcout << "Public Key:" << newline << TUNkeyPublicAsPemString(Key1);
   COLcout << "Private Key:" << newline << TUNkeyPrivateAsPemString(Key2);
   COLcout << "Public Key:" << newline << TUNkeyPublicAsPemString(Key2);


   COLstring Key1PublicDer = TUNkeyPublicAsDer(Key1);
   COL_DUMP("Key1PublicDer", Key1PublicDer.c_str(), Key1PublicDer.size());
   COLstring Key2PrivateDer = TUNkeyPrivateAsDer(Key2);
   COL_DUMP("Key2PrivateDer", Key2PrivateDer.c_str(), Key2PrivateDer.size());
   Key2PrivateDer = SFIbase64Encode(Key2PrivateDer);
   COL_VAR(Key2PrivateDer);
   COLstring Back;
   SFIbase64Decode(Key2PrivateDer, &Back);
   COL_DUMP("Key2PrivateDer", Back.c_str(), Back.size());


   TUNkey PeerKey1;
   TUNkeyPublicFromPemString(TUNkeyPublicAsPemString(Key1), &PeerKey1);
   TUNkey PeerKey2;
   TUNkeyPublicFromDer(TUNkeyPublicAsDer(Key2), &PeerKey2);

   TUNkey PrivateKey1;
   TUNkeyPrivateFromPemString(TUNkeyPrivateAsPemString(Key1), &PrivateKey1);
   TUNkey PrivateKey2;
   TUNkeyPrivateFromDer(TUNkeyPrivateAsDer(Key2), &PrivateKey2);

   COLstring Secret1 = TUNdiffyHellmanSecret(PrivateKey1, PeerKey2);
   COLstring Secret2 = TUNdiffyHellmanSecret(PrivateKey2, PeerKey1);
   
   COL_DUMP("Secret1", Secret1.c_str(), Secret1.size());
   COL_DUMP("Secret2", Secret2.c_str(), Secret2.size());  

   COLstring Input = "0123456789012345";
   COLstring Output;
   Output.setCapacity(16);
   Output.setSize(16);
   
   SFIaesEncryptor Encrypt((const unsigned char*)Secret1.c_str(), Secret1.size());
   Encrypt.ProcessBlock((const unsigned char*)Input.c_str(),(unsigned char*)Output.c_str());
   COL_DUMP("Output", Output.c_str(), Output.size());

   COLstring Out;
   Out.setCapacity(16);
   Out.setSize(16);
   SFIaesDecryptor Decrypt((const unsigned char*)Secret1.c_str(), Secret1.size());
   Decrypt.ProcessBlock((unsigned char*)Output.c_str(), (unsigned char*)Out.c_str());
   COL_DUMP("Out", Out.c_str(), Out.size());

   COLstring IV;
   TUNiv(&IV, 16);
   COL_DUMP("IV", IV.c_str(), IV.size());
}

static void testKeyGeneration(){
   COL_FUNCTION(testKeyGeneration);
   TUNkey Key;
   TUNgenerateEllipticalKey(&Key);
   COL_TRC("Private Key:" << newline << TUNkeyPrivateAsPemString(Key));
   COL_TRC("Public  Key:" << newline << TUNkeyPublicAsPemString(Key)); 
}

void testOpenSsl(UNITapp* pApp) {
   pApp->add("key/generation", &testKeyGeneration);
}
