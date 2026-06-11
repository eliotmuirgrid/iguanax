#ifndef __TUN_DIFFY_HELLMAN_SECRET_H__
#define __TUN_DIFFY_HELLMAN_SECRET_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNdiffyHellmanSecret
//
// Description:
//
// Use the diffy helman algorithm to calculate a shared secret.  This
// works with
//
// Author: Eliot Muir 
// Date:   Tuesday 13 June 2023 - 10:55AM
// ---------------------------------------------------------------------------

class TUNkey;
class COLstring;

COLstring TUNdiffyHellmanSecret(const TUNkey& MyPrivateKey, const TUNkey& PeerPublicKey);

#endif // end of defensive include
