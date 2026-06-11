#ifndef __SFI_AES_H__
#define __SFI_AES_H__

//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2007 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SFIaes
//
// Description:
//
// Base class for AES encryptors and decryptors.  Based on 
// http://www.esat.kuleuven.ac.be/~rijmen/rijndael/
//
// Copied from CRYaes.
//
// Author: Rob Moyse
// Date: November 29, 2004 
//
//
//---------------------------------------------------------------------------

#include <SFI/SFImisc.h>


class SFIaesPrivate;

class SFIaes
{
public:
      static const COLuint32 SFI_AES_BLOCK_SIZE;
protected:

   SFIaes(const COLuint8 *pUserKey, COLuint32 KeyLength);
   virtual ~SFIaes();
   static COLuint32 keyLength(COLuint32 KeyLength);


   static const COLuint32* Te0();
   static const COLuint32* Te1();
   static const COLuint32* Te2();
   static const COLuint32* Te3();
   static const COLuint32* Te4();

   static const COLuint32* Td0();
   static const COLuint32* Td1();
   static const COLuint32* Td2();
   static const COLuint32* Td3();
   static const COLuint32* Td4();

   static const COLuint32* rcon();

   COLuint32 rounds() const;
   SecBlock<COLuint32>& key() const;

private:
   SFIaesPrivate* pMember;
};

#endif // end of defensive include
