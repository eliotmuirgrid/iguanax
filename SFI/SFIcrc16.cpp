//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SFIcrc16
//
// Description:
//
// Implementation
//
// Author: Andrew Vajoczki
// Date:   Jan 13, 2010
//
//---------------------------------------------------------------------------
#include "SFIprecomp.h"
#pragma hdrstop

// Uncomment this if you want to regenerate code for crcTable.
//#define CHM_GENERATE_CRC_TABLE

#include "SFIcrc16.h"
#include <assert.h>

#ifdef CHM_GENERATE_CRC_TABLE
#include <stdio.h>
#endif

/**********************************************************************
 *
 * Filename:    crc.h
 *
 * Description: A header file describing the various CRC standards.
 *
 * Notes:
 *
 *
 * Copyright (c) 2000 by Michael Barr.  This software is placed into
 * the public domain and may be used for any purpose.  However, this
 * notice must not be changed or removed and no warranty is either
 * expressed or implied by its publication or distribution.
 **********************************************************************/

#undef FALSE
#undef TRUE

#define FALSE    0
#define TRUE    !FALSE

/*
 * Select the CRC standard from the list that follows.
 */
#undef CRC32
#define CRC16
#undef CRC_CCITT

#if defined(CRC_CCITT)

//typedef unsigned short  crc;
typedef COLuint16  crc;

#define CRC_NAME           "CRC-CCITT"
#define POLYNOMIAL          0x1021
#define INITIAL_REMAINDER   0xFFFF
#define FINAL_XOR_VALUE     0x0000
#define REFLECT_DATA        FALSE
#define REFLECT_REMAINDER   FALSE
#define CHECK_VALUE         0x29B1

#elif defined(CRC16)

//typedef unsigned short  crc;
typedef COLuint16  crc;

#define CRC_NAME            "CRC-16"
#define POLYNOMIAL          0x8005
#define INITIAL_REMAINDER   0x0000
#define FINAL_XOR_VALUE     0x0000
#define REFLECT_DATA        TRUE
#define REFLECT_REMAINDER   TRUE
#define CHECK_VALUE         0xBB3D

#elif defined(CRC32)

//typedef unsigned long  crc;
typedef COLuint32  crc;

#define CRC_NAME            "CRC-32"
#define POLYNOMIAL          0x04C11DB7
#define INITIAL_REMAINDER   0xFFFFFFFF
#define FINAL_XOR_VALUE     0xFFFFFFFF
#define REFLECT_DATA        TRUE
#define REFLECT_REMAINDER   TRUE
#define CHECK_VALUE         0xCBF43926

#else

#error "One of CRC_CCITT, CRC16, or CRC32 must be #define'd."

#endif

/**********************************************************************
 *
 * Filename:    crc.c
 *
 * Description: Slow and fast implementations of the CRC standards.
 *
 * Notes:       The parameters for each supported CRC standard are
 *            defined in the header file crc.h.  The implementations
 *            here should stand up to further additions to that list.
 *
 *
 * Copyright (c) 2000 by Michael Barr.  This software is placed into
 * the public domain and may be used for any purpose.  However, this
 * notice must not be changed or removed and no warranty is either
 * expressed or implied by its publication or distribution.
 **********************************************************************/

/*
 * Derive parameters from the standard-specific parameters in crc.h.
 */
#define WIDTH    (8 * sizeof(crc))
#define TOPBIT   (1 << (WIDTH - 1))

#if (REFLECT_DATA == TRUE)
#  undef  REFLECT_DATA
#  if 1
#    define REFLECT_DATA(X)       ((unsigned char)s_reflectByteTable[(X)])
//#  define REFLECT_DATA(X)       doublecheck_reflect(X)
#  else
#    define REFLECT_DATA(X)       ((unsigned char) reflect((X), 8))
#  endif
#else
#  undef  REFLECT_DATA
#  define REFLECT_DATA(X)         (X)
#endif

