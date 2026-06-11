//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testCOLostream.cpp
//
// Description:
//
// This is a test of COLostream
//
// Author: Eliot Muir
// Date:   Mon 02/09/2004 
//
//
//---------------------------------------------------------------------------

#include "COLexamplePrecomp.h"
#pragma hdrstop

#include <COL/COLstring.h>
#include <COL/COLsinkStandard.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void testCOLostreamBasic()
{
   COLstring Message;
   COLostream Stream(Message);
   Stream << "COLstring used to implement the sink interface";
   Stream.flush(); 
   TST_ASSERT(Message == "COLstring used to implement the sink interface");

   //test a instantiation of COLostream thats instantiated and destructed on the same line
   COLstring TestString;
   COLostream( TestString ) << "This is a test " << 123;
   TST_ASSERT(TestString == "This is a test 123");
}

void testCOLostreamInteger()
{
   COLstring String;
   COLostream Stream(String);
   
   String.clear();
   Stream << (COLuint16)65535 << ' ' 
          << (COLint16)-127 << ' '
          << (COLint16)127 << flush;
   TST_ASSERT_MESSAGE(String == "65535 -127 127","Instead got " << String);

   String.clear();
   Stream << (COLuint32)65536 << ' ' 
          << (COLint32)-127 << ' '
          << (COLint32)127 << flush;
   TST_ASSERT_MESSAGE(String == "65536 -127 127","Instead got " << String);
}

void testCOLostreamFloat()
{
   COLstring String;
   COLostream Stream(String);
   
   String.clear();
   Stream << (COLfloat32)2.0 << flush; 
   TST_ASSERT_MESSAGE(String == "2.000000","Instead got " << String);

   String.clear();
   Stream << (COLfloat64)2.0 << flush; 
   TST_ASSERT_MESSAGE(String == "2.000000","Instead got " << String);
   
   String.clear();
   Stream << (COLfloat32)-78.125 << flush; 
   TST_ASSERT_MESSAGE(String == "-78.125000","Instead got " << String);

   String.clear();
   Stream << (COLfloat64)-987654321.125 << flush; 
   TST_ASSERT_MESSAGE(String == "-987654321.125000","Instead got " << String);
}

void testCOLostreamPointer()
{
   COLstring String;
   COLostream Stream(String);

   String.clear();
   COLstring* Pointer = (COLstring*)0xdeadbeef;
   Stream << Pointer << flush;
   TST_ASSERT_MESSAGE(String == "0xdeadbeef","Instead got " << String);

   String.clear();
   void* VoidStartPointer = (COLstring*)0xdeadbeef;
   Stream << VoidStartPointer << flush;
   TST_ASSERT_MESSAGE(String == "0xdeadbeef","Instead got " << String);
}

void UnusedFunctionToExamineAssemblyCodeOfStreamingStringLiteral(COLostream& Stream)
{
   Stream << COL_T("Paper")
          << -54321
          << "traditional string."
          << COL_T("Scissors")
          << COL_T("Rock")
          << '?'
          << COL_T("fin");
}

void UnusedFunctionToExamineAssemblyCodeOfCOLstringStringLiteralInitialization(COLstring& Result)
{
   COLstring TestString = COL_T("Initial value.");
   TestString = COL_T("Different value.");
   TestString = "a traditional C string.";
   TestString = COL_T("Yet another value.");
   TestString += COL_T("Appending this value.");
   Result = TestString;
}

void testCOLostreamStringLiteral()
{
   COLstring ExpectedResult = COL_T("fred");
   TST_ASSERT_EQUALS("fred", ExpectedResult);

   ExpectedResult += COL_T("?!");
   TST_ASSERT_EQUALS("fred?!", ExpectedResult);

   ExpectedResult = COL_T("This is a C string literal. 123456789. This is another C string literal.");
   TST_ASSERT_EQUALS("This is a C string literal. 123456789. This is another C string literal.", ExpectedResult);

   COLstring TestString;
   COLostream TestStringStream(TestString);
   TestStringStream << COL_T("This is a C string literal. ")
      << 123456789 << COL_T(". This is another C string literal.");

   TST_ASSERT_EQUALS("This is a C string literal. 123456789. This is another C string literal.", TestString);
   TST_ASSERT_EQUALS(ExpectedResult, TestString);
}

void testCOLostreamString()
{
   // we are heap allocating a COLostreamString for valgrind testing to test memory accesses to COLostreamString.m_String
   COLostreamString* p = new COLostreamString(); 
   COLostreamString& oss = *p;

   TST_ASSERT_EQUALS(true, oss.c_str() != NULL);
   TST_ASSERT_EQUALS(0, oss.str().size());
   TST_ASSERT_EQUALS(0, oss.size());
   TST_ASSERT_EQUALS(oss.str().size(), oss.size());
   TST_ASSERT_EQUALS(COLstring(""), oss.str());

   oss << "hello " << 123;
   TST_ASSERT_EQUALS(true, oss.c_str() != NULL);
   TST_ASSERT_EQUALS("hello 123", oss.str());
   TST_ASSERT_EQUALS(9, oss.str().size());
   TST_ASSERT_EQUALS(9, oss.size());
   TST_ASSERT_EQUALS(oss.str().size(), oss.size());

   oss << COLstring("add a somewhat longer string to see what happens");
   TST_ASSERT_EQUALS(true, oss.c_str() != NULL);
   TST_ASSERT_EQUALS("hello 123add a somewhat longer string to see what happens", oss.str());
   TST_ASSERT_EQUALS(57, oss.size());
   TST_ASSERT_EQUALS(57, oss.str().size());
   TST_ASSERT_EQUALS(oss.str().size(), oss.size());

   delete p;
}

