//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SFIcrypt3
//
// Description:
//
// Implementation
//
// Author: Kiril Zorin  (ported from CRYcrypt3)
// Date:   Tuesday, January 15th, 2013 @ 03:23:19 PM
//
//---------------------------------------------------------------------------
#include "SFIprecomp.h"
#pragma hdrstop

#include "SFIcrypt3.h"
#include "SFImd5Imp.h"

#include <COL/COLmath.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

bool SFIcrypt3::m_RandomizerReset = false;

const COLuint32 SFIcrypt3::m_DESPC2[8][64] =
{
   {
      /* for C bits (numbered as per FIPS 46) 1 2 3 4 5 6 */
      0x00000000L, 0x00000010L, 0x20000000L, 0x20000010L, 
         0x00010000L, 0x00010010L, 0x20010000L, 0x20010010L, 
         0x00000800L, 0x00000810L, 0x20000800L, 0x20000810L, 
         0x00010800L, 0x00010810L, 0x20010800L, 0x20010810L, 
         0x00000020L, 0x00000030L, 0x20000020L, 0x20000030L, 
         0x00010020L, 0x00010030L, 0x20010020L, 0x20010030L, 
         0x00000820L, 0x00000830L, 0x20000820L, 0x20000830L, 
         0x00010820L, 0x00010830L, 0x20010820L, 0x20010830L, 
         0x00080000L, 0x00080010L, 0x20080000L, 0x20080010L, 
         0x00090000L, 0x00090010L, 0x20090000L, 0x20090010L, 
         0x00080800L, 0x00080810L, 0x20080800L, 0x20080810L, 
         0x00090800L, 0x00090810L, 0x20090800L, 0x20090810L, 
         0x00080020L, 0x00080030L, 0x20080020L, 0x20080030L, 
         0x00090020L, 0x00090030L, 0x20090020L, 0x20090030L, 
         0x00080820L, 0x00080830L, 0x20080820L, 0x20080830L, 
         0x00090820L, 0x00090830L, 0x20090820L, 0x20090830L, 
   },
   {
      /* for C bits (numbered as per FIPS 46) 7 8 10 11 12 13 */
      0x00000000L, 0x02000000L, 0x00002000L, 0x02002000L, 
         0x00200000L, 0x02200000L, 0x00202000L, 0x02202000L, 
         0x00000004L, 0x02000004L, 0x00002004L, 0x02002004L, 
         0x00200004L, 0x02200004L, 0x00202004L, 0x02202004L, 
         0x00000400L, 0x02000400L, 0x00002400L, 0x02002400L, 
         0x00200400L, 0x02200400L, 0x00202400L, 0x02202400L, 
         0x00000404L, 0x02000404L, 0x00002404L, 0x02002404L, 
         0x00200404L, 0x02200404L, 0x00202404L, 0x02202404L, 
         0x10000000L, 0x12000000L, 0x10002000L, 0x12002000L, 
         0x10200000L, 0x12200000L, 0x10202000L, 0x12202000L, 
         0x10000004L, 0x12000004L, 0x10002004L, 0x12002004L, 
         0x10200004L, 0x12200004L, 0x10202004L, 0x12202004L, 
         0x10000400L, 0x12000400L, 0x10002400L, 0x12002400L, 
         0x10200400L, 0x12200400L, 0x10202400L, 0x12202400L, 
         0x10000404L, 0x12000404L, 0x10002404L, 0x12002404L, 
         0x10200404L, 0x12200404L, 0x10202404L, 0x12202404L, 
      },
      {
         /* for C bits (numbered as per FIPS 46) 14 15 16 17 19 20 */
         0x00000000L, 0x00000001L, 0x00040000L, 0x00040001L, 
            0x01000000L, 0x01000001L, 0x01040000L, 0x01040001L, 
            0x00000002L, 0x00000003L, 0x00040002L, 0x00040003L, 
            0x01000002L, 0x01000003L, 0x01040002L, 0x01040003L, 
            0x00000200L, 0x00000201L, 0x00040200L, 0x00040201L, 
            0x01000200L, 0x01000201L, 0x01040200L, 0x01040201L, 
            0x00000202L, 0x00000203L, 0x00040202L, 0x00040203L, 
            0x01000202L, 0x01000203L, 0x01040202L, 0x01040203L, 
            0x08000000L, 0x08000001L, 0x08040000L, 0x08040001L, 
            0x09000000L, 0x09000001L, 0x09040000L, 0x09040001L, 
            0x08000002L, 0x08000003L, 0x08040002L, 0x08040003L, 
            0x09000002L, 0x09000003L, 0x09040002L, 0x09040003L, 
            0x08000200L, 0x08000201L, 0x08040200L, 0x08040201L, 
            0x09000200L, 0x09000201L, 0x09040200L, 0x09040201L, 
            0x08000202L, 0x08000203L, 0x08040202L, 0x08040203L, 
            0x09000202L, 0x09000203L, 0x09040202L, 0x09040203L, 
      },
      {
         /* for C bits (numbered as per FIPS 46) 21 23 24 26 27 28 */
         0x00000000L, 0x00100000L, 0x00000100L, 0x00100100L, 
            0x00000008L, 0x00100008L, 0x00000108L, 0x00100108L, 
            0x00001000L, 0x00101000L, 0x00001100L, 0x00101100L, 
            0x00001008L, 0x00101008L, 0x00001108L, 0x00101108L, 
            0x04000000L, 0x04100000L, 0x04000100L, 0x04100100L, 
            0x04000008L, 0x04100008L, 0x04000108L, 0x04100108L, 
            0x04001000L, 0x04101000L, 0x04001100L, 0x04101100L, 
            0x04001008L, 0x04101008L, 0x04001108L, 0x04101108L, 
            0x00020000L, 0x00120000L, 0x00020100L, 0x00120100L, 
            0x00020008L, 0x00120008L, 0x00020108L, 0x00120108L, 
            0x00021000L, 0x00121000L, 0x00021100L, 0x00121100L, 
            0x00021008L, 0x00121008L, 0x00021108L, 0x00121108L, 
            0x04020000L, 0x04120000L, 0x04020100L, 0x04120100L, 
            0x04020008L, 0x04120008L, 0x04020108L, 0x04120108L, 
            0x04021000L, 0x04121000L, 0x04021100L, 0x04121100L, 
            0x04021008L, 0x04121008L, 0x04021108L, 0x04121108L, 
         },
         {
            /* for D bits (numbered as per FIPS 46) 1 2 3 4 5 6 */
            0x00000000L, 0x10000000L, 0x00010000L, 0x10010000L, 
               0x00000004L, 0x10000004L, 0x00010004L, 0x10010004L, 
               0x20000000L, 0x30000000L, 0x20010000L, 0x30010000L, 
               0x20000004L, 0x30000004L, 0x20010004L, 0x30010004L, 
               0x00100000L, 0x10100000L, 0x00110000L, 0x10110000L, 
               0x00100004L, 0x10100004L, 0x00110004L, 0x10110004L, 
               0x20100000L, 0x30100000L, 0x20110000L, 0x30110000L, 
               0x20100004L, 0x30100004L, 0x20110004L, 0x30110004L, 
               0x00001000L, 0x10001000L, 0x00011000L, 0x10011000L, 
               0x00001004L, 0x10001004L, 0x00011004L, 0x10011004L, 
               0x20001000L, 0x30001000L, 0x20011000L, 0x30011000L, 
               0x20001004L, 0x30001004L, 0x20011004L, 0x30011004L, 
               0x00101000L, 0x10101000L, 0x00111000L, 0x10111000L, 
               0x00101004L, 0x10101004L, 0x00111004L, 0x10111004L, 
               0x20101000L, 0x30101000L, 0x20111000L, 0x30111000L, 
               0x20101004L, 0x30101004L, 0x20111004L, 0x30111004L, 
         },
         {
            /* for D bits (numbered as per FIPS 46) 8 9 11 12 13 14 */
            0x00000000L, 0x08000000L, 0x00000008L, 0x08000008, 
               0x00000400L, 0x08000400L, 0x00000408L, 0x08000408, 
               0x00020000L, 0x08020000L, 0x00020008L, 0x08020008, 
               0x00020400L, 0x08020400L, 0x00020408L, 0x08020408, 
               0x00000001L, 0x08000001L, 0x00000009L, 0x08000009, 
               0x00000401L, 0x08000401L, 0x00000409L, 0x08000409, 
               0x00020001L, 0x08020001L, 0x00020009L, 0x08020009, 
               0x00020401L, 0x08020401L, 0x00020409L, 0x08020409, 
               0x02000000L, 0x0A000000L, 0x02000008L, 0x0A000008, 
               0x02000400L, 0x0A000400L, 0x02000408L, 0x0A000408, 
               0x02020000L, 0x0A020000L, 0x02020008L, 0x0A020008, 
               0x02020400L, 0x0A020400L, 0x02020408L, 0x0A020408, 
               0x02000001L, 0x0A000001L, 0x02000009L, 0x0A000009, 
               0x02000401L, 0x0A000401L, 0x02000409L, 0x0A000409, 
               0x02020001L, 0x0A020001L, 0x02020009L, 0x0A020009, 
               0x02020401L, 0x0A020401L, 0x02020409L, 0x0A020409, 
            },
            {
               /* for D bits (numbered as per FIPS 46) 16 17 18 19 20 21 */
               0x00000000L, 0x00000100L, 0x00080000L, 0x00080100, 
                  0x01000000L, 0x01000100L, 0x01080000L, 0x01080100, 
                  0x00000010L, 0x00000110L, 0x00080010L, 0x00080110, 
                  0x01000010L, 0x01000110L, 0x01080010L, 0x01080110, 
                  0x00200000L, 0x00200100L, 0x00280000L, 0x00280100, 
                  0x01200000L, 0x01200100L, 0x01280000L, 0x01280100, 
                  0x00200010L, 0x00200110L, 0x00280010L, 0x00280110, 
                  0x01200010L, 0x01200110L, 0x01280010L, 0x01280110, 
                  0x00000200L, 0x00000300L, 0x00080200L, 0x00080300, 
                  0x01000200L, 0x01000300L, 0x01080200L, 0x01080300, 
                  0x00000210L, 0x00000310L, 0x00080210L, 0x00080310, 
                  0x01000210L, 0x01000310L, 0x01080210L, 0x01080310, 
                  0x00200200L, 0x00200300L, 0x00280200L, 0x00280300, 
                  0x01200200L, 0x01200300L, 0x01280200L, 0x01280300, 
                  0x00200210L, 0x00200310L, 0x00280210L, 0x00280310, 
                  0x01200210L, 0x01200310L, 0x01280210L, 0x01280310, 
            },
            {
               /* for D bits (numbered as per FIPS 46) 22 23 24 25 27 28 */
               0x00000000L, 0x04000000L, 0x00040000L, 0x04040000, 
                  0x00000002L, 0x04000002L, 0x00040002L, 0x04040002, 
                  0x00002000L, 0x04002000L, 0x00042000L, 0x04042000, 
                  0x00002002L, 0x04002002L, 0x00042002L, 0x04042002, 
                  0x00000020L, 0x04000020L, 0x00040020L, 0x04040020, 
                  0x00000022L, 0x04000022L, 0x00040022L, 0x04040022, 
                  0x00002020L, 0x04002020L, 0x00042020L, 0x04042020, 
                  0x00002022L, 0x04002022L, 0x00042022L, 0x04042022, 
                  0x00000800L, 0x04000800L, 0x00040800L, 0x04040800, 
                  0x00000802L, 0x04000802L, 0x00040802L, 0x04040802, 
                  0x00002800L, 0x04002800L, 0x00042800L, 0x04042800, 
                  0x00002802L, 0x04002802L, 0x00042802L, 0x04042802, 
                  0x00000820L, 0x04000820L, 0x00040820L, 0x04040820, 
                  0x00000822L, 0x04000822L, 0x00040822L, 0x04040822, 
                  0x00002820L, 0x04002820L, 0x00042820L, 0x04042820, 
                  0x00002822L, 0x04002822L, 0x00042822L, 0x04042822, 
               },
};

