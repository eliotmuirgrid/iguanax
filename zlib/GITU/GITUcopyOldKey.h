#ifndef __GITU_COPY_OLD_KEY_H__
#define __GITU_COPY_OLD_KEY_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITUcopyOldKey
//
// Description:
//
// TODO
//
// Author: Tyler Brown 
// Date:   Thursday 16 November 2023 - 12:52PM
// ---------------------------------------------------------------------------

class COLstring;

bool GITUwritePrivateKey(const COLstring& PrivateKeyPath, const COLstring& PrivateKeyData, COLstring* pOut);
bool GITUwritePublicKey (const COLstring& PrivateKeyPath, const COLstring& PublicKeyData, COLstring* pOut);

void GITUcopyOldKey(const COLstring& NewKeyPrivate);

#endif // end of defensive include