static void testDecimalFormats()
{
   COLostreamString oss;
   TST_ASSERT_EQUALS(10, oss.numberBase());

   oss << decimal;
   TST_ASSERT_EQUALS(10, oss.numberBase());

   oss.clear(); oss << (char)'A';
   TST_ASSERT_EQUALS("A", oss.str());

   oss.clear(); oss << (unsigned char)'A';
   TST_ASSERT_EQUALS("A", oss.str());

   oss.clear(); oss << (short)26000;         
   TST_ASSERT_EQUALS("26000", oss.str());

   oss.clear(); oss << (unsigned short)25000;
   TST_ASSERT_EQUALS("25000", oss.str());

   oss.clear(); oss << (int)(-12345678);      
   TST_ASSERT_EQUALS("-12345678", oss.str());

   oss.clear(); oss << (unsigned int)12345678;
   TST_ASSERT_EQUALS("12345678", oss.str());

   oss.clear(); oss << (long)(-12345678L);           
   TST_ASSERT_EQUALS("-12345678", oss.str());

   oss.clear(); oss << (unsigned long)(4294967295UL);
   TST_ASSERT_EQUALS("4294967295", oss.str());

   oss.clear(); oss << (long long)(-123456789012345ULL);
   TST_ASSERT_EQUALS("-123456789012345", oss.str());

   oss.clear(); oss << (unsigned long long)(18446744073709551615ULL);
   TST_ASSERT_EQUALS("18446744073709551615", oss.str());

   oss.clear(); oss << (float)(-1.234875);
   TST_ASSERT_EQUALS("-1.234875", oss.str());

   oss.clear(); oss << (double)(-1234567890.125875);
   TST_ASSERT_EQUALS("-1234567890.125875", oss.str());

   oss.clear(); oss << (void*)0xdeadbeef;
   TST_ASSERT_EQUALS("0xdeadbeef", oss.str());
}

static void testHexFormats()
{
   COLostreamString oss;
   TST_ASSERT_EQUALS(10, oss.numberBase());

   oss << hex;
   TST_ASSERT_EQUALS(16, oss.numberBase());

   oss.clear(); oss << (char)'A';
   TST_ASSERT_EQUALS("A", oss.str());

   oss.clear(); oss << (unsigned char)'A';
   TST_ASSERT_EQUALS("A", oss.str());

   oss.clear(); oss << (short)26000;         
   TST_ASSERT_EQUALS("6590", oss.str());

   oss.clear(); oss << (unsigned short)25000;
   TST_ASSERT_EQUALS("61A8", oss.str());

   oss.clear(); oss << (unsigned short)0xe90a;
   TST_ASSERT_EQUALS("E90A", oss.str());

   oss.clear(); oss << (int)(0x472b334);      
   TST_ASSERT_EQUALS("472B334", oss.str());

   oss.clear(); oss << (unsigned int)0xffffffff;
   TST_ASSERT_EQUALS("FFFFFFFF", oss.str());

   oss.clear(); oss << (long)(0xfe8eb3bcL);           
   TST_ASSERT_EQUALS("FE8EB3BC", oss.str());

   oss.clear(); oss << (unsigned long)(0xfedcbcdeUL);
   TST_ASSERT_EQUALS("FEDCBCDE", oss.str());

   oss.clear(); oss << (long long)(0xffeebbaa12345678LL);
   TST_ASSERT_EQUALS("FFEEBBAA12345678", oss.str());

   oss.clear(); oss << (unsigned long long)(0xfe3dbbaa12345679ULL);
   TST_ASSERT_EQUALS("FE3DBBAA12345679", oss.str());

   oss.clear(); oss << (float)(-1.234875);
   TST_ASSERT_EQUALS("-1.234875", oss.str());

   oss.clear(); oss << (double)(-1234567890.125875);
   TST_ASSERT_EQUALS("-1234567890.125875", oss.str());

   oss.clear(); oss << (void*)0xdeadbeef;
   TST_ASSERT_EQUALS("0xdeadbeef", oss.str());

   oss << hex;     TST_ASSERT_EQUALS(16, oss.numberBase());
   oss << decimal; TST_ASSERT_EQUALS(10, oss.numberBase());
   oss << decimal; TST_ASSERT_EQUALS(10, oss.numberBase());
   oss << hex;     TST_ASSERT_EQUALS(16, oss.numberBase());
}

static void testMixedFormats()
{
   COLostreamString oss;
   TST_ASSERT_EQUALS(10, oss.numberBase());

   oss << 12345 << "foo" << hex << "bar" << 0xabcdef << decimal << 987 << hex << 0xa7b304c << -29.123456;
   TST_ASSERT_EQUALS("12345foobarABCDEF987A7B304C-29.123456", oss.str());
   TST_ASSERT_EQUALS(16, oss.numberBase());

   oss << decimal << 27 << 0xffff;
   TST_ASSERT_EQUALS("12345foobarABCDEF987A7B304C-29.1234562765535", oss.str());
   TST_ASSERT_EQUALS(10, oss.numberBase());
}

TSTtestable* COLostreamTests()
{
   TST_START_COLLECTION("COLostream Tests")
      TST_ADD_TEST_FUNCTION(testCOLostreamBasic);
      TST_ADD_TEST_FUNCTION(testCOLostreamInteger);
      TST_ADD_TEST_FUNCTION(testCOLostreamFloat);
      TST_ADD_TEST_FUNCTION(testCOLostreamPointer);
      TST_ADD_TEST_FUNCTION(testCOLostreamStringLiteral);
      TST_ADD_TEST_FUNCTION(testCOLostreamString);
      TST_ADD_TEST_FUNCTION(testDecimalFormats);
      TST_ADD_TEST_FUNCTION(testHexFormats);
      TST_ADD_TEST_FUNCTION(testMixedFormats);
   TST_END_COLLECTION
}