const COLuint32 SFIcrypt3::m_DESSBox[8][64] =
{
   {
      /* nibble 0 */
      0x00820200L, 0x00020000L, 0x80800000L, 0x80820200,
         0x00800000L, 0x80020200L, 0x80020000L, 0x80800000,
         0x80020200L, 0x00820200L, 0x00820000L, 0x80000200,
         0x80800200L, 0x00800000L, 0x00000000L, 0x80020000,
         0x00020000L, 0x80000000L, 0x00800200L, 0x00020200,
         0x80820200L, 0x00820000L, 0x80000200L, 0x00800200,
         0x80000000L, 0x00000200L, 0x00020200L, 0x80820000,
         0x00000200L, 0x80800200L, 0x80820000L, 0x00000000,
         0x00000000L, 0x80820200L, 0x00800200L, 0x80020000,
         0x00820200L, 0x00020000L, 0x80000200L, 0x00800200,
         0x80820000L, 0x00000200L, 0x00020200L, 0x80800000,
         0x80020200L, 0x80000000L, 0x80800000L, 0x00820000,
         0x80820200L, 0x00020200L, 0x00820000L, 0x80800200,
         0x00800000L, 0x80000200L, 0x80020000L, 0x00000000,
         0x00020000L, 0x00800000L, 0x80800200L, 0x00820200,
         0x80000000L, 0x80820000L, 0x00000200L, 0x80020200,
   },
   {
      /* nibble 1 */
      0x10042004L, 0x00000000L, 0x00042000L, 0x10040000,
         0x10000004L, 0x00002004L, 0x10002000L, 0x00042000,
         0x00002000L, 0x10040004L, 0x00000004L, 0x10002000,
         0x00040004L, 0x10042000L, 0x10040000L, 0x00000004,
         0x00040000L, 0x10002004L, 0x10040004L, 0x00002000,
         0x00042004L, 0x10000000L, 0x00000000L, 0x00040004,
         0x10002004L, 0x00042004L, 0x10042000L, 0x10000004,
         0x10000000L, 0x00040000L, 0x00002004L, 0x10042004,
         0x00040004L, 0x10042000L, 0x10002000L, 0x00042004,
         0x10042004L, 0x00040004L, 0x10000004L, 0x00000000,
         0x10000000L, 0x00002004L, 0x00040000L, 0x10040004,
         0x00002000L, 0x10000000L, 0x00042004L, 0x10002004,
         0x10042000L, 0x00002000L, 0x00000000L, 0x10000004,
         0x00000004L, 0x10042004L, 0x00042000L, 0x10040000,
         0x10040004L, 0x00040000L, 0x00002004L, 0x10002000,
         0x10002004L, 0x00000004L, 0x10040000L, 0x00042000,
      },
      {
         /* nibble 2 */
         0x41000000L, 0x01010040L, 0x00000040L, 0x41000040,
            0x40010000L, 0x01000000L, 0x41000040L, 0x00010040,
            0x01000040L, 0x00010000L, 0x01010000L, 0x40000000,
            0x41010040L, 0x40000040L, 0x40000000L, 0x41010000,
            0x00000000L, 0x40010000L, 0x01010040L, 0x00000040,
            0x40000040L, 0x41010040L, 0x00010000L, 0x41000000,
            0x41010000L, 0x01000040L, 0x40010040L, 0x01010000,
            0x00010040L, 0x00000000L, 0x01000000L, 0x40010040,
            0x01010040L, 0x00000040L, 0x40000000L, 0x00010000,
            0x40000040L, 0x40010000L, 0x01010000L, 0x41000040,
            0x00000000L, 0x01010040L, 0x00010040L, 0x41010000,
            0x40010000L, 0x01000000L, 0x41010040L, 0x40000000,
            0x40010040L, 0x41000000L, 0x01000000L, 0x41010040,
            0x00010000L, 0x01000040L, 0x41000040L, 0x00010040,
            0x01000040L, 0x00000000L, 0x41010000L, 0x40000040,
            0x41000000L, 0x40010040L, 0x00000040L, 0x01010000,
      },
      {
         /* nibble 3 */
         0x00100402L, 0x04000400L, 0x00000002L, 0x04100402,
            0x00000000L, 0x04100000L, 0x04000402L, 0x00100002,
            0x04100400L, 0x04000002L, 0x04000000L, 0x00000402,
            0x04000002L, 0x00100402L, 0x00100000L, 0x04000000,
            0x04100002L, 0x00100400L, 0x00000400L, 0x00000002,
            0x00100400L, 0x04000402L, 0x04100000L, 0x00000400,
            0x00000402L, 0x00000000L, 0x00100002L, 0x04100400,
            0x04000400L, 0x04100002L, 0x04100402L, 0x00100000,
            0x04100002L, 0x00000402L, 0x00100000L, 0x04000002,
            0x00100400L, 0x04000400L, 0x00000002L, 0x04100000,
            0x04000402L, 0x00000000L, 0x00000400L, 0x00100002,
            0x00000000L, 0x04100002L, 0x04100400L, 0x00000400,
            0x04000000L, 0x04100402L, 0x00100402L, 0x00100000,
            0x04100402L, 0x00000002L, 0x04000400L, 0x00100402,
            0x00100002L, 0x00100400L, 0x04100000L, 0x04000402,
            0x00000402L, 0x04000000L, 0x04000002L, 0x04100400,
         },
         {
            /* nibble 4 */
            0x02000000L, 0x00004000L, 0x00000100L, 0x02004108,
               0x02004008L, 0x02000100L, 0x00004108L, 0x02004000,
               0x00004000L, 0x00000008L, 0x02000008L, 0x00004100,
               0x02000108L, 0x02004008L, 0x02004100L, 0x00000000,
               0x00004100L, 0x02000000L, 0x00004008L, 0x00000108,
               0x02000100L, 0x00004108L, 0x00000000L, 0x02000008,
               0x00000008L, 0x02000108L, 0x02004108L, 0x00004008,
               0x02004000L, 0x00000100L, 0x00000108L, 0x02004100,
               0x02004100L, 0x02000108L, 0x00004008L, 0x02004000,
               0x00004000L, 0x00000008L, 0x02000008L, 0x02000100,
               0x02000000L, 0x00004100L, 0x02004108L, 0x00000000,
               0x00004108L, 0x02000000L, 0x00000100L, 0x00004008,
               0x02000108L, 0x00000100L, 0x00000000L, 0x02004108,
               0x02004008L, 0x02004100L, 0x00000108L, 0x00004000,
               0x00004100L, 0x02004008L, 0x02000100L, 0x00000108,
               0x00000008L, 0x00004108L, 0x02004000L, 0x02000008,
         },
         {
            /* nibble 5 */
            0x20000010L, 0x00080010L, 0x00000000L, 0x20080800,
               0x00080010L, 0x00000800L, 0x20000810L, 0x00080000,
               0x00000810L, 0x20080810L, 0x00080800L, 0x20000000,
               0x20000800L, 0x20000010L, 0x20080000L, 0x00080810,
               0x00080000L, 0x20000810L, 0x20080010L, 0x00000000,
               0x00000800L, 0x00000010L, 0x20080800L, 0x20080010,
               0x20080810L, 0x20080000L, 0x20000000L, 0x00000810,
               0x00000010L, 0x00080800L, 0x00080810L, 0x20000800,
               0x00000810L, 0x20000000L, 0x20000800L, 0x00080810,
               0x20080800L, 0x00080010L, 0x00000000L, 0x20000800,
               0x20000000L, 0x00000800L, 0x20080010L, 0x00080000,
               0x00080010L, 0x20080810L, 0x00080800L, 0x00000010,
               0x20080810L, 0x00080800L, 0x00080000L, 0x20000810,
               0x20000010L, 0x20080000L, 0x00080810L, 0x00000000,
               0x00000800L, 0x20000010L, 0x20000810L, 0x20080800,
               0x20080000L, 0x00000810L, 0x00000010L, 0x20080010,
            },
            {
               /* nibble 6 */
               0x00001000L, 0x00000080L, 0x00400080L, 0x00400001,
                  0x00401081L, 0x00001001L, 0x00001080L, 0x00000000,
                  0x00400000L, 0x00400081L, 0x00000081L, 0x00401000,
                  0x00000001L, 0x00401080L, 0x00401000L, 0x00000081,
                  0x00400081L, 0x00001000L, 0x00001001L, 0x00401081,
                  0x00000000L, 0x00400080L, 0x00400001L, 0x00001080,
                  0x00401001L, 0x00001081L, 0x00401080L, 0x00000001,
                  0x00001081L, 0x00401001L, 0x00000080L, 0x00400000,
                  0x00001081L, 0x00401000L, 0x00401001L, 0x00000081,
                  0x00001000L, 0x00000080L, 0x00400000L, 0x00401001,
                  0x00400081L, 0x00001081L, 0x00001080L, 0x00000000,
                  0x00000080L, 0x00400001L, 0x00000001L, 0x00400080,
                  0x00000000L, 0x00400081L, 0x00400080L, 0x00001080,
                  0x00000081L, 0x00001000L, 0x00401081L, 0x00400000,
                  0x00401080L, 0x00000001L, 0x00001001L, 0x00401081,
                  0x00400001L, 0x00401080L, 0x00401000L, 0x00001001,
            },
            {
               /* nibble 7 */
               0x08200020L, 0x08208000L, 0x00008020L, 0x00000000,
                  0x08008000L, 0x00200020L, 0x08200000L, 0x08208020,
                  0x00000020L, 0x08000000L, 0x00208000L, 0x00008020,
                  0x00208020L, 0x08008020L, 0x08000020L, 0x08200000,
                  0x00008000L, 0x00208020L, 0x00200020L, 0x08008000,
                  0x08208020L, 0x08000020L, 0x00000000L, 0x00208000,
                  0x08000000L, 0x00200000L, 0x08008020L, 0x08200020,
                  0x00200000L, 0x00008000L, 0x08208000L, 0x00000020,
                  0x00200000L, 0x00008000L, 0x08000020L, 0x08208020,
                  0x00008020L, 0x08000000L, 0x00000000L, 0x00208000,
                  0x08200020L, 0x08008020L, 0x08008000L, 0x00200020,
                  0x08208000L, 0x00000020L, 0x00200020L, 0x08008000,
                  0x08208020L, 0x00200000L, 0x08200000L, 0x08000020,
                  0x00208000L, 0x00008020L, 0x08008020L, 0x08200000,
                  0x00000020L, 0x08208000L, 0x00208020L, 0x00000000,
                  0x08000000L, 0x08200020L, 0x00008000L, 0x00208020
               }
};

