#ifndef __COL_ENDIAN_H__
#define __COL_ENDIAN_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: COLendian
//
// Author: (probably) Eliot Muir
//
// Description:
//
// Functions for converting between big and little endian and system
// formats, since different architectures use different formats,
// and we must accommodate for this nuance.
//
// Worth having a read and every C/C++ programmer should be aware of endian-ness.
//---------------------------------------------------------------------------
#include "COLminimumInclude.h"

inline void COLendianFlip(COLuint16& Value) {
   Value = ((Value & 0xFF) << 8) | ((Value & 0xFF00) >> 8);
}

inline void COLendianFlip(COLuint32& Value) {
   Value = ((Value & 0xFF) << 24) | ((Value & 0xFF00) << 8) |
      ((Value & 0xFF0000) >> 8) | ((Value & 0xFF000000) >> 24);
}

#  define COL_UINT64_C_LITERAL(C) C##ULL

inline void COLendianFlip(COLuint64& Value) {
   Value=
   ((Value &   COL_UINT64_C_LITERAL(0x00000000000000FF)) << 56) |
   ((Value &   COL_UINT64_C_LITERAL(0x000000000000FF00)) << 40) |
   ((Value &   COL_UINT64_C_LITERAL(0x0000000000FF0000)) << 24) |
   ((Value &   COL_UINT64_C_LITERAL(0x00000000FF000000)) << 8)  |
   ((Value &   COL_UINT64_C_LITERAL(0x000000FF00000000)) >> 8)  | 
   ((Value &   COL_UINT64_C_LITERAL(0x0000FF0000000000)) >> 24) |
   ((Value &   COL_UINT64_C_LITERAL(0x00FF000000000000)) >> 40) |
   ((Value &   COL_UINT64_C_LITERAL(0xFF00000000000000)) >> 56);
}

#ifdef WORDS_BIGENDIAN // i.e. ala Solaris

inline COLuint16 COLsystemToLittleEndian(COLuint16 value) {
   return ((value&0xFF) << 8)|((value&0xFF00)>>8) ;
}

inline COLuint32 COLsystemToLittleEndian (COLuint32 value) {
   return ((value&0xFF000000L)>>24)|((value&0xFF0000L)>>8)
          | ((value&0xFF00L)<<8) | ((value&0xFFL)<<24) ;
}

inline COLuint64 COLsystemToLittleEndian (COLuint64 value) {
   COLendianFlip(value);
   return value;
}

inline COLuint16 COLsystemToBigEndian(COLuint16 value) {
   return value ;
}

inline COLuint32 COLsystemToBigEndian(COLuint32 value) {
   return value ;
}

inline COLuint64 COLsystemToBigEndian(COLuint64 value) {
   return value ;
}

inline COLuint16 COLlittleEndianToSystem(COLuint16 value) {
   return ((value&0xFF) << 8)|((value&0xFF00)>>8) ;
}

inline COLuint32 COLlittleEndianToSystem(COLuint32 value) {
   return ((value&0xFF000000L)>>24)|((value&0xFF0000L)>>8)
           | ((value&0xFF00L)<<8) | ((value&0xFFL)<<24) ;
}

inline COLuint64 COLlittleEndianToSystem(COLuint64 value) {
   COLendianFlip(value);
   return value;
}

inline COLuint16 COLbigEndianToSystem(COLuint16 value) {
   return value ;
}

inline COLuint32 COLbigEndianToSystem(COLuint32 value) {
   return value ;
}

inline COLuint64 COLbigEndianToSystem(COLuint64 value) {
   return value ;
}

#else // we have a 'normal' - i.e Intel Endian system.

inline COLuint16 COLsystemToLittleEndian(COLuint16 value) {
   return value ;
}

inline COLuint32 COLsystemToLittleEndian(COLuint32 value) {
   return value ;
}

inline COLuint64 COLsystemToLittleEndian(COLuint64 value) {
   return value ;
}

inline COLuint16 COLsystemToBigEndian(COLuint16 value) {
   return (COLuint16) ((value&0xFF) << 8)|((value&0xFF00)>>8) ;
}

inline COLuint32 COLsystemToBigEndian(COLuint32 value) {
   return ((value&0xFF000000L)>>24)|((value&0xFF0000L)>>8)
      | ((value&0xFF00L)<<8) | ((value&0xFFL)<<24) ;
}

inline COLuint64 COLsystemToBigEndian(COLuint64 value) {
   COLendianFlip(value);
   return value;
}

inline COLuint16 COLlittleEndianToSystem(COLuint16 value) {
   return value ;
}

inline COLuint32 COLlittleEndianToSystem(COLuint32 value) {
   return value ;
}

inline COLuint64 COLlittleEndianToSystem(COLuint64 value) {
   return value ;
}

inline COLuint16 COLbigEndianToSystem(COLuint16 value) {
   return (COLuint16) ((value&0xFF) << 8)|((value&0xFF00)>>8) ;
}

inline COLuint32 COLbigEndianToSystem(COLuint32 value) {
   return ((value&0xFF000000L)>>24)|((value&0xFF0000L)>>8)
      | ((value&0xFF00L)<<8) | ((value&0xFFL)<<24) ;
}

inline COLuint64 COLbigEndianToSystem(COLuint64 value) {
   COLendianFlip(value);
   return value;
}

#endif // WORDS_BIGENDIAN 

#endif // end of defensive include

