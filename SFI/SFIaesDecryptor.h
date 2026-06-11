#ifndef __SFI_AES_DECRYPTOR_H__
#define __SFI_AES_DECRYPTOR_H__

//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2006 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SFIaesDecryptor
//
// Description:
//
// AES Decryptor implementation.  Based on 
// http://www.esat.kuleuven.ac.be/~rijmen/rijndael/
//
// Copied from CRYaesDecryptor.
//
// Author: Rob Moyse
// Date: November 29, 2004 
//
//
//---------------------------------------------------------------------------

#include <SFI/SFIaes.h>

class SFIaesDecryptor : public SFIaes
{
public:
   SFIaesDecryptor(const COLuint8* pUserKey, COLuint32 KeyLength=16);

   void ProcessBlock(const COLuint8 *pInBlock, COLuint8 *pOutBlock) const;

};

#endif // end of defensive include