const COLuint32 SFIcrypt3::m_SaltTable[128] =
{
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 
   0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 
   0x0A, 0x0B, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 
   0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 
   0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 
   0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 
   0x23, 0x24, 0x25, 0x20, 0x21, 0x22, 0x23, 0x24, 
   0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 
   0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0x34, 
   0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 
   0x3D, 0x3E, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 
};

const COLuint32 SFIcrypt3::m_DESIP[64] =
{
   0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 
   0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 
   0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 
   0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 
   0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x61, 0x62, 
   0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 
   0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71, 0x72, 
   0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A
};

static inline COLuint32 byteToUnsigned(unsigned char Byte)
{
   COLuint32 Value = (COLuint32) Byte;
   return(Value >= 0 ? Value : Value + 256);
}

static inline COLuint32 fourBytesToUnsigned(const unsigned char* pBytes, COLuint32 Offset)
{
   return (byteToUnsigned(pBytes[Offset+0]) |
      (byteToUnsigned(pBytes[Offset+1]) <<  8L) |
      (byteToUnsigned(pBytes[Offset+2]) << 16L) |
      (byteToUnsigned(pBytes[Offset+3]) << 24L));
}

static inline void unsignedToFourBytes(COLuint32 Value, unsigned char* pBytes, COLuint32 Offset)
{
   pBytes[Offset+0] = (unsigned char)((Value) & 0xff);
   pBytes[Offset+1] = (unsigned char)((Value >> 8L ) & 0xff);
   pBytes[Offset+2] = (unsigned char)((Value >> 16L) & 0xff);
   pBytes[Offset+3] = (unsigned char)((Value >> 24L) & 0xff);
}

