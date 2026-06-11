// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNchallenge
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Monday 26 June 2023 - 10:04AM
// ---------------------------------------------------------------------------

#include <TUN/TUNchallenge.h>
#include <TUN/TUNiv.h>
#include <TUN/TUNdiffyHellmanSecret.h>

#include <SFI/SFIaesEncryptor.h>
#include <SFI/SFIaesDecryptor.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void TUNchallenge(const TUNkey& MyPrivateKey, const TUNkey& PeerPublicKey, COLstring* pSharedSecret, COLstring* pRandomString, COLstring* pEncryptedString){
   COL_FUNCTION(TUNchallenge);
   *pSharedSecret = TUNdiffyHellmanSecret(MyPrivateKey, PeerPublicKey);
   TUNiv(pRandomString, 16);
   SFIaesEncryptor Encrpt((const unsigned char*)pSharedSecret->c_str(), pSharedSecret->size());
   *pEncryptedString = *pRandomString;
   Encrpt.ProcessBlock((const unsigned char*)pRandomString->c_str(), (unsigned char*)pEncryptedString->c_str());
   COL_DUMP("Encrypted String:", pEncryptedString->c_str(), pEncryptedString->size());
}

COLstring TUNchallengeResponse(const TUNkey& MyPrivateKey, const TUNkey& PeerPublicKey, const COLstring& EncryptedString){
   COL_FUNCTION(TUNchallengeResponse);
   COLstring SharedSecret = TUNdiffyHellmanSecret(MyPrivateKey, PeerPublicKey);
   SFIaesDecryptor Decrpytor((unsigned char*)SharedSecret.c_str(), SharedSecret.size());
   COLstring RandomString;
   RandomString.setCapacity(16);
   RandomString.setSize(16);
   Decrpytor.ProcessBlock((const unsigned char*)EncryptedString.c_str(), (unsigned char*)RandomString.get_buffer());
   return RandomString;
}