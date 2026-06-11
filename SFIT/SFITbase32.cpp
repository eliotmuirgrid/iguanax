//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SFITbase32
//
// Description:
//
// Implementation
//
// Author: Steven Dodd
// Date:   Friday, August 20th, 2010 @ 09:32:22 AM
//
//---------------------------------------------------------------------------
#include "SFITprecomp.h"
#pragma hdrstop

#include "SFITbase32.h"

#include <SFI/SFIbase32.h>

#include <TST/TSThelpers.h>
#include <TST/TSTtestCollection.h>
#include <TST/TSTtestFunction.h>

#include <COL/COLlog.h>
#include <COL/COLsinkString.h>
COL_LOG_MODULE;


static void testEncode()
{
   COLstring Destination;
   COLsinkString Sink(Destination);

   // Zeroes
   Destination.clear();
   SFIbase32::encode(Sink, (const COLuint8*)"\0", 1);
   TST_ASSERT_EQUALS("AA======", Destination);
   
   Destination.clear();
   SFIbase32::encode(Sink, (const COLuint8*)"\0\0", 2);
   TST_ASSERT_EQUALS("AAAA====", Destination);
   
   Destination.clear();
   SFIbase32::encode(Sink, (const COLuint8*)"\0\0\0", 3);
   TST_ASSERT_EQUALS("AAAAA===", Destination);

   Destination.clear();
   SFIbase32::encode(Sink, (const COLuint8*)"\0\0\0\0", 4);
   TST_ASSERT_EQUALS("AAAAAAA=", Destination);

   Destination.clear();
   SFIbase32::encode(Sink, (const COLuint8*)"\0\0\0\0\0", 5);
   TST_ASSERT_EQUALS("AAAAAAAA", Destination);

   Destination.clear();
   SFIbase32::encode(Sink, (const COLuint8*)"\0\0\0\0\0\0", 6);
   TST_ASSERT_EQUALS("AAAAAAAAAA======", Destination);

   // Ones
   Destination.clear();
   SFIbase32::encode(Sink, (const COLuint8*)"\xFF", 1);
   TST_ASSERT_EQUALS("74======", Destination); // 11111 11100
   
   Destination.clear();
   SFIbase32::encode(Sink, (const COLuint8*)"\xFF\xFF", 2);
   TST_ASSERT_EQUALS("777Q====", Destination); // 11111 11111 11111 10000
   
   Destination.clear();
   SFIbase32::encode(Sink, (const COLuint8*)"\xFF\xFF\xFF", 3);
   TST_ASSERT_EQUALS("77776===", Destination); // 11111 11111 11111 11111 11110

   Destination.clear();
   SFIbase32::encode(Sink, (const COLuint8*)"\xFF\xFF\xFF\xFF", 4);
   TST_ASSERT_EQUALS("777777Y=", Destination); // 11111 11111 11111 11111 11111 11111 11000

   Destination.clear();
   SFIbase32::encode(Sink, (const COLuint8*)"\xFF\xFF\xFF\xFF\xFF", 5);
   TST_ASSERT_EQUALS("77777777", Destination); 

   Destination.clear();
   SFIbase32::encode(Sink, (const COLuint8*)"\xFF\xFF\xFF\xFF\xFF\xFF", 6);
   TST_ASSERT_EQUALS("7777777774======", Destination); 

   // foobar
   Destination.clear();
   SFIbase32::encode(Sink, (const COLuint8*)"foo", 3);
   TST_ASSERT_EQUALS("MZXW6===", Destination);

   Destination.clear();
   SFIbase32::encode(Sink, (const COLuint8*)"foob", 4);
   TST_ASSERT_EQUALS("MZXW6YQ=", Destination);

   Destination.clear();
   SFIbase32::encode(Sink, (const COLuint8*)"fooba", 5);
   TST_ASSERT_EQUALS("MZXW6YTB", Destination);

   Destination.clear();
   SFIbase32::encode(Sink, (const COLuint8*)"foobar", 6);
   TST_ASSERT_EQUALS("MZXW6YTBOI======", Destination);

   // Alphabet
   Destination.clear();
   SFIbase32::encode(Sink, (const COLuint8*)"\x00\x44\x32\x14\xC7\x42\x54\xB6\x35\xCF\x84\x65\x3A\x56\xD7\xC6\x75\xBE\x77\xDF", 20);
   TST_ASSERT_EQUALS("ABCDEFGHIJKLMNOPQRSTUVWXYZ234567", Destination);
   
   Destination.clear();
   SFIbase32::encode(Sink, (const COLuint8*)"\xFF\xBB\xCD\xEB\x38\xBD\xAB\x49\xCA\x30\x7B\x9A\xC5\xA9\x28\x39\x8A\x41\x88\x20", 20);
   TST_ASSERT_EQUALS("765432ZYXWVUTSRQPONMLKJIHGFEDCBA", Destination);
   
}


TSTtestable* SFITbase32()
{
   TST_START_COLLECTION("SFITbase32")
      TST_ADD_TEST_FUNCTION(testEncode);
   TST_END_FIXTURE
};