void SFIcrypt3::PERM_OP(COLuint32 a, COLuint32 b, COLuint32 n, COLuint32 m, COLuint32 KeySchedule[32])
{
   COLuint32 t;

   t = ((a >> n) ^ b) & m;
   a ^= t << n;
   b ^= t;

   KeySchedule[0] = a;
   KeySchedule[1] = b;
}

COLuint32 SFIcrypt3::HPERM_OP(COLuint32 a, COLint32 n, COLuint32 m)
{
   COLuint32 t;

   t = ((a << (16 - n)) ^ a) & m;
   a = a ^ t ^ (t >> (16 - n));

   return(a);
}

void SFIcrypt3::buildKeySchedule(const unsigned char Key[8], COLuint32 KeySchedule[32])
{
   COLuint32 LeftShifts[16] = {0, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0};

   COLuint32 Left = fourBytesToUnsigned(Key, 0);
   COLuint32 Right = fourBytesToUnsigned(Key, 4);

   PERM_OP(Right, Left, 4, 0x0f0f0f0f, KeySchedule);
   Right = KeySchedule[0]; 
   Left = KeySchedule[1];

   Left = HPERM_OP(Left, -2, 0xcccc0000);
   Right = HPERM_OP(Right, -2, 0xcccc0000);

   PERM_OP(Right, Left, 1, 0x55555555, KeySchedule);
   Right = KeySchedule[0]; 
   Left = KeySchedule[1];

   PERM_OP(Left, Right, 8, 0x00ff00ff, KeySchedule);
   Left = KeySchedule[0]; 
   Right = KeySchedule[1];

   PERM_OP(Right, Left, 1, 0x55555555, KeySchedule);
   Right = KeySchedule[0]; 
   Left = KeySchedule[1];

   Right = (((Right & 0x000000ff) <<  16L) |  (Right & 0x0000ff00) |
      ((Right & 0x00ff0000) >> 16L) | ((Left & 0xf0000000) >> 4L));
   Left &= 0x0fffffff;

   COLuint32 s, t; // temps
   COLuint32 KeyIndex = 0;

   for(COLuint32 IterationIndex = 0; IterationIndex < 16; IterationIndex++)
   {
      if(LeftShifts[IterationIndex])
      {
         Left = (Left >> 2L) | (Left << 26L);
         Right = (Right >> 2L) | (Right << 26L);
      }
      else
      {
         Left = (Left >> 1L) | (Left << 27L);
         Right = (Right >> 1L) | (Right << 27L);
      }

      Left &= 0x0fffffff;
      Right &= 0x0fffffff;

      s = m_DESPC2[0][(Left) & 0x3f] |
         m_DESPC2[1][((Left >>  6L) & 0x03) | ((Left >>  7L) & 0x3c)] |
         m_DESPC2[2][((Left >> 13L) & 0x0f) | ((Left >> 14L) & 0x30)] |
         m_DESPC2[3][((Left >> 20L) & 0x01) | ((Left >> 21L) & 0x06) |
         ((Left >> 22L) & 0x38)];

      t = m_DESPC2[4][(Right) & 0x3f] |
         m_DESPC2[5][((Right >> 7L) & 0x03) | ((Right >>  8L) & 0x3c)] |
         m_DESPC2[6][(Right >>15L) & 0x3f ] |
         m_DESPC2[7][((Right >>21L) & 0x0f) | ((Right >> 22L) & 0x30)];

      KeySchedule[KeyIndex++] = ((t <<  16L) | (s & 0x0000ffff)) & 0xffffffff;
      s = ((s >> 16L) | (t & 0xffff0000));

      s = (s << 4L) | (s >> 28L);
      KeySchedule[KeyIndex++] = s & 0xffffffff;
   }
}

