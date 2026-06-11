#ifndef __GIT_GENERATE_KEY_PAIR_H__
#define __GIT_GENERATE_KEY_PAIR_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITgenerateKeyPair
//
// Description:
//
// Generates an ssh key pair using ssh-keygen
//
// Author: Tyler Brown 
// Date:   Wednesday 22 November 2023 - 01:37PM
// ---------------------------------------------------------------------------

#include <COL/COLstring.h>

COLstring GITCkeyTypeOverride();
bool GITgenerateKeyPair(const COLstring& PrivateKey, COLstring* pOutput, const COLstring& KeyType="ed25519");


#endif // end of defensive include
