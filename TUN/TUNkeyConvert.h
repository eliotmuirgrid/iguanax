#ifndef __TUN_KEY_CONVERT_H__
#define __TUN_KEY_CONVERT_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNkeyConvert
//
// Description:
//
// Converts the keys into their PEM string format.
//
// Author: Eliot Muir 
// Date:   Tuesday 13 June 2023 - 10:38AM
// ---------------------------------------------------------------------------

class TUNkey;
class COLstring;

COLstring TUNkeyPrivateAsPemString(const TUNkey& Key);
COLstring TUNkeyPublicAsPemString(const TUNkey& Key);

void TUNkeyPublicFromPemString(const COLstring& KeyInPemString, TUNkey* pKey);
void TUNkeyPrivateFromPemString(const COLstring& pem_key, TUNkey* pKey);

COLstring TUNkeyPublicAsDer(const TUNkey& Key);
COLstring TUNkeyPrivateAsDer(const TUNkey& Key);

void TUNkeyPublicFromDer(const COLstring& DerKey, TUNkey* pKey);
void TUNkeyPrivateFromDer(const COLstring& DerKey, TUNkey* pKey);


#endif // end of defensive include