COLuint32 SFIcrypt3::DESEncrypt
(
 COLuint32 Left, 
 COLuint32 Right, 
 COLuint32 KeyIndex, 
 COLuint32 ExpansionSwap[2], 
 const COLuint32 KeySchedule[32]
)
{
   COLuint32 t, u, v; // temporary vars

   v = Right ^ (Right >> 16L);
   u = v & ExpansionSwap[0];
   v = v & ExpansionSwap[1];
   u = (u ^ (u << 16L)) ^ Right ^ KeySchedule[KeyIndex];
   t = (v ^ (v << 16L)) ^ Right ^ KeySchedule[KeyIndex + 1];
   t = (t >> 4L) | (t << 28L);

   Left ^= m_DESSBox[1][(t) & 0x3f] |
      m_DESSBox[3][(t >>  8L) & 0x3f] |
      m_DESSBox[5][(t >> 16L) & 0x3f] |
      m_DESSBox[7][(t >> 24L) & 0x3f] |
      m_DESSBox[0][(u) & 0x3f] |
      m_DESSBox[2][(u >>  8L) & 0x3f] |
      m_DESSBox[4][(u >> 16L) & 0x3f] |
      m_DESSBox[6][(u >> 24L) & 0x3f];

   return(Left);
}


void SFIcrypt3::cryptMain(const COLuint32 KeySchedule[32], COLuint32 ExpansionSwap[2], COLuint32 Cipher[2])
{
   COLuint32 Left, Right, Temp;
   COLuint32 TempCipher[2];

   Left = Right = Temp = 0;

   for(COLuint32 RoundIndex = 0; RoundIndex < 25; RoundIndex++)
   {
      // TODO - change i into KeyIndex
      for(COLuint32 i = 0; i < 32; i += 4)
      {
         Left  = DESEncrypt(Left, Right, i, ExpansionSwap, KeySchedule);
         Right = DESEncrypt(Right, Left, i + 2, ExpansionSwap, KeySchedule);
      }

      Temp  = Left; 
      Left  = Right; 
      Right = Temp;
   }

   Temp = Right;

   Right = (Left >> 1L) | (Left << 31L);
   Left  = (Temp >> 1L) | (Temp << 31L);

   Left  &= 0xffffffff;
   Right &= 0xffffffff;

   PERM_OP(Right, Left, 1, 0x55555555, TempCipher); 
   Right = TempCipher[0]; 
   Left = TempCipher[1];

   PERM_OP(Left, Right, 8, 0x00ff00ff, TempCipher); 
   Left = TempCipher[0]; 
   Right = TempCipher[1];

   PERM_OP(Right, Left, 2, 0x33333333, TempCipher); 
   Right = TempCipher[0]; 
   Left = TempCipher[1];

   PERM_OP(Left, Right, 16, 0x0000ffff, TempCipher);
   Left = TempCipher[0]; 
   Right = TempCipher[1];

   PERM_OP(Right, Left, 4, 0x0f0f0f0f, TempCipher);
   Right = TempCipher[0]; 
   Left = TempCipher[1];

   Cipher[0] = Left; 
   Cipher[1] = Right;
}

