#ifndef __SFI_AES_ENCRYPTOR_H__
#define __SFI_AES_ENCRYPTOR_H__

//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2006 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SFIaesEncryptor
//
// Description:
//
// AES Encryptors implementation.  Based on 
// http://www.esat.kuleuven.ac.be/~rijmen/rijndael/
//
// Copied from CRYaesEncryptor.
//
// Author: Rob Moyse
// Date: November 29, 2004 
//
//
//---------------------------------------------------------------------------

#include <SFI/SFIaes.h>

class SFIaesEncryptor : public SFIaes
{
public:
   SFIaesEncryptor(const COLuint8 *pUserKey, COLuint32 KeyLength = 16)
      : SFIaes(pUserKey, KeyLength) {}

   void ProcessBlock(const COLuint8 *pInBlock, COLuint8 *pOutBlock) const;

};

#endif // end of defensive include