#if (REFLECT_REMAINDER == TRUE)
#undef  REFLECT_REMAINDER
#define REFLECT_REMAINDER(X)   ((crc) reflect((X), WIDTH))
#else
#undef  REFLECT_REMAINDER
#define REFLECT_REMAINDER(X)   (X)
#endif


/*********************************************************************
 *
 * Function:    reflect()
 *
 * Description: Reorder the bits of a binary sequence, by reflecting
 *            them about the middle position.
 *
 * Notes:      No checking is done that nBits <= 32.
 *
 * Returns:      The reflection of the original data.
 *
 *********************************************************************/
static unsigned long reflect(unsigned long data, unsigned char nBits)
{
   unsigned long  reflection = 0x00000000;
   unsigned char  bit;

   /*
    * Reflect the data about the center bit.
    */
   for (bit = 0; bit < nBits; ++bit)
   {
      /*
       * If the LSB bit is set, set the reflection of it.
       */
      if (data & 0x01)
      {
         reflection |= (1 << ((nBits - 1) - bit));
      }

      data = (data >> 1);
   }

   return (reflection);

}   /* reflect() */

//#define GENERATE_CRC_REFLECT_TABLE
#ifdef GENERATE_CRC_REFLECT_TABLE
static int generateCrcReflectTable()
{
   assert(sizeof(crc) == 2);
   const int length = 256;
   const int wrap = 8;
   printf("static unsigned char s_reflectByteTable[%d] =\n{", length);
   for (unsigned i = 0; i < length; ++i)
   {
      printf((i%wrap == 0) ? "\n   " : ", ");
      char buf[32];
      snprintf(buf, 32, "0x%02x", (unsigned)reflect(i,8));
      printf(buf);
      if (i%wrap == (wrap-1) && i != (length-1)) printf(", ");
   }
   printf("\n};\n");
   return 1;
}
static int foo = generateCrcReflectTable();
#endif

// #define GENERATE_CRC_REFLECT_TABLE to generate table
static unsigned char s_reflectByteTable[256] =
{
   0x00, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0,
   0x10, 0x90, 0x50, 0xd0, 0x30, 0xb0, 0x70, 0xf0,
   0x08, 0x88, 0x48, 0xc8, 0x28, 0xa8, 0x68, 0xe8,
   0x18, 0x98, 0x58, 0xd8, 0x38, 0xb8, 0x78, 0xf8,
   0x04, 0x84, 0x44, 0xc4, 0x24, 0xa4, 0x64, 0xe4,
   0x14, 0x94, 0x54, 0xd4, 0x34, 0xb4, 0x74, 0xf4,
   0x0c, 0x8c, 0x4c, 0xcc, 0x2c, 0xac, 0x6c, 0xec,
   0x1c, 0x9c, 0x5c, 0xdc, 0x3c, 0xbc, 0x7c, 0xfc,
   0x02, 0x82, 0x42, 0xc2, 0x22, 0xa2, 0x62, 0xe2,
   0x12, 0x92, 0x52, 0xd2, 0x32, 0xb2, 0x72, 0xf2,
   0x0a, 0x8a, 0x4a, 0xca, 0x2a, 0xaa, 0x6a, 0xea,
   0x1a, 0x9a, 0x5a, 0xda, 0x3a, 0xba, 0x7a, 0xfa,
   0x06, 0x86, 0x46, 0xc6, 0x26, 0xa6, 0x66, 0xe6,
   0x16, 0x96, 0x56, 0xd6, 0x36, 0xb6, 0x76, 0xf6,
   0x0e, 0x8e, 0x4e, 0xce, 0x2e, 0xae, 0x6e, 0xee,
   0x1e, 0x9e, 0x5e, 0xde, 0x3e, 0xbe, 0x7e, 0xfe,
   0x01, 0x81, 0x41, 0xc1, 0x21, 0xa1, 0x61, 0xe1,
   0x11, 0x91, 0x51, 0xd1, 0x31, 0xb1, 0x71, 0xf1,
   0x09, 0x89, 0x49, 0xc9, 0x29, 0xa9, 0x69, 0xe9,
   0x19, 0x99, 0x59, 0xd9, 0x39, 0xb9, 0x79, 0xf9,
   0x05, 0x85, 0x45, 0xc5, 0x25, 0xa5, 0x65, 0xe5,
   0x15, 0x95, 0x55, 0xd5, 0x35, 0xb5, 0x75, 0xf5,
   0x0d, 0x8d, 0x4d, 0xcd, 0x2d, 0xad, 0x6d, 0xed,
   0x1d, 0x9d, 0x5d, 0xdd, 0x3d, 0xbd, 0x7d, 0xfd,
   0x03, 0x83, 0x43, 0xc3, 0x23, 0xa3, 0x63, 0xe3,
   0x13, 0x93, 0x53, 0xd3, 0x33, 0xb3, 0x73, 0xf3,
   0x0b, 0x8b, 0x4b, 0xcb, 0x2b, 0xab, 0x6b, 0xeb,
   0x1b, 0x9b, 0x5b, 0xdb, 0x3b, 0xbb, 0x7b, 0xfb,
   0x07, 0x87, 0x47, 0xc7, 0x27, 0xa7, 0x67, 0xe7,
   0x17, 0x97, 0x57, 0xd7, 0x37, 0xb7, 0x77, 0xf7,
   0x0f, 0x8f, 0x4f, 0xcf, 0x2f, 0xaf, 0x6f, 0xef,
   0x1f, 0x9f, 0x5f, 0xdf, 0x3f, 0xbf, 0x7f, 0xff
};