void SFIcrypt3::crypt(const COLstring& PlainText, const COLstring* pSalt, COLstring& ReturnCipherText)
{
   static COLstring SaltRange = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789./";

   unsigned char CipherBuffer[9], Key[8] = {0};
   COLuint32 KeySchedule[32], Cipher[2], ExpansionSwap[2];
   COLstring CipherText;

   if (!m_RandomizerReset)
   {
      COLmath::resetRandomGenerator();
      m_RandomizerReset = true;
   }

   CipherText += (pSalt) ? (*pSalt)[0] : SaltRange[COLmath::randomNumber(SaltRange.size() - 1)];
   CipherText += (pSalt) ? (*pSalt)[1] : SaltRange[COLmath::randomNumber(SaltRange.size() - 1)];

   ExpansionSwap[0] = m_SaltTable[CipherText[0]];
   ExpansionSwap[1] = m_SaltTable[CipherText[1]] << 4;

   COLuint32 ByteIndex;
   for(ByteIndex = 0; ByteIndex < 8 && ByteIndex < PlainText.size(); ByteIndex++)
   {
      Key[ByteIndex] = (unsigned char) (PlainText[ByteIndex] << 1);
   }

   buildKeySchedule(Key, KeySchedule);
   cryptMain(KeySchedule, ExpansionSwap, Cipher);

   unsignedToFourBytes(Cipher[0], CipherBuffer, 0);
   unsignedToFourBytes(Cipher[1], CipherBuffer, 4);
   CipherBuffer[8] = 0;

   COLuint32 y, u, BitIndex, OutBit;
   for(ByteIndex = 2, y = 0, u = 0x80; ByteIndex < 13; ByteIndex++)
   {
      for(BitIndex = 0, OutBit = 0; BitIndex < 6; BitIndex++)
      {
         OutBit <<= 1L;

         if(((int)CipherBuffer[y] & u) != 0)
         {
            OutBit |= 1;
         }

         u >>= 1L;

         if(u == 0)
         {
            y++;
            u = 0x80;
         }
      }

      CipherText += m_DESIP[OutBit];
   }

   ReturnCipherText = CipherText;
}

