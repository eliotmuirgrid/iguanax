#ifndef __DRX_SERIALIZATION_H__
#define __DRX_SERIALIZATION_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DRXinformation
//
// Description:
//
// A couple of functions to convert a license TREe object into an encrypted
// license string, and vice versa.
//
// The license instance is serialized into the format,
// member1_name:member1_value;member2_name:member2_value;
// and then encrypted using DRXfilterEncrypt.
//
// If there have been any changes to the implementation of the license object,
// such as when members are added/removed, the license version should change
// to prevent any unexpected results when different license versions are
// decrypted.
//
// The checks embedded into this license scheme are: an MD5 hash of the 
// license version is placed in front of the license data to verify 
// the version during decryption, and an MD5 hash of the entire license 
// including the license version hash is placed in front of the entire
// license. The hash of the entire license detects possible corruption 
// anywhere in the license key.
//
// Author: Henry Tran
// Date: July 27, 2004
//
//
//---------------------------------------------------------------------------


class COLstring;
class DRXinformation;

void DRXserializeLicense(const DRXinformation& License, const COLstring& IguanaId, COLstring* pOut);
void DRXparseLicense(const COLstring& License, const COLstring& IguanaId, DRXinformation* pOut);

// Exception error code indicating that a license was created for a different
// Iguana instance
static const int DRX_IGUANA_ID_MISMATCH = 100;

#endif // end of defensive include
