//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SFIcrc
//
// Description:
//
// Implementation
//
// Author: Andrew Vajoczki
// Date:   December 22, 2008
//
//---------------------------------------------------------------------------
#include "SFIprecomp.h"
#pragma hdrstop

// Uncomment this if you want to regenerate code for crcTable.
// #define CHM_GENERATE_CRC_TABLE

#include "SFIcrc.h"
#include <assert.h>
//#include <stdio.h>

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
#define CRC32
#undef CRC16
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
   assert(sizeof(crc) == 4);
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
   0x00000000, 0x04c11db7, 0x09823b6e, 0x0d4326d9,
   0x130476dc, 0x17c56b6b, 0x1a864db2, 0x1e475005,
   0x2608edb8, 0x22c9f00f, 0x2f8ad6d6, 0x2b4bcb61,
   0x350c9b64, 0x31cd86d3, 0x3c8ea00a, 0x384fbdbd,
   0x4c11db70, 0x48d0c6c7, 0x4593e01e, 0x4152fda9,
   0x5f15adac, 0x5bd4b01b, 0x569796c2, 0x52568b75,
   0x6a1936c8, 0x6ed82b7f, 0x639b0da6, 0x675a1011,
   0x791d4014, 0x7ddc5da3, 0x709f7b7a, 0x745e66cd,
   0x9823b6e0, 0x9ce2ab57, 0x91a18d8e, 0x95609039,
   0x8b27c03c, 0x8fe6dd8b, 0x82a5fb52, 0x8664e6e5,
   0xbe2b5b58, 0xbaea46ef, 0xb7a96036, 0xb3687d81,
   0xad2f2d84, 0xa9ee3033, 0xa4ad16ea, 0xa06c0b5d,
   0xd4326d90, 0xd0f37027, 0xddb056fe, 0xd9714b49,
   0xc7361b4c, 0xc3f706fb, 0xceb42022, 0xca753d95,
   0xf23a8028, 0xf6fb9d9f, 0xfbb8bb46, 0xff79a6f1,
   0xe13ef6f4, 0xe5ffeb43, 0xe8bccd9a, 0xec7dd02d,
   0x34867077, 0x30476dc0, 0x3d044b19, 0x39c556ae,
   0x278206ab, 0x23431b1c, 0x2e003dc5, 0x2ac12072,
   0x128e9dcf, 0x164f8078, 0x1b0ca6a1, 0x1fcdbb16,
   0x018aeb13, 0x054bf6a4, 0x0808d07d, 0x0cc9cdca,
   0x7897ab07, 0x7c56b6b0, 0x71159069, 0x75d48dde,
   0x6b93dddb, 0x6f52c06c, 0x6211e6b5, 0x66d0fb02,
   0x5e9f46bf, 0x5a5e5b08, 0x571d7dd1, 0x53dc6066,
   0x4d9b3063, 0x495a2dd4, 0x44190b0d, 0x40d816ba,
   0xaca5c697, 0xa864db20, 0xa527fdf9, 0xa1e6e04e,
   0xbfa1b04b, 0xbb60adfc, 0xb6238b25, 0xb2e29692,
   0x8aad2b2f, 0x8e6c3698, 0x832f1041, 0x87ee0df6,
   0x99a95df3, 0x9d684044, 0x902b669d, 0x94ea7b2a,
   0xe0b41de7, 0xe4750050, 0xe9362689, 0xedf73b3e,
   0xf3b06b3b, 0xf771768c, 0xfa325055, 0xfef34de2,
   0xc6bcf05f, 0xc27dede8, 0xcf3ecb31, 0xcbffd686,
   0xd5b88683, 0xd1799b34, 0xdc3abded, 0xd8fba05a,
   0x690ce0ee, 0x6dcdfd59, 0x608edb80, 0x644fc637,
   0x7a089632, 0x7ec98b85, 0x738aad5c, 0x774bb0eb,
   0x4f040d56, 0x4bc510e1, 0x46863638, 0x42472b8f,
   0x5c007b8a, 0x58c1663d, 0x558240e4, 0x51435d53,
   0x251d3b9e, 0x21dc2629, 0x2c9f00f0, 0x285e1d47,
   0x36194d42, 0x32d850f5, 0x3f9b762c, 0x3b5a6b9b,
   0x0315d626, 0x07d4cb91, 0x0a97ed48, 0x0e56f0ff,
   0x1011a0fa, 0x14d0bd4d, 0x19939b94, 0x1d528623,
   0xf12f560e, 0xf5ee4bb9, 0xf8ad6d60, 0xfc6c70d7,
   0xe22b20d2, 0xe6ea3d65, 0xeba91bbc, 0xef68060b,
   0xd727bbb6, 0xd3e6a601, 0xdea580d8, 0xda649d6f,
   0xc423cd6a, 0xc0e2d0dd, 0xcda1f604, 0xc960ebb3,
   0xbd3e8d7e, 0xb9ff90c9, 0xb4bcb610, 0xb07daba7,
   0xae3afba2, 0xaafbe615, 0xa7b8c0cc, 0xa379dd7b,
   0x9b3660c6, 0x9ff77d71, 0x92b45ba8, 0x9675461f,
   0x8832161a, 0x8cf30bad, 0x81b02d74, 0x857130c3,
   0x5d8a9099, 0x594b8d2e, 0x5408abf7, 0x50c9b640,
   0x4e8ee645, 0x4a4ffbf2, 0x470cdd2b, 0x43cdc09c,
   0x7b827d21, 0x7f436096, 0x7200464f, 0x76c15bf8,
   0x68860bfd, 0x6c47164a, 0x61043093, 0x65c52d24,
   0x119b4be9, 0x155a565e, 0x18197087, 0x1cd86d30,
   0x029f3d35, 0x065e2082, 0x0b1d065b, 0x0fdc1bec,
   0x3793a651, 0x3352bbe6, 0x3e119d3f, 0x3ad08088,
   0x2497d08d, 0x2056cd3a, 0x2d15ebe3, 0x29d4f654,
   0xc5a92679, 0xc1683bce, 0xcc2b1d17, 0xc8ea00a0,
   0xd6ad50a5, 0xd26c4d12, 0xdf2f6bcb, 0xdbee767c,
   0xe3a1cbc1, 0xe760d676, 0xea23f0af, 0xeee2ed18,
   0xf0a5bd1d, 0xf464a0aa, 0xf9278673, 0xfde69bc4,
   0x89b8fd09, 0x8d79e0be, 0x803ac667, 0x84fbdbd0,
   0x9abc8bd5, 0x9e7d9662, 0x933eb0bb, 0x97ffad0c,
   0xafb010b1, 0xab710d06, 0xa6322bdf, 0xa2f33668,
   0xbcb4666d, 0xb8757bda, 0xb5365d03, 0xb1f740b4
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
   assert(sizeof(crc) == 4);
   COLcout << arrayName << '[' << length << "] =\n{";
   for (int i = 0; i < length; ++i)
   {
      COLcout << ((i%4 == 0) ? "\n   " : ", ");
      char buf[32];
      snprintf(buf, 32, "0x%08x", (unsigned)array[i]);
      COLcout << buf;
      if (i%4 == 3 && i != (length-1)) COLcout << ',';
   }
   COLcout << "\n};\n";
}
#endif

COLuint32 SFIcrc::calculate(const void* Data, int CountBytes)
{
   assert(sizeof(crc) == 4);

#ifdef CHM_GENERATE_CRC_TABLE
   crcInit();
   dump("static crc crcTable", crcTable, 256);
#endif

   return crcFast((const unsigned char*)Data, CountBytes);
}

SFIcrc::SFIcrc()
{
   assert(sizeof(crc) == sizeof(COLuint32));
   reset();
}

SFIcrc::~SFIcrc()
{
}

COLuint32 SFIcrc::write(const void* cpBuffer, COLuint32 nBytes)
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

COLuint32 SFIcrc::digest()
{
   return (REFLECT_REMAINDER(Remainder) ^ FINAL_XOR_VALUE);
}

void SFIcrc::reset()
{
   Remainder = INITIAL_REMAINDER;
}
