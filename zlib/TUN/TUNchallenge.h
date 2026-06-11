#ifndef __TUN_CHALLENGE_H__
#define __TUN_CHALLENGE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNchallenge
//
// Description:
//
// Calculate the shared secret between two keys, make a random string and
// encrypt it as a challenge.
//
// Author: Eliot Muir 
// Date:   Monday 26 June 2023 - 10:04AM
// ---------------------------------------------------------------------------

class TUNkey;
class COLstring;

void TUNchallenge(const TUNkey& MyPrivateKey, const TUNkey& PeerPublicKey, COLstring* pSharedSecret, COLstring* pRandomString, COLstring* pEncryptedString);

COLstring TUNchallengeResponse(const TUNkey& MyPrivateKey, const TUNkey& PeerPublicKey, const COLstring& EncryptedString);

#endif // end of defensive include