#if 0
static unsigned char doublecheck_reflect(unsigned x)
{
   assert(x <= 255);
   const unsigned original = reflect(x, 8);
   const unsigned fastway = s_reflectByteTable[x];
   assert(original == fastway);
   return fastway;
}
#endif

/*********************************************************************
 *
 * Function:    crcSlow()
 *
 * Description: Compute the CRC of a given message.
 *
 * Notes:
 *
 * Returns:      The CRC of the message.
 *
 *********************************************************************/
#if 0
static crc crcSlow(unsigned char const message[], int nBytes)
{
   crc            remainder = INITIAL_REMAINDER;
   int            byte;
   unsigned char  bit;

    /*
     * Perform modulo-2 division, a byte at a time.
     */
    for (byte = 0; byte < nBytes; ++byte)
    {
        /*
         * Bring the next byte into the remainder.
         */
        remainder ^= (REFLECT_DATA(message[byte]) << (WIDTH - 8));

        /*
         * Perform modulo-2 division, a bit at a time.
         */
        for (bit = 8; bit > 0; --bit)
        {
            /*
             * Try to divide the current data bit.
             */
            if (remainder & TOPBIT)
            {
                remainder = (remainder << 1) ^ POLYNOMIAL;
            }
            else
            {
                remainder = (remainder << 1);
            }
        }
    }

    /*
     * The final remainder is the CRC result.
     */
    return (REFLECT_REMAINDER(remainder) ^ FINAL_XOR_VALUE);

}   /* crcSlow() */
#endif

#ifdef CHM_GENERATE_CRC_TABLE

static crc crcTable[256];

/*********************************************************************
 *
 * Function:    crcInit()
 *
 * Description: Populate the partial CRC lookup table.
 *
 * Notes:      This function must be rerun any time the CRC standard
 *            is changed.  If desired, it can be run "offline" and
 *            the table results stored in an embedded system's ROM.
 *
 * Returns:      None defined.
 *
 *********************************************************************/
