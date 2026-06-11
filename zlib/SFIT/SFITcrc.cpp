//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testSFIcrc.cpp
//
// Description:
//
// This is a test of SFIcrc
//
// Author: Andrew Vajoczki
// Date:   December 22, 2008
//
//
//---------------------------------------------------------------------------

#include "SFITprecomp.h"
#pragma hdrstop

#include <SFI/SFIcrc.h>
#include <SFI/SFIcrc16.h>

#include <TST/TSTtestCollection.h>
#include <TST/TSThelpers.h>
#include <TST/TSTtestFunction.h>

void testCRCcalculate()
{
   TST_ASSERT_MESSAGE(SFIcrc::calculate("123456789", 9) == 0xCBF43926, "testSFIcrc failed");
}

void testCRCwrite()
{
   SFIcrc Crc;

   Crc.write("1", 1);
   Crc.write("2", 1);
   Crc.write("3", 1);
   Crc.write("4", 1);
   Crc.write("5", 1);
   Crc.write("6", 1);
   Crc.write("7", 1);
   Crc.write("8", 1);
   Crc.write("9", 1);
   TST_ASSERT_MESSAGE(Crc.digest() == 0xCBF43926, "testCRCwrite single byte writes test failed");

   Crc.reset();
   TST_ASSERT_MESSAGE(Crc.digest() != 0xCBF43926, "testCRCwrite reset test failed");

   Crc.write("123456789", 9);
   TST_ASSERT_MESSAGE(Crc.digest() == 0xCBF43926, "testCRCwrite single write test failed");

   Crc.reset();
   TST_ASSERT_MESSAGE(Crc.digest() != 0xCBF43926, "testCRCwrite reset test failed");

   Crc.write("123", 3);
   Crc.write("456789", 6);
   TST_ASSERT_MESSAGE(Crc.digest() == 0xCBF43926, "testCRCwrite two write test failed");
}

void testCRC16calculate()
{
   TST_ASSERT_EQUALS(0xbb3d, SFIcrc16::calculate("123456789", 9));
}

void testCRC16write()
{
   SFIcrc16 Crc;

   Crc.write("1", 1);
   Crc.write("2", 1);
   Crc.write("3", 1);
   Crc.write("4", 1);
   Crc.write("5", 1);
   Crc.write("6", 1);
   Crc.write("7", 1);
   Crc.write("8", 1);
   Crc.write("9", 1);
   TST_ASSERT_EQUALS(0xbb3d, Crc.digest()); // testCRC16write single byte writes test failed

   Crc.reset();
   TST_ASSERT_MESSAGE(Crc.digest() != 0xbb3d, "testCRC16write reset test failed");

   Crc.write("123456789", 9);
   TST_ASSERT_EQUALS(0xbb3d, Crc.digest()); // testCRC16write single write test failed

   Crc.reset();
   TST_ASSERT_MESSAGE(Crc.digest() != 0xbb3d, "testCRC16write reset test failed");

   Crc.write("123", 3);
   Crc.write("456789", 6);
   TST_ASSERT_EQUALS(0xbb3d, Crc.digest()); // testCRC16write two write test failed

   Crc.reset();
   TST_ASSERT_MESSAGE(Crc.digest() != 0xbb3d, "testCRC16write reset test failed");

   Crc.write("iNTERFACEWARE Iguana", 20);
   TST_ASSERT_EQUALS(0xAC41, Crc.digest());
}

TSTtestable* SFITcrc()
{
   TST_START_COLLECTION("SFITcrcTests")
      TST_ADD_TEST_FUNCTION(testCRCcalculate);
      TST_ADD_TEST_FUNCTION(testCRCwrite);

      TST_ADD_TEST_FUNCTION(testCRC16calculate);
      TST_ADD_TEST_FUNCTION(testCRC16write);
   TST_END_COLLECTION
}
