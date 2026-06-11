//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: COLhash
//
// Description:
//
// Implementation
//
// Author: Nasron Cheong
// Date:   March 2004 
//---------------------------------------------------------------------------
#include "COLhash.h"
#include "COLlog.h"
COL_LOG_MODULE;

#define COL_LOOKUP_HASH_HASH_VALUE_BITS 32

//COLstring specialization
template <>
COLindex COLhash<COLstring>::defaultHash(const COLstring& Key)
{
   //COL_FUNCTION(COLhash<COLstring>::defaultHash);
   COLuint32 Hash = 0;
   if (Key.size())
   {
      //TODO replace all one letter variables with meaningful names
      //TODO add some comments for block of code, eliminate magic values
      COLuint32 HashIncrement;
      COLuint32 ByteIncrement = sizeof(COLuint32);
      COLuint32 ByteIndex;
      COLuint32 c;
      COLuint32 DividingByte = Key.size() & 0xFFFC; // the largest multiple of 4 <= Key.size()
      const char* pStr = Key.c_str();

      //COL_VAR3(Hash,DividingByte, Key);
      // process 4 byte chunks of Key up to the DividingByte, hashing each into a 32 bit integer
      for (ByteIndex = 0; ByteIndex < DividingByte; ByteIndex += ByteIncrement)
      {
         HashIncrement = *(COLuint32*) (pStr + ByteIndex);
         Hash += HashIncrement;
         c = (int(HashIncrement >> 24) + (int(HashIncrement >> 16) & 0x00FF) + (int(HashIncrement >> 8) & 0x00FF) + (int(HashIncrement) & 0x00FF)) % (COL_LOOKUP_HASH_HASH_VALUE_BITS - 1) + 1;
         Hash = (Hash << (COL_LOOKUP_HASH_HASH_VALUE_BITS - c)) | (Hash >> c);
      }
      // process bytes remaining after DividingByte
      for (ByteIndex = DividingByte; ByteIndex < Key.size(); ByteIndex++)
      {
         Hash += pStr[ByteIndex];
         c = (pStr[ByteIndex] + ByteIndex) % (COL_LOOKUP_HASH_HASH_VALUE_BITS - 1) + 1;
         Hash = (Hash << (COL_LOOKUP_HASH_HASH_VALUE_BITS - c)) | (Hash >> c);
      }
   }
   COL_DBG("Key = " << Key << ", Hash = " << Hash);
   return Hash;   
}

//A Basic CRC hash we can use for other types
static const COLuint32 COL_LOOKUP_HASH_BYTE_WIDTH = 8; // Number of bits in a byte
static const COLuint32 COL_LOOKUP_HASH_WORD_WIDTH = sizeof (COLuint32) * COL_LOOKUP_HASH_BYTE_WIDTH;
static const COLuint32 COL_LOOKUP_HASH_CRC_HASH_SHIFT = 5;

static inline COLuint32 COLhashCRC(void* Key, COLuint32 Size)
{
   if(!Size)
   {
      return 0;
   }
   COLuint32 HashValue = 0;
   COLuint32 Index = 0;
   while (Index < (Size-1)) //note that 0 is always returned if Size = 1, so do not use for chars
   {
      COLuint32 leftShiftedValue = HashValue << COL_LOOKUP_HASH_CRC_HASH_SHIFT;
      COLuint32 rightShiftedValue = HashValue >> (COL_LOOKUP_HASH_WORD_WIDTH - COL_LOOKUP_HASH_CRC_HASH_SHIFT);
   
      // Put the shifted values together, and then XOR them with the
      // next key character
      HashValue = (leftShiftedValue | rightShiftedValue) ^ (((COLuint8*)Key)[Index+1]);
      Index++;
   }
   return HashValue;
}

static const COLuint32 SizeofCOLuint64 = sizeof(COLuint64);
static const COLuint32 SizeofCOLuint32 = sizeof(COLuint32);
static const COLuint32 SizeofCOLuint16 = sizeof(COLuint16);
static const COLuint32 SizeofCOLuint8 = sizeof(COLuint8);

//Integer specializations
template <>
COLindex COLhash<COLuint64>::defaultHash(const COLuint64& Key)
{
   return COLhashCRC((void*)&Key,SizeofCOLuint64);
}

template <>
COLindex COLhash<COLint64>::defaultHash(const COLint64& Key)
{
   return COLhashCRC((void*)&Key,SizeofCOLuint64);
}

template <>
COLindex COLhash<COLuint32>::defaultHash(const COLuint32& Key)
{
   return COLhashCRC((void*)&Key,SizeofCOLuint32);
}

template <>
COLindex COLhash<COLint32>::defaultHash(const COLint32& Key)
{
   return COLhashCRC((void*)&Key,SizeofCOLuint32);
}

template <>
COLindex COLhash<COLuint16>::defaultHash(const COLuint16& Key)
{
   return COLhashCRC((void*)&Key,SizeofCOLuint16);
}

template <>
COLindex COLhash<COLint16>::defaultHash(const COLint16& Key)
{
   return COLhashCRC((void*)&Key,SizeofCOLuint16);
}

//TODO, explain this better, get rid of magic value 3
//chars use the integer multiply algorithm
inline COLuint32 COLhashIntegerMultiply(COLuint32 Key)
{
   return Key * (Key + 3);
}

template <>
COLindex COLhash<COLuint8>::defaultHash(const COLuint8& Key)
{
   return COLhashIntegerMultiply(Key);
}

template <>
COLindex COLhash<COLint8>::defaultHash(const COLint8& Key)
{
   return COLhashIntegerMultiply(Key);
}