static void crcInit(void)
{
   crc            remainder;
   int            dividend;
   unsigned char  bit;

    /*
     * Compute the remainder of each possible dividend.
     */
    for (dividend = 0; dividend < 256; ++dividend)
    {
        /*
         * Start with the dividend followed by zeros.
         */
        remainder = dividend << (WIDTH - 8);

        /*
         * Perform modulo-2 division, a bit at a time.
         */
        for (bit = 8; bit > 0; --bit)
        {
            /*
             * Try to divide the current data bit.
             */
            if (remainder & TOPBIT)
            {
                remainder = (remainder << 1) ^ POLYNOMIAL;
            }
            else
            {
                remainder = (remainder << 1);
            }
        }

        /*
         * Store the result into the table.
         */
        crcTable[dividend] = remainder;
    }

}   /* crcInit() */

#else

//
// This table is generated code.
// See CHM_GENERATE_CRC_TABLE.
//
static crc crcTable[256] =
{
   0x0000, 0x8005, 0x800f, 0x000a,
   0x801b, 0x001e, 0x0014, 0x8011,
   0x8033, 0x0036, 0x003c, 0x8039,
   0x0028, 0x802d, 0x8027, 0x0022,
   0x8063, 0x0066, 0x006c, 0x8069,
   0x0078, 0x807d, 0x8077, 0x0072,
   0x0050, 0x8055, 0x805f, 0x005a,
   0x804b, 0x004e, 0x0044, 0x8041,
   0x80c3, 0x00c6, 0x00cc, 0x80c9,
   0x00d8, 0x80dd, 0x80d7, 0x00d2,
   0x00f0, 0x80f5, 0x80ff, 0x00fa,
   0x80eb, 0x00ee, 0x00e4, 0x80e1,
   0x00a0, 0x80a5, 0x80af, 0x00aa,
   0x80bb, 0x00be, 0x00b4, 0x80b1,
   0x8093, 0x0096, 0x009c, 0x8099,
   0x0088, 0x808d, 0x8087, 0x0082,
   0x8183, 0x0186, 0x018c, 0x8189,
   0x0198, 0x819d, 0x8197, 0x0192,
   0x01b0, 0x81b5, 0x81bf, 0x01ba,
   0x81ab, 0x01ae, 0x01a4, 0x81a1,
   0x01e0, 0x81e5, 0x81ef, 0x01ea,
   0x81fb, 0x01fe, 0x01f4, 0x81f1,
   0x81d3, 0x01d6, 0x01dc, 0x81d9,
   0x01c8, 0x81cd, 0x81c7, 0x01c2,
   0x0140, 0x8145, 0x814f, 0x014a,
   0x815b, 0x015e, 0x0154, 0x8151,
   0x8173, 0x0176, 0x017c, 0x8179,
   0x0168, 0x816d, 0x8167, 0x0162,
   0x8123, 0x0126, 0x012c, 0x8129,
   0x0138, 0x813d, 0x8137, 0x0132,
   0x0110, 0x8115, 0x811f, 0x011a,
   0x810b, 0x010e, 0x0104, 0x8101,
   0x8303, 0x0306, 0x030c, 0x8309,
   0x0318, 0x831d, 0x8317, 0x0312,
   0x0330, 0x8335, 0x833f, 0x033a,
   0x832b, 0x032e, 0x0324, 0x8321,
   0x0360, 0x8365, 0x836f, 0x036a,
   0x837b, 0x037e, 0x0374, 0x8371,
   0x8353, 0x0356, 0x035c, 0x8359,
   0x0348, 0x834d, 0x8347, 0x0342,
   0x03c0, 0x83c5, 0x83cf, 0x03ca,
   0x83db, 0x03de, 0x03d4, 0x83d1,
   0x83f3, 0x03f6, 0x03fc, 0x83f9,
   0x03e8, 0x83ed, 0x83e7, 0x03e2,
   0x83a3, 0x03a6, 0x03ac, 0x83a9,
   0x03b8, 0x83bd, 0x83b7, 0x03b2,
   0x0390, 0x8395, 0x839f, 0x039a,
   0x838b, 0x038e, 0x0384, 0x8381,
   0x0280, 0x8285, 0x828f, 0x028a,
   0x829b, 0x029e, 0x0294, 0x8291,
   0x82b3, 0x02b6, 0x02bc, 0x82b9,
   0x02a8, 0x82ad, 0x82a7, 0x02a2,
   0x82e3, 0x02e6, 0x02ec, 0x82e9,
   0x02f8, 0x82fd, 0x82f7, 0x02f2,
   0x02d0, 0x82d5, 0x82df, 0x02da,
   0x82cb, 0x02ce, 0x02c4, 0x82c1,
   0x8243, 0x0246, 0x024c, 0x8249,
   0x0258, 0x825d, 0x8257, 0x0252,
   0x0270, 0x8275, 0x827f, 0x027a,
   0x826b, 0x026e, 0x0264, 0x8261,
   0x0220, 0x8225, 0x822f, 0x022a,
   0x823b, 0x023e, 0x0234, 0x8231,
   0x8213, 0x0216, 0x021c, 0x8219,
   0x0208, 0x820d, 0x8207, 0x0202
};

