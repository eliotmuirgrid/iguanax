#ifndef __GIT_DEFAULT_KEY_PAIR_H__
#define __GIT_DEFAULT_KEY_PAIR_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITCdefaultKeyPair
//
// Description: fetch default ssh key pair
//
// Author: John Q
// Date:   Tue  6 Dec 2022 14:55:12 EST
//---------------------------------------------------------------------------
class COLstring;
class COLwebRequest;

// COLstring GITCkeyDirectory();
// COLstring GITCfileNamePublicKey();
// COLstring GITCfileNamePrivateKey();

void GITCdefaultKeyPair(const COLwebRequest& Request);

#endif // end of defensive include