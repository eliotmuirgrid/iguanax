//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SFIaesEncryptor
//
// Description:
//
// AES Encryptors implementation.  Based on 
// http://www.esat.kuleuven.ac.be/~rijmen/rijndael/
//
// Author: Rob Moyse
// Date: November 29, 2004 
//
//---------------------------------------------------------------------------
#include "SFIprecomp.h"
#pragma hdrstop

#include <SFI/SFImisc.h>
#include <SFI/SFIaesEncryptor.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void SFIaesEncryptor::ProcessBlock(const COLuint8 *pInBlock, COLuint8 *pOutBlock) const{
   COL_METHOD(SFIaesEncryptor::ProcessBlock);
   COLuint32 s0, s1, s2, s3, t0, t1, t2, t3;
   const COLuint32 *prk = key();
   COL_VAR(*prk);
   COL_VAR(key().size);
   COL_DUMP("Key:", key().ptr, key().size);

    /*
    * map byte array block to cipher state
    * and add initial round key:
    */
   GetBlockBigEndian(pInBlock, s0, s1, s2, s3);
   s0 ^= prk[0];
   s1 ^= prk[1];
   s2 ^= prk[2];
   s3 ^= prk[3];
    /*
    * Nr - 1 full rounds:
    */
    unsigned int r = rounds() >> 1;
    for (;;) {
        t0 =
            Te0()[(s0 >> 24)       ] ^
            Te1()[(s1 >> 16) & 0xff] ^
            Te2()[(s2 >>  8) & 0xff] ^
            Te3()[(s3      ) & 0xff] ^
            prk[4];
        t1 =
            Te0()[(s1 >> 24)       ] ^
            Te1()[(s2 >> 16) & 0xff] ^
            Te2()[(s3 >>  8) & 0xff] ^
            Te3()[(s0      ) & 0xff] ^
            prk[5];
        t2 =
            Te0()[(s2 >> 24)       ] ^
            Te1()[(s3 >> 16) & 0xff] ^
            Te2()[(s0 >>  8) & 0xff] ^
            Te3()[(s1      ) & 0xff] ^
            prk[6];
        t3 =
            Te0()[(s3 >> 24)       ] ^
            Te1()[(s0 >> 16) & 0xff] ^
            Te2()[(s1 >>  8) & 0xff] ^
            Te3()[(s2      ) & 0xff] ^
            prk[7];

        prk += 8;
        if (--r == 0) {
            break;
        }

        s0 =
            Te0()[(t0 >> 24)       ] ^
            Te1()[(t1 >> 16) & 0xff] ^
            Te2()[(t2 >>  8) & 0xff] ^
            Te3()[(t3      ) & 0xff] ^
            prk[0];
        s1 =
            Te0()[(t1 >> 24)       ] ^
            Te1()[(t2 >> 16) & 0xff] ^
            Te2()[(t3 >>  8) & 0xff] ^
            Te3()[(t0      ) & 0xff] ^
            prk[1];
        s2 =
            Te0()[(t2 >> 24)       ] ^
            Te1()[(t3 >> 16) & 0xff] ^
            Te2()[(t0 >>  8) & 0xff] ^
            Te3()[(t1      ) & 0xff] ^
            prk[2];
        s3 =
            Te0()[(t3 >> 24)       ] ^
            Te1()[(t0 >> 16) & 0xff] ^
            Te2()[(t1 >>  8) & 0xff] ^
            Te3()[(t2      ) & 0xff] ^
            prk[3];
    }
    /*
    * apply last round and
    * map cipher state to byte array block:
    */
   s0 =
      (Te4()[(t0 >> 24)       ] & 0xff000000) ^
      (Te4()[(t1 >> 16) & 0xff] & 0x00ff0000) ^
      (Te4()[(t2 >>  8) & 0xff] & 0x0000ff00) ^
      (Te4()[(t3      ) & 0xff] & 0x000000ff) ^
      prk[0];
   s1 =
      (Te4()[(t1 >> 24)       ] & 0xff000000) ^
      (Te4()[(t2 >> 16) & 0xff] & 0x00ff0000) ^
      (Te4()[(t3 >>  8) & 0xff] & 0x0000ff00) ^
      (Te4()[(t0      ) & 0xff] & 0x000000ff) ^
      prk[1];
   s2 =
      (Te4()[(t2 >> 24)       ] & 0xff000000) ^
      (Te4()[(t3 >> 16) & 0xff] & 0x00ff0000) ^
      (Te4()[(t0 >>  8) & 0xff] & 0x0000ff00) ^
      (Te4()[(t1      ) & 0xff] & 0x000000ff) ^
      prk[2];
   s3 =
      (Te4()[(t3 >> 24)       ] & 0xff000000) ^
      (Te4()[(t0 >> 16) & 0xff] & 0x00ff0000) ^
      (Te4()[(t1 >>  8) & 0xff] & 0x0000ff00) ^
      (Te4()[(t2      ) & 0xff] & 0x000000ff) ^
      prk[3];

   PutBlockBigEndian(pOutBlock, s0, s1, s2, s3);
}
