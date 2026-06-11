//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SFIaesDecryptor
//
// Description:
//
// Implementation
//
// Author: Rob Moyse
// Date: November 29, 2004 
//
//---------------------------------------------------------------------------
#include "SFIprecomp.h"
#pragma hdrstop

#include <SFI/SFImisc.h>
#include <SFI/SFIaesDecryptor.h>

SFIaesDecryptor::SFIaesDecryptor(const COLuint8* pUserKey, COLuint32 KeyLength)
   : SFIaes(pUserKey, KeyLength)
{
   COLuint16 i, j;
   COLuint32 Temp, *prk = key();

   /* invert the order of the round keys: */
   for (i = 0, j = 4*rounds(); i < j; i += 4, j -= 4) {
      Temp = prk[i    ]; prk[i    ] = prk[j    ]; prk[j    ] = Temp;
      Temp = prk[i + 1]; prk[i + 1] = prk[j + 1]; prk[j + 1] = Temp;
      Temp = prk[i + 2]; prk[i + 2] = prk[j + 2]; prk[j + 2] = Temp;
      Temp = prk[i + 3]; prk[i + 3] = prk[j + 3]; prk[j + 3] = Temp;
   }
   /* apply the inverse MixColumn transform to all round keys but the first and the last: */
   for (i = 1; i < rounds(); i++) {
      prk += 4;
      prk[0] =
         Td0()[Te4()[(prk[0] >> 24)       ] & 0xff] ^
         Td1()[Te4()[(prk[0] >> 16) & 0xff] & 0xff] ^
         Td2()[Te4()[(prk[0] >>  8) & 0xff] & 0xff] ^
         Td3()[Te4()[(prk[0]      ) & 0xff] & 0xff];
      prk[1] =
         Td0()[Te4()[(prk[1] >> 24)       ] & 0xff] ^
         Td1()[Te4()[(prk[1] >> 16) & 0xff] & 0xff] ^
         Td2()[Te4()[(prk[1] >>  8) & 0xff] & 0xff] ^
         Td3()[Te4()[(prk[1]      ) & 0xff] & 0xff];
      prk[2] =
         Td0()[Te4()[(prk[2] >> 24)       ] & 0xff] ^
         Td1()[Te4()[(prk[2] >> 16) & 0xff] & 0xff] ^
         Td2()[Te4()[(prk[2] >>  8) & 0xff] & 0xff] ^
         Td3()[Te4()[(prk[2]      ) & 0xff] & 0xff];
      prk[3] =
         Td0()[Te4()[(prk[3] >> 24)       ] & 0xff] ^
         Td1()[Te4()[(prk[3] >> 16) & 0xff] & 0xff] ^
         Td2()[Te4()[(prk[3] >>  8) & 0xff] & 0xff] ^
         Td3()[Te4()[(prk[3]      ) & 0xff] & 0xff];
   }

}

void SFIaesDecryptor::ProcessBlock(const COLuint8 *pInBlock, COLuint8 *pOutBlock) const
{
   COLuint32 s0, s1, s2, s3, t0, t1, t2, t3;
   const COLuint32 *prk = key();

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
            Td0()[(s0 >> 24)       ] ^
            Td1()[(s3 >> 16) & 0xff] ^
            Td2()[(s2 >>  8) & 0xff] ^
            Td3()[(s1      ) & 0xff] ^
            prk[4];
        t1 =
            Td0()[(s1 >> 24)       ] ^
            Td1()[(s0 >> 16) & 0xff] ^
            Td2()[(s3 >>  8) & 0xff] ^
            Td3()[(s2      ) & 0xff] ^
            prk[5];
        t2 =
            Td0()[(s2 >> 24)       ] ^
            Td1()[(s1 >> 16) & 0xff] ^
            Td2()[(s0 >>  8) & 0xff] ^
            Td3()[(s3      ) & 0xff] ^
            prk[6];
        t3 =
            Td0()[(s3 >> 24)       ] ^
            Td1()[(s2 >> 16) & 0xff] ^
            Td2()[(s1 >>  8) & 0xff] ^
            Td3()[(s0      ) & 0xff] ^
            prk[7];

        prk += 8;
        if (--r == 0) {
            break;
        }

        s0 =
            Td0()[(t0 >> 24)       ] ^
            Td1()[(t3 >> 16) & 0xff] ^
            Td2()[(t2 >>  8) & 0xff] ^
            Td3()[(t1      ) & 0xff] ^
            prk[0];
        s1 =
            Td0()[(t1 >> 24)       ] ^
            Td1()[(t0 >> 16) & 0xff] ^
            Td2()[(t3 >>  8) & 0xff] ^
            Td3()[(t2      ) & 0xff] ^
            prk[1];
        s2 =
            Td0()[(t2 >> 24)       ] ^
            Td1()[(t1 >> 16) & 0xff] ^
            Td2()[(t0 >>  8) & 0xff] ^
            Td3()[(t3      ) & 0xff] ^
            prk[2];
        s3 =
            Td0()[(t3 >> 24)       ] ^
            Td1()[(t2 >> 16) & 0xff] ^
            Td2()[(t1 >>  8) & 0xff] ^
            Td3()[(t0      ) & 0xff] ^
            prk[3];
    }
    /*
    * apply last round and
    * map cipher state to byte array block:
    */
      s0 =
         (Td4()[(t0 >> 24)       ] & 0xff000000) ^
         (Td4()[(t3 >> 16) & 0xff] & 0x00ff0000) ^
         (Td4()[(t2 >>  8) & 0xff] & 0x0000ff00) ^
         (Td4()[(t1      ) & 0xff] & 0x000000ff) ^
         prk[0];
      s1 =
         (Td4()[(t1 >> 24)       ] & 0xff000000) ^
         (Td4()[(t0 >> 16) & 0xff] & 0x00ff0000) ^
         (Td4()[(t3 >>  8) & 0xff] & 0x0000ff00) ^
         (Td4()[(t2      ) & 0xff] & 0x000000ff) ^
         prk[1];
      s2 =
         (Td4()[(t2 >> 24)       ] & 0xff000000) ^
         (Td4()[(t1 >> 16) & 0xff] & 0x00ff0000) ^
         (Td4()[(t0 >>  8) & 0xff] & 0x0000ff00) ^
         (Td4()[(t3      ) & 0xff] & 0x000000ff) ^
         prk[2];
      s3 =
         (Td4()[(t3 >> 24)       ] & 0xff000000) ^
         (Td4()[(t2 >> 16) & 0xff] & 0x00ff0000) ^
         (Td4()[(t1 >>  8) & 0xff] & 0x0000ff00) ^
         (Td4()[(t0      ) & 0xff] & 0x000000ff) ^
         prk[3];

   PutBlockBigEndian(pOutBlock, s0, s1, s2, s3);

}

