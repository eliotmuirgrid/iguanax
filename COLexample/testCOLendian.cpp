//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testCOLendian.cpp
//
// Description:
//
// Implementation
//
// Author: Steven Dodd
// Date:   January 16, 2010
//
//
//---------------------------------------------------------------------------

#include "COLexamplePrecomp.h"
#pragma hdrstop

#include <COL/COLendian.h>

static void test16()
{
#ifdef WORDS_BIGENDIAN
   const COLuint16 LittleEndian = 0x0102;
   const COLuint16 BigEndian = 0x0201;
   const COLuint16 SystemEndian = BigEndian;
#else
   const COLuint16 LittleEndian = 0x0201;
   const COLuint16 BigEndian = 0x0102;
   const COLuint16 SystemEndian = LittleEndian;
#endif

   COLuint16 TestData = LittleEndian;
   COLendianFlip(TestData);
   TST_ASSERT_EQUALS(BigEndian, TestData);

   TST_ASSERT_EQUALS(LittleEndian,  COLsystemToLittleEndian(SystemEndian));
   TST_ASSERT_EQUALS(BigEndian,     COLsystemToBigEndian(SystemEndian));
   TST_ASSERT_EQUALS(SystemEndian,  COLlittleEndianToSystem(LittleEndian));
   TST_ASSERT_EQUALS(SystemEndian,  COLbigEndianToSystem(BigEndian));
}

static void test32()
{
#ifdef WORDS_BIGENDIAN
   const COLuint32 LittleEndian = 0x01020304L;
   const COLuint32 BigEndian = 0x04030201L;
   const COLuint32 SystemEndian = BigEndian;
#else
   const COLuint32 LittleEndian = 0x04030201L;
   const COLuint32 BigEndian = 0x01020304L;
   const COLuint32 SystemEndian = LittleEndian;
#endif

   COLuint32 TestData = LittleEndian;
   COLendianFlip(TestData);
   TST_ASSERT_EQUALS(BigEndian, TestData);

   TST_ASSERT_EQUALS(LittleEndian,  COLsystemToLittleEndian(SystemEndian));
   TST_ASSERT_EQUALS(BigEndian,     COLsystemToBigEndian(SystemEndian));
   TST_ASSERT_EQUALS(SystemEndian,  COLlittleEndianToSystem(LittleEndian));
   TST_ASSERT_EQUALS(SystemEndian,  COLbigEndianToSystem(BigEndian));
}

static void test64()
{
#ifdef WORDS_BIGENDIAN
   const COLuint64 LittleEndian = 0x0102030405060708LL;
   const COLuint64 BigEndian = 0x0807060504030201LL;
   const COLuint64 SystemEndian = BigEndian;
#else
   const COLuint64 LittleEndian = COL_UINT64_C_LITERAL(0x0807060504030201);
   const COLuint64 BigEndian = COL_UINT64_C_LITERAL(0x0102030405060708);
   const COLuint64 SystemEndian = LittleEndian;
#endif

   COLuint64 TestData = LittleEndian;
   COLendianFlip(TestData);
   TST_ASSERT_EQUALS(BigEndian, TestData);

   TST_ASSERT_EQUALS(LittleEndian,  COLsystemToLittleEndian(SystemEndian));
   TST_ASSERT_EQUALS(BigEndian,     COLsystemToBigEndian(SystemEndian));
   TST_ASSERT_EQUALS(SystemEndian,  COLlittleEndianToSystem(LittleEndian));
   TST_ASSERT_EQUALS(SystemEndian,  COLbigEndianToSystem(BigEndian));
}

TSTtestable* COLendianTests()
{
   TST_START_COLLECTION("COLendianTests")
      TST_ADD_TEST_FUNCTION(test16);
      TST_ADD_TEST_FUNCTION(test32);
      TST_ADD_TEST_FUNCTION(test64);
   TST_END_COLLECTION
}