#endif

/*********************************************************************
 *
 * Function:    crcFast()
 *
 * Description: Compute the CRC of a given message.
 *
 * Notes:      crcInit() must be called first.
 *
 * Returns:      The CRC of the message.
 *
 *********************************************************************/
static crc crcFast(const unsigned char* message, int nBytes)
{
   crc            remainder = INITIAL_REMAINDER;
   unsigned char  data;
   int            byte;

    /*
     * Divide the message by the polynomial, a byte at a time.
     */
   for (byte = 0; byte < nBytes; ++byte)
   {
      data = REFLECT_DATA(message[byte]) ^ (remainder >> (WIDTH - 8));
      remainder = crcTable[data] ^ (remainder << 8);
   }

   /*
    * The final remainder is the CRC.
    */
   return (REFLECT_REMAINDER(remainder) ^ FINAL_XOR_VALUE);

}   /* crcFast() */

#ifdef CHM_GENERATE_CRC_TABLE
static void dump(const char* arrayName, const crc* array, int length)
{
   assert(sizeof(crc) == 2);
   COLcout << arrayName << '[' << length << "] =\n{";
   for (int i = 0; i < length; ++i)
   {
      COLcout << ((i%4 == 0) ? "\n   " : ", ");
      char buf[32];
      snprintf(buf, 32, "0x%04x", (unsigned)array[i]);
      COLcout << buf;
      if (i%4 == 3 && i != (length-1)) COLcout << ',';
   }
   COLcout << "\n};\n";
}
#endif

COLuint32 SFIcrc16::calculate(const void* Data, int CountBytes)
{
   assert(sizeof(crc) == 2);

#ifdef CHM_GENERATE_CRC_TABLE
   crcInit();
   dump("static crc crcTable", crcTable, 256);
#endif

   return crcFast((const unsigned char*)Data, CountBytes);
}

SFIcrc16::SFIcrc16()
{
   assert(sizeof(crc) == sizeof(COLuint16));
   reset();
}

SFIcrc16::~SFIcrc16()
{
}

COLuint32 SFIcrc16::write(const void* cpBuffer, COLuint32 nBytes)
{
   const unsigned char* message = (const unsigned char*)cpBuffer;

   // Divide the message by the polynomial, a byte at a time.
   for (int byte = 0; byte < (int)nBytes; ++byte)
   {
      unsigned char data = REFLECT_DATA(message[byte]) ^ (Remainder >> (WIDTH - 8));
      Remainder = crcTable[data] ^ (Remainder << 8);
   }
   return nBytes;
}

COLuint32 SFIcrc16::digest()
{
   return (REFLECT_REMAINDER(Remainder) ^ FINAL_XOR_VALUE);
}

void SFIcrc16::reset()
{
   Remainder = INITIAL_REMAINDER;
}
