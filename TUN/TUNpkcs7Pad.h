#ifndef __TUN_PKCS7_PAD_H__
#define __TUN_PKCS7_PAD_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNpkcs7Padd
//
// Description:
//
// PKCS#7 padding makes sure we have a round value of 16 bytes for AES encryption
//
// Author: Eliot Muir 
// Date:   Friday 16 June 2023 - 08:35AM
// ---------------------------------------------------------------------------

class COLstring;

void TUNpkcs7Pad(COLstring* pString);

void TUNpkcs7Unpad(COLstring* pString);

#endif // end of defensive include