static const COLstring ItoA64 = "./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

void SFIcrypt3::to64(char* pString, COLuint32 Number, COLint32 Size)
{
   while (--Size >= 0)
   {
      *pString++ = ItoA64[Number & 0x3f];
      Number >>= 6;
   }
}

void SFIcrypt3::md5crypt(const COLstring& PlainText, const COLstring* pSalt, COLstring& CipherText)
{
   static const COLstring MD5HashID = "$1$"; // magic string, defined by inventor of algorithm (not me)

   static const COLuint32 SizeOfTempBuffer = 16;
   static const COLuint32 SizeOfHashedPassword = 22;
   static const COLuint32 SaltMaxSize = 8;

   char HashedPassword[SizeOfHashedPassword + 1] = {0}; 
   unsigned char TempBuffer[SizeOfTempBuffer] = {0};
   COLstring RandomSalt(SaltMaxSize, '\0');

   md5_state_t FirstPass, SecondPass;

   if (!pSalt)
   {
      if (!m_RandomizerReset)
      {
         COLmath::resetRandomGenerator();
         m_RandomizerReset = true;
      }

      for (COLuint32 CharIndex = 0; CharIndex < SaltMaxSize; CharIndex++)
      {
         RandomSalt[CharIndex] = ItoA64[COLmath::randomNumber(ItoA64.size() - 1)];
      }

      pSalt = &RandomSalt;
   }

   COLuint32 SizeOfSalt = COL_MIN(pSalt->size(), SaltMaxSize);

   md5_init(&FirstPass);

   // do password first
   md5_append(&FirstPass, (const unsigned char*) PlainText.c_str(), PlainText.size());

   // do hash ID
   md5_append(&FirstPass, (const unsigned char*) MD5HashID.c_str(), MD5HashID.size());

   // do raw salt
   md5_append(&FirstPass, (const unsigned char*) pSalt->c_str(), SizeOfSalt);

   // do just as many characters of the MD5(PlainText, Salt, PlainText)
   md5_init(&SecondPass);
   md5_append(&SecondPass, (const unsigned char*) PlainText.c_str(), PlainText.size());
   md5_append(&SecondPass, (const unsigned char*) pSalt->c_str(), SizeOfSalt);
   md5_append(&SecondPass, (const unsigned char*) PlainText.c_str(), PlainText.size());

   md5_finish(&SecondPass, TempBuffer);

   COLint32 CharIndex;
   for (CharIndex = PlainText.size(); CharIndex > 0; CharIndex -= SizeOfTempBuffer)
   {
      md5_append(&FirstPass, TempBuffer, CharIndex > SizeOfTempBuffer ? SizeOfTempBuffer : CharIndex);
   }

   ::memset(TempBuffer, 0, SizeOfTempBuffer); 

   // then something weird
   COLuint32 RevCharIndex;
   for (CharIndex = 0, RevCharIndex = PlainText.size(); RevCharIndex; RevCharIndex >>= 1)
   {
      if (RevCharIndex & 0x1)
      {
         md5_append(&FirstPass, TempBuffer + CharIndex, 1);
      }
      else
      {
         md5_append(&FirstPass, (const unsigned char*) &PlainText.c_str()[CharIndex], 1);
      }
   }

   // make output string
   CipherText = MD5HashID; // magic string
   CipherText += *pSalt;   // salt
   CipherText += "$";      // $ before hashed password

   md5_finish(&FirstPass, TempBuffer);

   static const COLuint32 CountOfRound = 1000;
   for(COLuint32 RoundIndex = 0; RoundIndex < CountOfRound; RoundIndex++) 
   {
      md5_init(&SecondPass);

      if (RoundIndex & 1)
      {
         md5_append(&SecondPass, (const unsigned char*) PlainText.c_str(), PlainText.size());
      }
      else
      {
         md5_append(&SecondPass, TempBuffer, SizeOfTempBuffer);
      }

      if (RoundIndex % 3)
      {
         md5_append(&SecondPass, (const unsigned char*) pSalt->c_str(), SizeOfSalt);
      }

      if (RoundIndex % 7)
      {
         md5_append(&SecondPass, (const unsigned char*) PlainText.c_str(), PlainText.size());
      }

      if(RoundIndex & 1)
      {
         md5_append(&SecondPass, TempBuffer, SizeOfTempBuffer);
      }
      else
      {
         md5_append(&SecondPass, (const unsigned char*) PlainText.c_str(), PlainText.size());
      }

      md5_finish(&SecondPass, TempBuffer);
   }

   char* pHashedChar = HashedPassword;
   COLuint32 Temp;

   Temp = (TempBuffer[0] << SizeOfTempBuffer) | (TempBuffer[6] << 8) | TempBuffer[12]; 
   to64(pHashedChar, Temp, 4); 
   pHashedChar += 4;
   Temp = (TempBuffer[1] << SizeOfTempBuffer) | (TempBuffer[7] << 8) | TempBuffer[13]; 
   to64(pHashedChar, Temp, 4); 
   pHashedChar += 4;
   Temp = (TempBuffer[2] << SizeOfTempBuffer) | (TempBuffer[8] << 8) | TempBuffer[14]; 
   to64(pHashedChar, Temp, 4); 
   pHashedChar += 4;
   Temp = (TempBuffer[3] << SizeOfTempBuffer) | (TempBuffer[9] << 8) | TempBuffer[15]; 
   to64(pHashedChar, Temp, 4); 
   pHashedChar += 4;
   Temp = (TempBuffer[4] << SizeOfTempBuffer) | (TempBuffer[10] << 8) | TempBuffer[5]; 
   to64(pHashedChar, Temp, 4); 
   pHashedChar += 4;
   Temp = TempBuffer[11]; 
   to64(pHashedChar, Temp, 2); 

   CipherText += HashedPassword;
}
