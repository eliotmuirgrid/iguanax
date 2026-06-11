//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testCOLstring.cpp
//
// Description:
//
// This is a test of COLstring
//
// Author: George Ma
// Date:   Mon 02/12/2004
//
//---------------------------------------------------------------------------

#include "COLexamplePrecomp.h"
#pragma hdrstop

#include <TST/TSTtestCollection.h>
#include <TST/TSTtestFunction.h>
#include <TST/TSThelpers.h>

#include <COL/COLstring.h>
#include <COL/COLdateTime.h>
#include <COL/COLsplit.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;
#include <wchar.h>
#include <stdio.h>
#include <string.h>

const char* emptyString = "";
const char* basicString = "It was a dark and stormy night.";
const char* shortString = "Short string.";
const char* symbols = "!@#$a^&*";
const char* numbers = "-1123049";
const wchar_t* wideBasicString = L"It was a dark and stormy night.";
const wchar_t* wideCharacterString = L"This is a wide character string.";
const wchar_t* wideCharacterShort = L"Short string.";
const wchar_t* wideCharacterSymbols = L"!@#$a^&*";
const wchar_t* wideCharacterNumbers = L"1234567890";

void testCOLstringConstructors()
{
   // Default (empty) constructor
   COLstring emptyString;
   emptyString = basicString;

   // COLstring copy constructor
   COLstring copyString(basicString);
   TST_ASSERT_EQUALS(basicString, copyString);

   COLstring symbolString(symbols);
   TST_ASSERT_EQUALS(symbols, symbolString);

   COLstring numberString(numbers);
   TST_ASSERT_EQUALS(numbers, numberString);

   // C-string copy constructor
   const COLstring basicCString(copyString.c_str() + 9);
   TST_ASSERT_EQUALS("dark and stormy night.", basicCString);

   const COLstring symbolCString(symbolString.c_str() + 3);
   TST_ASSERT_EQUALS("$a^&*", symbolCString);

   const COLstring numberCString(numberString.c_str() + 2);
   TST_ASSERT_EQUALS("123049", numberCString);

   /* From sequence */

   // Length within range of the input string
   COLstring testString1(basicString, 10);
   TST_ASSERT_EQUALS("It was a d", testString1);

   // Length = 0
   COLstring testString2(basicString, 0);
   TST_ASSERT_EQUALS("", testString2);

   // Length = size of the source string
   COLstring testString3(basicString, 31);
   TST_ASSERT_EQUALS(basicString, testString3);

   char* stringBuffer = new char[20];
   ::memset(stringBuffer, 'A', 20);
   COLstring fromSequenceString(stringBuffer, 4);
   TST_ASSERT_EQUALS("AAAA", fromSequenceString);
   delete [] stringBuffer;
   stringBuffer = NULL;

   // Initialize to specified size with specified char constructor
   COLstring nullString(0, 'A');
   TST_ASSERT_EQUALS("", nullString);

   COLstring shortString(5, 'A');
   TST_ASSERT_EQUALS("AAAAA", shortString);

   COLstring longString1(15,'A');
   TST_ASSERT_EQUALS("AAAAAAAAAAAAAAA", longString1);

   COLstring longString2(16, 'A');
   TST_ASSERT_EQUALS("AAAAAAAAAAAAAAAA", longString2);

   COLstring longString3(17, 'A');
   TST_ASSERT_EQUALS("AAAAAAAAAAAAAAAAA", longString3);

   // Windows wide strings
   const COLstring testWideString(wideCharacterString);
   TST_ASSERT_EQUALS(wideCharacterString, testWideString);

   COLstring testWideSymbols(wideCharacterSymbols);
   TST_ASSERT_EQUALS(wideCharacterSymbols, testWideSymbols);

   COLstring testWideNumbers(wideCharacterNumbers);
   TST_ASSERT_EQUALS(wideCharacterNumbers, testWideNumbers);

   // COLtextLiteral Constructor
   COLtextLiteral emptyLiteral("", 0);
   COLtextLiteral basicLiteral(basicString, 31);
   COLtextLiteral symbolsLiteral(symbols, 8);
   COLtextLiteral numbersLiteral(numbers, 8);

   COLstring testEmptyLiteral(emptyLiteral);
   TST_ASSERT_EQUALS("", testEmptyLiteral);

   COLstring testBasicLiteral(basicLiteral);
   TST_ASSERT_EQUALS(basicString, testBasicLiteral);

   COLstring testSymbolsLiteral(symbolsLiteral);
   TST_ASSERT_EQUALS(symbols, testSymbolsLiteral);

   COLstring testNumbersLiteral(numbersLiteral);
   TST_ASSERT_EQUALS(numbers, testNumbersLiteral);
}

void testCOLstringSubstring(){
   COLstring COLBasicString(basicString);
   COLstring COLShortString(shortString);

   /* Substring with one parameter (begin position) */

   // Substring begin position = 0
   COLstring testZeroSubstring1(COLBasicString.substr(0));
   TST_ASSERT_EQUALS(COLBasicString, testZeroSubstring1);

   COLstring testZeroSubstring2(COLShortString.substr(0));
   TST_ASSERT_EQUALS(COLShortString, testZeroSubstring2);

   // Substring with begin position at the end of the string
   COLstring testEndSubstring1(COLBasicString.substr(31));
   TST_ASSERT_EQUALS("", testEndSubstring1);

   COLstring testEndSubstring2(COLShortString.substr(13));
   TST_ASSERT_EQUALS("", testEndSubstring2);

   // Substring with begin position beyond the string
   COLstring testBeyondSubstring1(COLBasicString.substr(50));
   TST_ASSERT_EQUALS("", testBeyondSubstring1);

   COLstring testBeyondSubstring2(COLShortString.substr(50));
   TST_ASSERT_EQUALS("", testBeyondSubstring2);

   /* Substring with two parameters (begin and substring length) */

   // begin = 0 and substring length = 0
   COLstring testZeroSubstring5(COLBasicString.substr(0, 0));
   TST_ASSERT_EQUALS("", testZeroSubstring5);

   COLstring testZeroSubstring6(COLShortString.substr(0, 0));
   TST_ASSERT_EQUALS("", testZeroSubstring6);

   // begin = 0 and 0 < (substring length < length of string)
   COLstring testNormalSubstring3(COLBasicString.substr(0, 5));
   TST_ASSERT_EQUALS("It wa", testNormalSubstring3);

   COLstring testNormalSubstring4(COLShortString.substr(0, 5));
   TST_ASSERT_EQUALS("Short", testNormalSubstring4);

   // begin = 0 and substring length > length of string
   COLstring testLargeSubstring1(COLBasicString.substr(0, 50));
   TST_ASSERT_EQUALS(COLBasicString, testLargeSubstring1);

   COLstring testLargeSubstring2(COLShortString.substr(0, 50));
   TST_ASSERT_EQUALS(COLShortString, testLargeSubstring2);

   // (0 < begin < length of string) and
   // (0 < substring length < length of string)
   COLstring testRegularSubstring1(COLBasicString.substr(1, 10));
   TST_ASSERT_EQUALS("t was a da", testRegularSubstring1);

   COLstring testRegularSubstring2(COLShortString.substr(2, 4));
   TST_ASSERT_EQUALS("ort ", testRegularSubstring2);

   // (0 < begin < length of string) and substring length > length of string
   COLstring testLargeSubstring3(COLBasicString.substr(10, 100));
   TST_ASSERT_EQUALS("ark and stormy night.", testLargeSubstring3);

   COLstring testLargeSubstring4(COLShortString.substr(5, 100));
   TST_ASSERT_EQUALS(" string.", testLargeSubstring4);

   // begin > length of string and substring length = 0
   COLstring testLargeBegin3(COLBasicString.substr(50, 0));
   TST_ASSERT_EQUALS("", testLargeBegin3);

   COLstring testLargeBegin4(COLShortString.substr(50, 0));
   TST_ASSERT_EQUALS("", testLargeBegin4);

   // begin > length of string and (0 < substring < length of string)
   COLstring testLargeBegin5(COLBasicString.substr(50, 10));
   TST_ASSERT_EQUALS("", testLargeBegin5);

   COLstring testLargeBegin6(COLShortString.substr(50, 4));
   TST_ASSERT_EQUALS("", testLargeBegin6);

   // begin > length of string and substring length > length of string
   COLstring testLargeBegin7(COLBasicString.substr(80, 50));
   TST_ASSERT_EQUALS("", testLargeBegin7);

   COLstring testLargeBegin8(COLShortString.substr(80, 50));
   TST_ASSERT_EQUALS("", testLargeBegin8);

   // Test COLstring::substr() length argument can be npos.
   // Scenario seen in NODL, TRAN libraries.
   COLstring InUrl = "http://interfaceware.com";
   COLstring Url = InUrl.substr(0,InUrl.find("?"));
   TST_ASSERT_EQUALS("http://interfaceware.com", Url);

   const COLstring str = "It was a dark and story night.";

   // test negative begin position
   TST_ASSERT_EQUALS(COLstring("It was"), str.substr((unsigned)(0),6));
   TST_ASSERT_EQUALS(COLstring(""), str.substr((unsigned)(-1),3));
   TST_ASSERT_EQUALS(COLstring(""), str.substr((unsigned)(-2),3));
   TST_ASSERT_EQUALS(COLstring(""), str.substr((unsigned)(-1),3000000));
   TST_ASSERT_EQUALS(COLstring(""), str.substr((unsigned)(-2),3000000));
   TST_ASSERT_EQUALS(COLstring(""), str.substr((unsigned)(-500001),3000000));
   TST_ASSERT_EQUALS(COLstring(""), str.substr((unsigned)(-500001),3000000));

   // test begin position larger than string length
   TST_ASSERT_EQUALS(COLstring("igh"), str.substr(25, 3));
   TST_ASSERT_EQUALS(COLstring("ght"), str.substr(26, 3));
   TST_ASSERT_EQUALS(COLstring("ht."), str.substr(27, 3));
   TST_ASSERT_EQUALS(COLstring("t."), str.substr(28, 3));
   TST_ASSERT_EQUALS(COLstring("."), str.substr(29, 3));
   TST_ASSERT_EQUALS(COLstring(""),  str.substr(30, 3));
   TST_ASSERT_EQUALS(COLstring(""),  str.substr(31, 3));
   TST_ASSERT_EQUALS(COLstring(""),  str.substr(90000001, 3));
   TST_ASSERT_EQUALS(COLstring(""),  str.substr(90000001, 3));
   TST_ASSERT_EQUALS(COLstring(""),  str.substr(90000001, 9999999));
   TST_ASSERT_EQUALS(COLstring(""),  str.substr(90000001, -9999999));

   // Test length too long.
   for (int ii = 0; ii < 4000; ii++) {
      TST_ASSERT_EQUALS(COLstring("night."), str.substr(24, 6+ii));
   }
   TST_ASSERT_EQUALS(COLstring("night"), str.substr(24, 5));

   // Test negative length results are deterministic.
   for (int ii = -1; ii > -4000; ii--) {
      // These results seem wrong, but are consistent with legacy COLstring::substr()
      // behaviour where negative lengths are large unsigned numbers.
      // Note: negative lengths should never happen in practice.
      TST_ASSERT_EQUALS(COLstring("night."), str.substr(24, ii));
   }
   TST_ASSERT_EQUALS(COLstring("night"), str.substr(24, 5));
   TST_ASSERT_EQUALS(COLstring(""), str.substr(24, 0));
}

/*
inline COLstring operator+(const char*      a, const COLstring& b) { return COLstring(a).append(b);   }
inline COLstring operator+(const COLstring& a, const char*      b) { return COLstring(a).append(b);   }
inline COLstring operator+(char             a, const COLstring& b) { return COLstring(1,a).append(b); }
inline COLstring operator+(const COLstring& a, char             b) { return COLstring(a).append(1,b); }
inline COLstring operator+(const COLstring& a, const COLstring& b) { return COLstring(a).append(b);   }
*/

void testCOLstringAdd(){
   TST_ASSERT_EQUALS("The quick brown dog jumps over the lazy fox",
      "The quick brown dog" + COLstring(" jumps over the lazy fox"));

   TST_ASSERT_EQUALS("The quick brown fox jumps over the lazy dog",
      COLstring("The quick brown fox") + " jumps over the lazy dog");

   TST_ASSERT_EQUALS(COLstring("abcdef"), 'a' + COLstring("bcdef"));
   TST_ASSERT_EQUALS("abcdef", 'a' + COLstring("bcdef"));

   TST_ASSERT_EQUALS("12345678901234567", COLstring("1234567890123456") + "7");
   TST_ASSERT_EQUALS("1234567890123456",  COLstring("123456789012345") + "6");
   TST_ASSERT_EQUALS("123456789012345",   COLstring("12345678901234") + "5");
   TST_ASSERT_EQUALS("12345678901234",    COLstring("1234567890123") + "4");
   TST_ASSERT_EQUALS("1234567890123",     COLstring("123456789012") + "3");
   TST_ASSERT_EQUALS("1234567890123",     "1" + COLstring("234567890123"));
   TST_ASSERT_EQUALS("12345678901234",    "1" + COLstring("2345678901234"));
   TST_ASSERT_EQUALS("123456789012345",   "1" + COLstring("23456789012345"));
   TST_ASSERT_EQUALS("1234567890123456",  "1" + COLstring("234567890123456"));
   TST_ASSERT_EQUALS("12345678901234567", "1" + COLstring("2345678901234567"));

   TST_ASSERT_EQUALS("12345678901234567", COLstring("1234567890123456") + '7');
   TST_ASSERT_EQUALS("1234567890123456",  COLstring("123456789012345") + '6');
   TST_ASSERT_EQUALS("123456789012345",   COLstring("12345678901234") + '5');
   TST_ASSERT_EQUALS("12345678901234",    COLstring("1234567890123") + '4');
   TST_ASSERT_EQUALS("1234567890123",     COLstring("123456789012") + '3');
   TST_ASSERT_EQUALS("1234567890123",     '1' + COLstring("234567890123"));
   TST_ASSERT_EQUALS("12345678901234",    '1' + COLstring("2345678901234"));
   TST_ASSERT_EQUALS("123456789012345",   '1' + COLstring("23456789012345"));
   TST_ASSERT_EQUALS("1234567890123456",  '1' + COLstring("234567890123456"));
   TST_ASSERT_EQUALS("12345678901234567", '1' + COLstring("2345678901234567"));

   TST_ASSERT_EQUALS(COLstring("ABCDEF"), COLstring("ABCDE") + 'F');
   TST_ASSERT_EQUALS("ABCDEF", COLstring("ABCDE") + 'F');

   TST_ASSERT_EQUALS("the quick brown fox jumps over the lazy dog",
      COLstring("the ") + COLstring("quick brown fox jumps over the lazy dog"));
}

void testCOLstringAppend(){
   COLstring COLBasicString(basicString);
   COLstring COLShortString(shortString);
   COLstring COLSymbols(symbols);
   COLstring COLNumbers(numbers);
   COLstring testAppendString0;
   COLstring testAppendString1;
   COLstring testAppendString2;
   COLstring testAppendChar0;
   COLstring testAppendChar1;
   COLstring testAppendChar2;
   COLstring testAppendCharSize0;
   COLstring testAppendCharSize1;
   COLstring testAppendCharSize2;

   /* COLstring& append(const COLstring& s) */

   testAppendString0.append(testAppendString0);
   TST_ASSERT_EQUALS("", testAppendString0);

   testAppendString1.append(COLBasicString);
   TST_ASSERT_EQUALS(COLBasicString, testAppendString1);
   testAppendString1.append(COLShortString);
   TST_ASSERT_EQUALS("It was a dark and stormy night.Short string.",
      testAppendString1);

   testAppendString2.append(COLSymbols);
   TST_ASSERT_EQUALS(COLSymbols, testAppendString2);
   testAppendString2.append(COLNumbers);
   TST_ASSERT_EQUALS("!@#$a^&*-1123049", testAppendString2);

   /* COLstring& append(const char* s) */

   testAppendChar0.append(emptyString);
   TST_ASSERT_EQUALS("", testAppendChar0);

   testAppendChar1.append(basicString);
   TST_ASSERT_EQUALS(COLBasicString, testAppendChar1);
   testAppendChar1.append(shortString);
   TST_ASSERT_EQUALS("It was a dark and stormy night.Short string.",
      testAppendChar1);

   testAppendChar2.append(symbols);
   TST_ASSERT_EQUALS(COLSymbols, testAppendChar2);
   testAppendChar2.append(numbers);
   TST_ASSERT_EQUALS("!@#$a^&*-1123049", testAppendChar2);

   /* COLstring& append(const char* s, size_t n) */

   testAppendCharSize0.append(emptyString, 0);
   TST_ASSERT_EQUALS("", testAppendCharSize0);
   TST_ASSERT_EQUALS(0, testAppendCharSize0.size());
   testAppendCharSize0.append(basicString, 6);
   TST_ASSERT_EQUALS("It was", testAppendCharSize0);
   TST_ASSERT_EQUALS(6, testAppendCharSize0.size());

   testAppendCharSize1.append(basicString, 0);
   TST_ASSERT_EQUALS("", testAppendCharSize1);
   TST_ASSERT_EQUALS(0, testAppendCharSize1.size());
   testAppendCharSize1.append(basicString, 6);
   TST_ASSERT_EQUALS("It was", testAppendCharSize1);
   TST_ASSERT_EQUALS(6, testAppendCharSize1.size());
   testAppendCharSize1.append(shortString, 14);
   TST_ASSERT_EQUALS("It wasShort string.", testAppendCharSize1);
   TST_ASSERT_EQUALS(20, testAppendCharSize1.size());

   /* COLstring& append(size_t n, char c) */

   testAppendCharSize2.append(0, 'a');
   TST_ASSERT_EQUALS("", testAppendCharSize2);
   TST_ASSERT_EQUALS(0, testAppendCharSize2.size());
   testAppendCharSize2.append(10, 'a');
   TST_ASSERT_EQUALS("aaaaaaaaaa", testAppendCharSize2);
   TST_ASSERT_EQUALS(10, testAppendCharSize2.size());
   testAppendCharSize2.append(3, '$');
   TST_ASSERT_EQUALS("aaaaaaaaaa$$$", testAppendCharSize2);
   TST_ASSERT_EQUALS(13, testAppendCharSize2.size());
   testAppendCharSize2.append(1, '\0');
   TST_ASSERT_EQUALS("aaaaaaaaaa$$$\0", testAppendCharSize2);
   TST_ASSERT_EQUALS(14, testAppendCharSize2.size());
   testAppendCharSize2.append(5, 'x');
   TST_ASSERT_EQUALS("aaaaaaaaaa$$$\0xxxxx", testAppendCharSize2);
   TST_ASSERT_EQUALS(19, testAppendCharSize2.size());
}

void testCOLstringCapacity(){
   // Assign Capacity
   COLstring String1 = "123456789012345678901234567890123";
   TST_ASSERT_EQUALS(33, String1.size());
   TST_ASSERT(String1.capacity() > 33);
   String1 = "";
   TST_ASSERT_EQUALS(0, String1.size());
   TST_ASSERT(String1.capacity() > 33);

   String1 = "123456789012345678901234567890123";
   TST_ASSERT_EQUALS(33, String1.size());
   TST_ASSERT(String1.capacity() > 33);
   String1 = "abc";
   TST_ASSERT_EQUALS(3, String1.size());
   TST_ASSERT(String1.capacity() > 33);

   // Assign Null Capacity
   String1 = "123456789012345678901234567890123";
   TST_ASSERT_EQUALS(33, String1.size());
   TST_ASSERT(String1.capacity() > 33);
   String1 = (const char*)0;
   TST_ASSERT_EQUALS(0, String1.size());
   TST_ASSERT(String1.capacity() > 33);
}

void testCOLstringIsWhitespace(){
   COLstring string1 = "\t\t     \r\r\n\t  ";
   TST_ASSERT(string1.isWhitespace());

   COLstring string2 = "";
   TST_ASSERT(string2.isWhitespace());

   COLstring string3 = "!@#$a^&*  be  \t ";
   TST_ASSERT(!string3.isWhitespace());
}

void testCOLstringCase(){
   COLstring string;

   string = "";
   string.toUpperCase();
   TST_ASSERT_EQUALS("", string);
   string.toLowerCase();
   TST_ASSERT_EQUALS("", string);

   string = "abcdABCD";
   string.toUpperCase();
   TST_ASSERT_EQUALS("ABCDABCD", string);
   string.toLowerCase();
   TST_ASSERT_EQUALS("abcdabcd", string);

   string = "1234567890!@#$^&*\n\t ";
   string.toUpperCase();
   TST_ASSERT_EQUALS("1234567890!@#$^&*\n\t ", string);
   string.toLowerCase();
   TST_ASSERT_EQUALS("1234567890!@#$^&*\n\t ", string);

   string = "1234 abcd ABCD \n !@#$";
   string.toUpperCase();
   TST_ASSERT_EQUALS("1234 ABCD ABCD \n !@#$", string);
   string.toLowerCase();
   TST_ASSERT_EQUALS("1234 abcd abcd \n !@#$", string);
}

void testCOLstringCstr(){
   COLstring string;
   const char* cstring;

   string = "It was a dark and stormy night.";
   cstring = string.c_str();
   TST_ASSERT_EQUALS(0, strcmp(cstring, "It was a dark and stormy night."));

   string = "0123456789";
   cstring = string.c_str();
   TST_ASSERT_EQUALS(0, strcmp(cstring, "0123456789"));

   string = "!@#$^&*";
   cstring = string.c_str();
   TST_ASSERT_EQUALS(0, strcmp(cstring, "!@#$^&*"));

   string = "\n\thello\t\n ";
   cstring = string.c_str();
   TST_ASSERT_EQUALS(0, strcmp(cstring, "\n\thello\t\n "));
}

void testCOLstringEquality(){
   TST_ASSERT(COLstring("Fred") == COLstring("Fred"));
   TST_ASSERT("Barney" == COLstring("Barney"));
   TST_ASSERT(COLstring("Barney") == "Barney");

   TST_ASSERT(!(COLstring("Fred") == COLstring("From")));
   TST_ASSERT(!("Barney" == COLstring("Barn")));
   TST_ASSERT(!(COLstring("Bar") == "Barney"));

   const char* pChar = "abcdefghijklmnop";
   COLstring Test(pChar);
   COLstring Control(pChar);

   TST_ASSERT_EQUALS(pChar, Test);
   TST_ASSERT_EQUALS(Control, Test);

   Test = "";
   TST_ASSERT_EQUALS("", Test);
   Test = "A";
   TST_ASSERT_EQUALS("A", Test);
}

void testCOLstringInequality(){
   TST_ASSERT(COLstring("Fred") != COLstring("Barney"));
   TST_ASSERT("Fred" != COLstring("Barney"));
   TST_ASSERT(COLstring("Fred") != "Barney");

   TST_ASSERT(COLstring("Fred") > COLstring("Barney"));
   TST_ASSERT("Fred" > COLstring("Barney"));
   TST_ASSERT(COLstring("Fred") > "Barney");

   TST_ASSERT(COLstring("Fried") >= COLstring("Fred"));
   TST_ASSERT("Fried" >= COLstring("Fred"));
   TST_ASSERT(COLstring("Fried") >= "Fred");

   TST_ASSERT(COLstring("Betty") < COLstring("Wilma"));
   TST_ASSERT("Betty" < COLstring("Wilma"));
   TST_ASSERT(COLstring("Betty") < "Wilma");

   TST_ASSERT(!(COLstring("Better") < COLstring("Better")));
   TST_ASSERT(!("Better" < COLstring("Better")));
   TST_ASSERT(!(COLstring("Better") < "Better"));

   TST_ASSERT(!(COLstring("Better") > COLstring("Better")));
   TST_ASSERT(!("Better" > COLstring("Better")));
   TST_ASSERT(!(COLstring("Better") > "Better"));

   TST_ASSERT((COLstring("Better") <= COLstring("Better")));
   TST_ASSERT(("Better" <= COLstring("Better")));
   TST_ASSERT((COLstring("Better") <= "Better"));

   TST_ASSERT(COLstring("Better") <= COLstring("Betty"));
   TST_ASSERT("Better" <= COLstring("Betty"));
   TST_ASSERT(COLstring("Better") <= "Betty");

   TST_ASSERT(!(COLstring("Fred") != COLstring("Fred")));
   TST_ASSERT(!("Fred" != COLstring("Fred")));
   TST_ASSERT(!(COLstring("Fred") != "Fred"));

   TST_ASSERT(!(COLstring("Fred") > COLstring("Fred")));
   TST_ASSERT(!("Fred" > COLstring("Fred")));
   TST_ASSERT(!(COLstring("Fred") > "Fred"));

   TST_ASSERT((COLstring("Frame") >= COLstring("Frame")));
   TST_ASSERT(("Frame" >= COLstring("Frame")));
   TST_ASSERT((COLstring("Frame") >= "Frame"));

   TST_ASSERT(!(COLstring("Betty") < COLstring("Betty")));
   TST_ASSERT(!("Betty" < COLstring("Betty")));
   TST_ASSERT(!(COLstring("Betty") < "Betty"));

   TST_ASSERT(!(COLstring("Bother") <= COLstring("Betty")));
   TST_ASSERT(!("Bother" <= COLstring("Betty")));
   TST_ASSERT(!(COLstring("Bother") <= "Betty"));
}

void testCOLstringNilCharacterPlusEquals(){
   // a lot of code depends on a NIL character append increasing the string length
   TST_ASSERT_EQUALS(1, (COLstring() += '\0').size());
   TST_ASSERT_EQUALS(3, (((COLstring() += 'a') += '\0') += 'c').size());
}

void testCOLstringMisc(){
   COLstring String1;

   String1 = "0123456789";
   TST_ASSERT_MESSAGE(String1.size() == 10,"COLstring::size failed");

   String1.clear();
   TST_ASSERT_MESSAGE((String1.size() == 0) && (String1.is_null()),"COLstring::size failed");

   String1 = "build/COL/testCOLstring.cpp";
   COLstring Out1;
   COLstring Out2;

   //normal split
   String1.split(Out1,Out2,"/");
   TST_ASSERT_MESSAGE(Out1 == "build","COLstring::split failed" << Out1);
   TST_ASSERT_MESSAGE(Out2 == "COL/testCOLstring.cpp","COLstring::split failed: " << Out2);

   bool rsplitReturnCode = false;
   rsplitReturnCode = String1.rsplit(Out1,Out2,"/");;
   TST_ASSERT_EQUALS(true, rsplitReturnCode);
   TST_ASSERT_MESSAGE(Out1 == "build/COL","COLstring::rsplit failed");
   TST_ASSERT_MESSAGE(Out2 == "testCOLstring.cpp","COLstring::rsplit failed");

   String1.splitAtIndex(Out1, Out2, 5);
   TST_ASSERT_MESSAGE(Out1 == "build", "COLstring:splitAtIndex failed" << Out1);
   TST_ASSERT_MESSAGE(Out2 == "/COL/testCOLstring.cpp", "COLstring:splitAtIndex failed" << Out2);

   //split with no token
   String1.split(Out1,Out2," ");
   TST_ASSERT_MESSAGE(Out1 == String1,"COLstring::split failed" << Out1);
   TST_ASSERT_MESSAGE(Out2 == "","COLstring::split failed: " << Out2);

   rsplitReturnCode = String1.rsplit(Out1,Out2," ");;
   TST_ASSERT_EQUALS(false, rsplitReturnCode);
   TST_ASSERT_MESSAGE(Out1 == "","COLstring::rsplit failed");
   TST_ASSERT_MESSAGE(Out2 == String1,"COLstring::rsplit failed");

   String1.splitAtIndex(Out1, Out2, COL_UINT_32_MAX);
   TST_ASSERT_MESSAGE(Out1 == String1, "COLstring:splitAtIndex failed" << Out1);
   TST_ASSERT_MESSAGE(Out2 == "", "COLstring:splitAtIndex failed" << Out2);

   String1 = "aaaFFFaFaaFFFaaaa";
   TST_ASSERT_MESSAGE(String1.strip(COLstring::Leading,'a') == "FFFaFaaFFFaaaa",
                      "COLstring::strip(Leading) failed" << newline);
   TST_ASSERT_MESSAGE(String1.strip(COLstring::Trailing,'a') == "aaaFFFaFaaFFF",
                      "COLstring::strip(Trailing) failed" << newline);
   TST_ASSERT_MESSAGE(String1.strip(COLstring::Both,'a') == "FFFaFaaFFF",
                      "COLstring::strip(Both) failed" << newline);

   // Some more split tests.

   COLstring StringA;
   COLstring OutA;
   COLstring OutB;

   StringA = "/home/michael/build/backup_crm/backup/imsdbbackup2/";
   rsplitReturnCode = StringA.rsplit(OutA, OutB, "/");
   TST_ASSERT_EQUALS(true, rsplitReturnCode);

   TST_ASSERT_MESSAGE(OutA == "/home/michael/build/backup_crm/backup/imsdbbackup2", "COLstring::rsplit failed on output a.");
   TST_ASSERT_MESSAGE(OutB == "", "COLstring::rsplit failed on output b.");

   StringA = "/home/michael/build/backup_crm/backup/imsdbbackup2/";
   StringA.split(OutA, OutB, "/");

   TST_ASSERT_MESSAGE(OutB == "home/michael/build/backup_crm/backup/imsdbbackup2/", "COLstring::rsplit failed on output a.");
   TST_ASSERT_MESSAGE(OutA == "", "COLstring::rsplit failed on output b.");

   /**
   #############################################################################
   COLsplit Unit-Tests
   #############################################################################
   */
   COLarray<COLstring> PartList;

   PartList.clear();
   COLsplit(&PartList, "FOO|BAR|ZOO", "|");
   TST_ASSERT_EQUALS(3, PartList.size());
   TST_ASSERT_EQUALS(COLstring("FOO"), PartList[0]);
   TST_ASSERT_EQUALS(COLstring("BAR"), PartList[1]);
   TST_ASSERT_EQUALS(COLstring("ZOO"), PartList[2]);

   // testing empty strings (immediate separators) at start, end -
   // they should still be parsed as empty string at respective positions
   PartList.clear();
   COLsplit(&PartList, "|BAR|", "|");
   TST_ASSERT_EQUALS(3, PartList.size());
   TST_ASSERT_EQUALS(COLstring(""), PartList[0]);
   TST_ASSERT_EQUALS(COLstring("BAR"), PartList[1]);
   TST_ASSERT_EQUALS(COLstring(""), PartList[2]);

   // just an empty string - produces zero parts (boundary case)
   PartList.clear();
   COLsplit(&PartList, "", "|");
   TST_ASSERT_EQUALS(0, PartList.size());

   // just a non-empty string - produces one part (normal case - #parts = #separators + 1)
   PartList.clear();
   COLsplit(&PartList, "FOO", "|");
   TST_ASSERT_EQUALS(1, PartList.size());
   TST_ASSERT_EQUALS(COLstring("FOO"), PartList[0]);
   COLvar List;
   COLsplit(&List, "", ",");
   TST_ASSERT(List.isArray());
}

void testCOLstringFind(){
   /* ##################### COLstring::find Unit-Tests ###################### */

   COLstring string, pattern;
   char character;
   size_t position, result;
   int Pos = 0;

   // size_t find(const char Character, size_t Position = 0) const

   string = "";
   character = '\0';
   result = string.find(character);
   TST_ASSERT_EQUALS(npos, result);
   TST_ASSERT(string.find(&Pos, character) == false);

   string = "";
   character = 'x';
   result = string.find(character);
   TST_ASSERT_EQUALS(npos, result);
   TST_ASSERT(string.find(&Pos, character) == false);

   string = "It was a dark and stormy night.";
   character = '\t';
   result = string.find(character);
   TST_ASSERT_EQUALS(npos, result);
   TST_ASSERT(string.find(&Pos, character) == false);

   // FogBugz Case 22661
   string = "It was a dark and stormy night.";
   character = 'a';
   position = 50000000;
   result = string.find(character, position);
   TST_ASSERT_EQUALS(npos, result);
   TST_ASSERT(string.find(&Pos, character, position) == false);

   string = "It \t was a \t dark and stormy \t night. \n";
   character = '\t';
   result = string.find(character);
   TST_ASSERT_EQUALS(3, result);
   TST_ASSERT(string.find(&Pos, character) == true && 3 == Pos);

   string = "Username";
   character = 'u';
   result = string.find(character);
   TST_ASSERT_EQUALS(npos, result);
   TST_ASSERT(string.find(&Pos, character) == false);

   // size_t find(const COLstring& Pattern, size_t Position = 0) const

   string = "";
   pattern = "";
   result = string.find(pattern);
   TST_ASSERT_EQUALS(npos, result);
   TST_ASSERT_EQUALS(npos, COLstring("").find(""));
   TST_ASSERT(string.find(&Pos, pattern) == false);

   string = "";
   pattern = "hello";
   result = string.find(pattern);
   TST_ASSERT_EQUALS(npos, result);
   TST_ASSERT_EQUALS(npos, COLstring("").find("hello"));
   TST_ASSERT(string.find(&Pos, pattern) == false);

   string = "It was a dark and stormy night.";
   pattern = "was a";
   result = string.find(pattern);
   TST_ASSERT_EQUALS(3, result);
   TST_ASSERT_EQUALS(3, COLstring("It was a dark and stormy night.").find("was a"));
   TST_ASSERT(string.find(&Pos, pattern) == true && 3 == Pos);

   string = "It was a dark and stormy night.";
   pattern = "was a";
   position = 4;
   result = string.find(pattern, position);
   TST_ASSERT_EQUALS(npos, result);
   TST_ASSERT_EQUALS(npos, COLstring("It was a dark and stormy night.").find("was a",4));
   TST_ASSERT(string.find(&Pos, pattern, position) == false);

   string = "It \t \nwas a dark\t\n";
   pattern = "\t\n";
   result = string.find(pattern);
   TST_ASSERT_EQUALS(16, result);
   TST_ASSERT_EQUALS(16, COLstring("It \t \nwas a dark\t\n").find("\t\n"));
   TST_ASSERT(string.find(&Pos, pattern) == true && 16 == Pos);

   string = "It was a dark and STORMY night.";
   pattern = "StORMY";
   result = string.find(pattern);
   TST_ASSERT_EQUALS(npos, result);
   TST_ASSERT_EQUALS(18, COLstring("It was a dark and STORMY night.").find("STORMY"));
   TST_ASSERT_EQUALS(npos, COLstring("It was a dark and STORMY night.").find("StORMY"));
   TST_ASSERT(string.find(&Pos, "StORMY") == false);
   TST_ASSERT(string.find(&Pos, "STORMY") == true && 18 == Pos);

   // size_t rfind(const COLstring& Pattern) const

   TST_ASSERT_EQUALS(  13, COLstring("012345678901234567890").rfind("3456",npos));
   TST_ASSERT_EQUALS(npos, COLstring("012345678901234567890").rfind("3456",0));
   TST_ASSERT_EQUALS(npos, COLstring("012345678901234567890").rfind("3456",1));
   TST_ASSERT_EQUALS(npos, COLstring("012345678901234567890").rfind("3456",2));
   TST_ASSERT_EQUALS(   3, COLstring("012345678901234567890").rfind("3456",3));
   TST_ASSERT_EQUALS(   3, COLstring("012345678901234567890").rfind("3456",4));
   TST_ASSERT_EQUALS(   3, COLstring("012345678901234567890").rfind("3456",5));
   TST_ASSERT_EQUALS(   3, COLstring("012345678901234567890").rfind("3456",6));
   TST_ASSERT_EQUALS(   3, COLstring("012345678901234567890").rfind("3456",7));
   TST_ASSERT_EQUALS(   3, COLstring("012345678901234567890").rfind("3456",8));
   TST_ASSERT_EQUALS(   3, COLstring("012345678901234567890").rfind("3456",9));
   TST_ASSERT_EQUALS(   3, COLstring("012345678901234567890").rfind("3456",10));
   TST_ASSERT_EQUALS(   3, COLstring("012345678901234567890").rfind("3456",11));
   TST_ASSERT_EQUALS(   3, COLstring("012345678901234567890").rfind("3456",12));
   TST_ASSERT_EQUALS(  13, COLstring("012345678901234567890").rfind("3456",13));
   TST_ASSERT_EQUALS(  13, COLstring("012345678901234567890").rfind("3456",14));
   TST_ASSERT_EQUALS(  13, COLstring("012345678901234567890").rfind("3456",15));
   TST_ASSERT_EQUALS(  13, COLstring("012345678901234567890").rfind("3456",16));
   TST_ASSERT_EQUALS(  13, COLstring("012345678901234567890").rfind("3456",17));
   TST_ASSERT_EQUALS(  13, COLstring("012345678901234567890").rfind("3456",18));
   TST_ASSERT_EQUALS(  13, COLstring("012345678901234567890").rfind("3456",19));
   TST_ASSERT_EQUALS(  13, COLstring("012345678901234567890").rfind("3456",20));
   TST_ASSERT_EQUALS(  13, COLstring("012345678901234567890").rfind("3456",21));
   TST_ASSERT_EQUALS(  13, COLstring("012345678901234567890").rfind("3456",22));
   TST_ASSERT_EQUALS(  13, COLstring("012345678901234567890").rfind("3456",23));
   TST_ASSERT_EQUALS(  13, COLstring("012345678901234567890").rfind("3456",24));
   TST_ASSERT_EQUALS(  13, COLstring("012345678901234567890").rfind("3456",25));
   TST_ASSERT_EQUALS(  13, COLstring("012345678901234567890").rfind("3456",99999999));

   string = "";
   pattern = "";
   result = string.rfind(pattern);
   TST_ASSERT_EQUALS(npos, result);
   TST_ASSERT(string.rfind(&Pos, pattern) == false);

   string = "";
   pattern = "hello";
   result = string.rfind(pattern);
   TST_ASSERT_EQUALS(npos, result);
   TST_ASSERT(string.rfind(&Pos, pattern) == false);

   // FogBugz Case 22619
   string = "It was a dark and stormy night.";
   pattern = "was";
   position = 10;
   result = string.rfind(pattern, position);
   TST_ASSERT_EQUALS(3, result);
   TST_ASSERT(string.rfind(&Pos, pattern, position) == true && 3 == Pos);

   string = "It was a dark and dark night.";
   pattern = "dark";
   result = string.rfind(pattern);
   TST_ASSERT_EQUALS(18, result);
   TST_ASSERT(string.rfind(&Pos, pattern) == true && 18 == Pos);

   string = "It\t\nwas a dark \t \n ";
   pattern = "\t\n";
   result = string.rfind(pattern);
   TST_ASSERT_EQUALS(2, result);
   TST_ASSERT(string.rfind(&Pos, pattern) == true && 2 == Pos);

   string = "It was a dark and STORMY night.";
   pattern = "StORMY";
   result = string.rfind(pattern);
   TST_ASSERT_EQUALS(npos, result);
   TST_ASSERT(string.rfind(&Pos, pattern) == false);

   string = "It was a dark and stormy night.dark";
   pattern = "dark";
   position = 20;
   result = string.rfind(pattern, position);
   TST_ASSERT_EQUALS(9, result);
   TST_ASSERT(string.rfind(&Pos, pattern, position) == true && 9 == Pos);

   /* ################# COLstring::find Integration Tests ################### */

   COLstring String1 = basicString;
   String1 += " Suddenly a shot rang out.";
   size_t CountOfFound;
   size_t Position;

   CountOfFound = 0;
   Position = 0;
   // search forwards
   while (true) {
      Position = String1.find("an",Position);
      if (Position == npos) {
         break;
      }
      switch (CountOfFound) {
         case 0: {
            TST_ASSERT_MESSAGE(Position == 14,"COLstring::find failed");
            break;
         }
         case 1: {
            TST_ASSERT_MESSAGE(Position == 49,"COLstring::find failed");
            break;
         }
      }
      COL_TRC(Position << ' ' << CountOfFound << newline);

      Position++;
      CountOfFound++;
   }

   CountOfFound = 0;
   Position = String1.size();
   // search backwards
   while (true)
   {
      Position = String1.rfind("an",Position);

      if (Position == npos){
         break;
      }
      switch (CountOfFound){
         case 0:{
            TST_ASSERT_MESSAGE(Position == 49,"COLstring::rfind failed");
            break;
         }
         case 1:{
            TST_ASSERT_MESSAGE(Position == 14,"COLstring::rfind failed");
            break;
         }
      }
      COL_TRC(Position << ' ' << CountOfFound << newline);

      Position--;

      if (Position == npos){
         break;
      }
      CountOfFound++;
   }
   // #########################################################################
}

void testCOLstringCompare(){
   COLstring string1, string2;
   const char* cstring;
   int result;

   string1 = "";
   string2 = "";
   result = string1.compare(string2);
   TST_ASSERT_EQUALS(0, result);
   result = string2.compare(string1);
   TST_ASSERT_EQUALS(0, result);

   string1 = "It was a dark and stormy night.";
   string2 = "It was a dark and Stormy night.";
   result = string1.compare(string2);
   TST_ASSERT(result > 0);
   result = string2.compare(string1);
   TST_ASSERT(result < 0);

   string1 = "\t\n ";
   string2 = "\t ";
   result = string1.compare(string2);
   TST_ASSERT(result < 0);
   result = string2.compare(string1);
   TST_ASSERT(result > 0);

   string1 = "";
   cstring = "";
   result = string1.compare(cstring);
   TST_ASSERT_EQUALS(0, result);

   string1 = "It was a dark and stormy night.";
   cstring = "It was a dark and Stormy night.";
   result = string1.compare(cstring);
   TST_ASSERT(result > 0);

   string1 = "\t\n ";
   cstring = "\t ";
   result = string1.compare(cstring);
   TST_ASSERT(result < 0);

   string1 = "hello";
   cstring = "hello\0";
   result = string1.compare(cstring);
   TST_ASSERT_EQUALS(0, result);

   string1 = "hello\0";
   cstring = "hello";
   result = string1.compare(cstring);
   TST_ASSERT_EQUALS(0, result);
}

void testCOLstringEdits(){
   COLstring someString;
   COLostream someStream(someString);
   someStream << "Hello, lets assume this is some test email.with a bunch of CRLFs.\n\r"
              << "\r\r\n\r.  Here is a naked line feed \n..............";


   someString.replace("\n", "\r\n");
   someString.replace("\r\r\n", "\r\n");

   TST_ASSERT_MESSAGE(someString == "Hello, lets assume this is some test email.with a bunch of CRLFs.\r\n\r\r\r\n\r.  Here is a naked line feed \r\n..............", "COLstring::replace failed on naked line feeds.");

   COLstring String1 = "rain\\Rsnow\\Rtinder\\Rferment\\Rnear\\Rchime";
   int CountOfReplace = String1.replace("\\r","~");

   if ((CountOfReplace != 5) != (String1 != "rain~snow~tinder~ferment~near~chime")){
      COL_TRC("COLstring::replace failed" << newline);
   }

   COL_TRC("TESTING" << newline);

   String1 = "thetheAnd the wolves ththowtled at the.. the full moon.thethe";

   TST_ASSERT_MESSAGE(String1.replace("the", "da") == 7, "COLstring::replace2 failed count.");
   TST_ASSERT_MESSAGE(String1 == "dadaAnd da wolves ththowtled at da.. da full moon.dada", "COLstring::replace2 failed content.");

   String1 = "fd78HK(*^$";
   String1.toUpperCase();

   TST_ASSERT_MESSAGE(String1 == "FD78HK(*^$","COLstring::toUpperCase failed");

   String1.toLowerCase();

   TST_ASSERT_MESSAGE(String1 == "fd78hk(*^$","COLstring::toLowerCase failed");

   String1 = basicString;

   while (String1.size() < 100000){
      String1 += String1; // exercise append
   }

   String1 = basicString;

   while (String1.size() < 100000){
      String1.prepend(String1); // exercise prepend
   }

   String1.assign(3,'A');
   TST_ASSERT_MESSAGE(String1 == "AAA","COLstring.assign(3,'A') failed");

   String1.assign(33,'R');
   TST_ASSERT_MESSAGE(String1 == "RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR","COLstring.assign(33,'R') failed");

   //This little code snippet was in REG.
   COLstring String;
   String.assign(10,'\0'); // in REG this was String.assign('\0',10), which is damn dangerous;
   snprintf(String.get_buffer(), String.capacity(), "%8lx", (long)12345678);
   TST_ASSERT_EQUALS("  bc614e", String);

   String1 = basicString;
   String1.stripAll();

   TST_ASSERT_MESSAGE(String1 == "Itwasadarkandstormynight.","COLstring::stripAll failed");

   String1 = basicString;

   TST_ASSERT_MESSAGE(String1.substr(9,4) == "dark","COLstring::substr failed");

   String1.remove(13,11);
   TST_ASSERT_MESSAGE(String1 == "It was a dark night.","COLstring::remove failed");

   String1.replace(9,4,"bright");
   TST_ASSERT_EQUALS("It was a bright night.", String1);
   TST_ASSERT_MESSAGE(String1 == "It was a bright night.","COLstring::replace failed");

   String1.append(" Crickets chirped.", 18);

   TST_ASSERT_MESSAGE(String1 == "It was a bright night. Crickets chirped.","COLstring::append failed");

   String1.prepend("Jack gazed out through the window. ");
   TST_ASSERT_EQUALS("Jack gazed out through the window. It was a bright night. Crickets chirped.", String1);
   TST_ASSERT_MESSAGE(String1 == "Jack gazed out through the window. It was a bright night. Crickets chirped.","COLstring::prepend failed");

   String1.insert(58,"Four seasons in one day. ");
   TST_ASSERT_EQUALS("Jack gazed out through the window. It was a bright night. Four seasons in one day. Crickets chirped.", String1);
   TST_ASSERT_MESSAGE(String1 == "Jack gazed out through the window. It was a bright night. Four seasons in one day. Crickets chirped.","COLstring::insert failed");
}

void testCOLstringSink(){
   COLstring Sink;
   COLostream Output(Sink);

   Sink.write(0,0); //this should be a safe call

   Output << 'A';
   TST_ASSERT_EQUALS("A", Sink);
   Sink.clear();

   Output << basicString;
   TST_ASSERT_EQUALS(basicString, Sink);
   Sink.clear();

   COLstring String1(basicString);
   Output << String1;
   TST_ASSERT_EQUALS(basicString, Sink);
   Sink.clear();

   COLstring Empty;
   Output << Empty;
   TST_ASSERT_EQUALS("", Sink);
   Sink.clear();

   Output << 1234567890;
   TST_ASSERT_EQUALS("1234567890", Sink);
   Sink.clear();

   Output << 3.141592653589793238;
   TST_ASSERT_EQUALS("3.141593", Sink);
   Sink.clear();
}

void testCOLstringAssign(){
   TST_ASSERT_EQUALS(0, COLstring("short string").assign(-2,'X').size());
   TST_ASSERT_EQUALS(0, COLstring("short string").assign(-1,'X').size());
   TST_ASSERT_EQUALS(0, COLstring("short string").assign(0,'X').size());
   TST_ASSERT_EQUALS(1, COLstring("short string").assign(1,'X').size());
   TST_ASSERT_EQUALS(2, COLstring("short string").assign(2,'X').size());
   TST_ASSERT_EQUALS(3, COLstring("short string").assign(3,'X').size());

   TST_ASSERT_EQUALS("",    COLstring("A much longer string to test capacity.").assign(-2,'J'));
   TST_ASSERT_EQUALS("",    COLstring("A much longer string to test capacity.").assign(-1,'J'));
   TST_ASSERT_EQUALS("",    COLstring("A much longer string to test capacity.").assign(0,'X'));
   TST_ASSERT_EQUALS("X",   COLstring("A much longer string to test capacity.").assign(1,'X'));
   TST_ASSERT_EQUALS("XX",  COLstring("A much longer string to test capacity.").assign(2,'X'));
   TST_ASSERT_EQUALS("XXX", COLstring("A much longer string to test capacity.").assign(3,'X'));

   for (int i = 0; i < 67; ++i) {
      // verify COLstring size() after assign()
      TST_ASSERT_EQUALS(i, COLstring("dummy").assign(i,'@').size());

      // verify COLstring string contents after assign()
      char buff[100];
      memset(buff, '?', sizeof(buff));
      memset(buff, 'Z', i);
      buff[i] = 0;
      TST_ASSERT_EQUALS(0, ::memcmp(COLstring().assign(i,'Z').data(), buff, i+1));
   }

   COLstring string;
   size_t n = 0;
   char filler;

   string = "";
   n = 0;
   filler = ' ';
   string.assign(n, filler);
   TST_ASSERT_EQUALS("", string);
   TST_ASSERT_EQUALS(0, string.size());

   string = "";
   n = 10;
   filler = '\0';
   string.assign(n, filler);
   TST_ASSERT_EQUALS("\0\0\0\0\0\0\0\0\0\0", string);
   TST_ASSERT_EQUALS(10, string.size());

   string = "It was a dark and stormy night.";
   n = 20;
   filler = '\t';
   string.assign(n, filler);
   TST_ASSERT_EQUALS("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t", string);
   TST_ASSERT_EQUALS(20, string.size());

   string = "C:\\Program Files\\iNTERFACEWARE\\";
   n = 5;
   filler = '\\';
   string.assign(n, filler);
   TST_ASSERT_EQUALS("\\\\\\\\\\", string);
   TST_ASSERT_EQUALS(5, string.size());
}

void testCOLstringSize(){
   COLstring String("goldfish");

   String.setSize(4);
   TST_ASSERT_EQUALS(4, String.size());
   TST_ASSERT_EQUALS(4, strlen(String.c_str()));
   TST_ASSERT_EQUALS("gold", String);
   TST_ASSERT_EQUALS(0, strcmp(String.c_str(), "gold"));

   String.setSize(0);
   TST_ASSERT_EQUALS(0, String.size());
   TST_ASSERT_EQUALS(0, strlen(String.c_str()));
   TST_ASSERT_EQUALS("", String);
   TST_ASSERT_EQUALS(0, strcmp(String.c_str(), ""));
}

void testCOLstringStripAll(){
   COLstring string;
   char c;

   string = "";
   string.stripAll();
   TST_ASSERT_EQUALS("", string);

   string = "It was a dark and stormy night.  ";
   string.stripAll();
   TST_ASSERT_EQUALS("Itwasadarkandstormynight.", string);

   string = "It\t\t\n was a dark.";
   string.stripAll('\t');
   TST_ASSERT_EQUALS("It\n was a dark.", string);

   string = "It was a dark and stormy night.";
   string.stripAll('A');
   TST_ASSERT_EQUALS("It was a dark and stormy night.", string);

   string = "It was a dark and stormy night.";
   string.stripAll('a');
   TST_ASSERT_EQUALS("It ws  drk nd stormy night.", string);
}

void testCOLstringRemove(){
   COLstring string;
   COLstring resultString;
   size_t startIndex, numberOfCharacters;

   string = "";
   startIndex = 0;
   numberOfCharacters = 0;
   resultString = string.remove(startIndex, numberOfCharacters);
   TST_ASSERT_EQUALS("", string);
   TST_ASSERT_EQUALS(string, resultString);

   string = "It was a dark and stormy night.";
   startIndex = 0;
   resultString = string.remove(startIndex);
   TST_ASSERT_EQUALS("", string);
   TST_ASSERT_EQUALS(string, resultString);

   string = "It was a dark and stormy night.";
   startIndex = 6;
   numberOfCharacters = 12;
   resultString = string.remove(startIndex, numberOfCharacters);
   TST_ASSERT_EQUALS("It wasstormy night.", string);
   TST_ASSERT_EQUALS(string, resultString);

   string = "\t\n\n ";
   startIndex = 1;
   numberOfCharacters = 2;
   resultString = string.remove(startIndex, numberOfCharacters);
   TST_ASSERT_EQUALS("\t ", string);
   TST_ASSERT_EQUALS(string, resultString);

   string = "It was a dark and stormy night.";
   startIndex = 50;
   resultString = string.remove(startIndex);
   TST_ASSERT_EQUALS("It was a dark and stormy night.", string);
   TST_ASSERT_EQUALS(string, resultString);

   string = "It was a dark and stormy night.";
   startIndex = 2;
   numberOfCharacters = 50;
   resultString = string.remove(startIndex, numberOfCharacters);
   TST_ASSERT_EQUALS("It", string);
   TST_ASSERT_EQUALS(string, resultString);

   string = "It was a dark and stormy night.";
   startIndex = 50;
   numberOfCharacters = 50;
   resultString = string.remove(startIndex, numberOfCharacters);
   TST_ASSERT_EQUALS("It was a dark and stormy night.", string);
   TST_ASSERT_EQUALS(string, resultString);
}

void testCOLstringReplace(){
   COLstring string, replacementString, result;
   size_t startIndex, numberOfCharacters;
   size_t replacementStartIndex, replacementNumberOfCharacters;

   string = "";
   replacementString = "";
   startIndex = 0;
   numberOfCharacters = 0;
   result = string.replace(startIndex, numberOfCharacters, replacementString);
   TST_ASSERT_EQUALS("", result);
   TST_ASSERT_EQUALS(string, result);

   string = "";
   replacementString = "It was a dark and stormy night.";
   startIndex = 0;
   numberOfCharacters = 0;
   result = string.replace(startIndex, numberOfCharacters, replacementString);
   TST_ASSERT_EQUALS("It was a dark and stormy night.", string);
   TST_ASSERT_EQUALS(string, result);

   string = "It was a dark and stormy night.";
   replacementString = "crappy";
   startIndex = 18;
   numberOfCharacters = 6;
   result = string.replace(startIndex, numberOfCharacters, replacementString);
   TST_ASSERT_EQUALS("It was a dark and crappy night.", string);
   TST_ASSERT_EQUALS(string, result);

   string = "It was a dark and stormy night.";
   replacementString = "damp";
   startIndex = 18;
   numberOfCharacters = 6;
   result = string.replace(startIndex, numberOfCharacters, replacementString);
   TST_ASSERT_EQUALS("It was a dark and damp night.", string);
   TST_ASSERT_EQUALS(string, result);

   string = "It was a dark and stormy night.";
   replacementString = "day";
   startIndex = 25;
   numberOfCharacters = 2;
   result = string.replace(startIndex, numberOfCharacters, replacementString);
   TST_ASSERT_EQUALS("It was a dark and stormy dayght.", string);
   TST_ASSERT_EQUALS(string, result);

   string = "It was a dark \nand stormy night.";
   replacementString = "\t\t\n";
   startIndex = 14;
   numberOfCharacters = 5;
   result = string.replace(startIndex, numberOfCharacters, replacementString);
   TST_ASSERT_EQUALS("It was a dark \t\t\nstormy night.", string);
   TST_ASSERT_EQUALS(string, result);

   string = "";
   replacementString = "";
   startIndex = 0;
   numberOfCharacters = 0;
   replacementStartIndex = 0;
   replacementNumberOfCharacters = 0;
   result = string.replace(startIndex, numberOfCharacters, replacementString,
      replacementStartIndex, replacementNumberOfCharacters);
   TST_ASSERT_EQUALS("", string);
   TST_ASSERT_EQUALS(string, result);

   string = "";
   replacementString = "It was a dark and stormy night.";
   startIndex = 0;
   numberOfCharacters = 0;
   replacementStartIndex = 0;
   replacementNumberOfCharacters = 0;
   result = string.replace(startIndex, numberOfCharacters, replacementString,
      replacementStartIndex, replacementNumberOfCharacters);
   TST_ASSERT_EQUALS("", string);
   TST_ASSERT_EQUALS(string, result);

   string = "";
   replacementString = "It was a dark and stormy night.";
   startIndex = 0;
   numberOfCharacters = 0;
   replacementStartIndex = 0;
   replacementNumberOfCharacters = 31;
   result = string.replace(startIndex, numberOfCharacters, replacementString,
      replacementStartIndex, replacementNumberOfCharacters);
   TST_ASSERT_EQUALS("It was a dark and stormy night.", string);
   TST_ASSERT_EQUALS(string, result);

   string = "It was a dark and stormy night.";
   replacementString = "It was a bright and sunny day.";
   startIndex = 9;
   numberOfCharacters = 22;
   replacementStartIndex = 9;
   result = string.replace(startIndex, numberOfCharacters, replacementString,
      replacementStartIndex);
   TST_ASSERT_EQUALS("It was a bright and sunny day.", string);
   TST_ASSERT_EQUALS(string, result);

   string = "\t \n\b";
   replacementString = "!@#$^&*\\";
   startIndex = 1;
   numberOfCharacters = 2;
   replacementStartIndex = 50;
   replacementNumberOfCharacters = 5;
   result = string.replace(startIndex, numberOfCharacters, replacementString,
      replacementStartIndex, replacementNumberOfCharacters);
   TST_ASSERT_EQUALS("\t\b", string);
   TST_ASSERT_EQUALS(string, result);

   string = "\t \n\b";
   replacementString = "!@#$^&*\\1234567890";
   startIndex = 1;
   numberOfCharacters = 2;
   replacementStartIndex = 2;
   replacementNumberOfCharacters = 9;
   result = string.replace(startIndex, numberOfCharacters, replacementString,
      replacementStartIndex, replacementNumberOfCharacters);
   TST_ASSERT_EQUALS("\t#$^&*\\123\b", string);
   TST_ASSERT_EQUALS(string, result);
}

void testCOLstringPrepend(){
   COLstring string, prependString, result;
   const char* prependStr;
   size_t orig, n;

   string = "";
   prependString = "";
   result = string.prepend(prependString);
   TST_ASSERT_EQUALS("", string);
   TST_ASSERT_EQUALS(string, result);

   string = "";
   prependString = "It was a dark and stormy night.";
   result = string.prepend(prependString);
   TST_ASSERT_EQUALS("It was a dark and stormy night.", string);
   TST_ASSERT_EQUALS(string, result);

   string = "It was a dark and stormy night.";
   prependString = "Example: \n";
   result = string.prepend(prependString);
   TST_ASSERT_EQUALS("Example: \nIt was a dark and stormy night.", string);
   TST_ASSERT_EQUALS(string, result);

   string = "It was a dark and stormy night.";
   prependString = "\0";
   result = string.prepend(prependString);
   TST_ASSERT_EQUALS("It was a dark and stormy night.", string);
   TST_ASSERT_EQUALS(string, result);

   string = "";
   prependStr = "";
   result = string.prepend(prependStr);
   TST_ASSERT_EQUALS("", string);
   TST_ASSERT_EQUALS(string, result);

   string = "";
   prependStr = "It was a dark and stormy night.";
   result = string.prepend(prependStr);
   TST_ASSERT_EQUALS("It was a dark and stormy night.", string);
   TST_ASSERT_EQUALS(string, result);

   string = "It was a dark and stormy night.";
   prependStr = "Example: \n";
   result = string.prepend(prependStr);
   TST_ASSERT_EQUALS("Example: \nIt was a dark and stormy night.", string);
   TST_ASSERT_EQUALS(string, result);

   string = "It was a dark and stormy night.";
   prependStr = "\0";
   result = string.prepend(prependStr);
   TST_ASSERT_EQUALS("It was a dark and stormy night.", string);
   TST_ASSERT_EQUALS(string, result);
}

void testCOLstringInsert(){
   TST_ASSERT_EQUALS("",         COLstring("").insert(-2,"BCD"));
   TST_ASSERT_EQUALS("",         COLstring("").insert(-2,COLstring("BCD")));

   TST_ASSERT_EQUALS("",         COLstring("").insert(-1,"BCD"));
   TST_ASSERT_EQUALS("",         COLstring("").insert(-1,COLstring("BCD")));

   TST_ASSERT_EQUALS("BCDEF",    COLstring("").insert(0,"BCDEF"));
   TST_ASSERT_EQUALS("BCD",      COLstring("").insert(0,COLstring("BCD")));

   TST_ASSERT_EQUALS("xyz",      COLstring("xyz").insert(-2,"BCD"));
   TST_ASSERT_EQUALS("xyz",      COLstring("xyz").insert(-2,COLstring("AAA")));

   TST_ASSERT_EQUALS("xyz",      COLstring("xyz").insert(-1,"BCD"));
   TST_ASSERT_EQUALS("xyz",      COLstring("xyz").insert(-1,COLstring("ZZZ")));

   TST_ASSERT_EQUALS("BCDxyz",   COLstring("xyz").insert(0,"BCD"));
   TST_ASSERT_EQUALS("xyz",      COLstring("xyz").insert(0,COLstring("")));

   TST_ASSERT_EQUALS("xBCDyz",   COLstring("xyz").insert(1,"BCD"));
   TST_ASSERT_EQUALS("xABCDyz",  COLstring("xyz").insert(1,COLstring("ABCD")));
   TST_ASSERT_EQUALS("xyz",      COLstring("xyz").insert(1,COLstring("")));

   TST_ASSERT_EQUALS("xyBCDz",   COLstring("xyz").insert(2,"BCD"));
   TST_ASSERT_EQUALS("xyBCDz",   COLstring("xyz").insert(2,COLstring("BCD")));

   TST_ASSERT_EQUALS("xyzBCD",   COLstring("xyz").insert(3,"BCD"));
   TST_ASSERT_EQUALS("xyzABCD",   COLstring("xyz").insert(3,COLstring("ABCD")));

   TST_ASSERT_EQUALS("xyz",      COLstring("xyz").insert(4,"ABCDE"));
   TST_ASSERT_EQUALS("xyz",      COLstring("xyz").insert(4,COLstring("ABCDE")));

   TST_ASSERT_EQUALS("xyz",      COLstring("xyz").insert(5,"ABCDE"));
   TST_ASSERT_EQUALS("xyz",      COLstring("xyz").insert(5,COLstring("ABCDE")));

   COLstring string1, string2, result;
   const char* cstring;
   size_t position, orig, n;

   string1 = "";
   string2 = "";
   position = 0;
   result = string1.insert(position, string2);
   TST_ASSERT_EQUALS("", string1);
   TST_ASSERT_EQUALS(string1, result);

   // FogBugz Case 22645
   string1 = "";
   string2 = "It was a dark and stormy night.";
   position = 0;
   result = string1.insert(position, string2);
   TST_ASSERT_EQUALS("It was a dark and stormy night.", string1);
   TST_ASSERT_EQUALS(string1, result);

   string1 = "It was a dark";
   string2 = "and stormy night.";
   position = 12;
   result = string1.insert(position, string2);
   TST_ASSERT_EQUALS("It was a darand stormy night.k", string1);
   TST_ASSERT_EQUALS(string1, result);

   string1 = "It was a dark";
   string2 = "and stormy night.";
   position = 13;
   result = string1.insert(position, string2);
   TST_ASSERT_EQUALS("It was a darkand stormy night.", string1);
   TST_ASSERT_EQUALS(string1, result);

   // FogBugz Case 22648
   string1 = "It was a dark";
   string2 = "and stormy night.";
   position = 50;
   // 50 > strlen("It was a dark"), so no change expected to string1
   result = string1.insert(position, string2);
   TST_ASSERT_EQUALS("It was a dark", string1);
   TST_ASSERT_EQUALS(string1, result);

   string1 = "It was a dark";
   string2 = "and stormy night";
   position = 13;
   result = string1.insert(position, string2);
   TST_ASSERT_EQUALS("It was a darkand stormy night", string1);
   TST_ASSERT_EQUALS(string1, result);

   string1 = "\b\t\nHello\n ";
   string2 = "\\:\t";
   position = 3;
   result = string1.insert(position, string2);
   TST_ASSERT_EQUALS("\b\t\n\\:\tHello\n ", string1);
   TST_ASSERT_EQUALS(string1, result);

   string1 = "\b\t\nHello\n ";
   string2 = "\\:\t";
   position = 0;
   result = string1.insert(position, string2);
   TST_ASSERT_EQUALS("\\:\t\b\t\nHello\n ", string1);
   TST_ASSERT_EQUALS(string1, result);

   string1 = "It was a dark";
   string2 = "\0";
   position = 6;
   result = string1.insert(position, string2);
   TST_ASSERT_EQUALS("It was a dark", string1);
   TST_ASSERT_EQUALS(string1, result);

   string1 = "";
   cstring = "";
   position = 0;
   result = string1.insert(position, cstring);
   TST_ASSERT_EQUALS("", string1);
   TST_ASSERT_EQUALS(string1, result);

   // FogBugz Case 22645
   string1 = "";
   cstring = "It was a dark and stormy night.";
   position = 0;
   result = string1.insert(position, cstring);
   TST_ASSERT_EQUALS("It was a dark and stormy night.", string1);
   TST_ASSERT_EQUALS(string1, result);

   string1 = "It was a dark";
   cstring = "and stormy night.";
   position = 12;
   result = string1.insert(position, cstring);
   TST_ASSERT_EQUALS("It was a darand stormy night.k", string1);
   TST_ASSERT_EQUALS(string1, result);

   string1 = "It was a dark";
   cstring = "and stormy night.";
   position = 13;
   result = string1.insert(position, cstring);
   TST_ASSERT_EQUALS("It was a darkand stormy night.", string1);
   TST_ASSERT_EQUALS(string1, result);

   // FogBugz Case 22648
   string1 = "It was a dark";
   cstring = "and stormy night.";
   position = 50;
   result = string1.insert(position, cstring);
   // 50 > strlen("it was a dark") so no change expected to string1
   TST_ASSERT_EQUALS("It was a dark", string1);
   TST_ASSERT_EQUALS(string1, result);

   string1 = "It was a very extremely dark";
   cstring = "and stormy night";
   result = string1.insert(string1.size(), cstring);
   TST_ASSERT_EQUALS("It was a very extremely darkand stormy night", string1);
   TST_ASSERT_EQUALS(string1, result);

   string1 = "\b\t\nHello\n ";
   cstring = "\\:\t";
   position = 3;
   result = string1.insert(position, cstring);
   TST_ASSERT_EQUALS("\b\t\n\\:\tHello\n ", string1);
   TST_ASSERT_EQUALS(string1, result);

   string1 = "\b\t\nHello\n ";
   cstring = "\\:\t";
   position = 0;
   result = string1.insert(position, cstring);
   TST_ASSERT_EQUALS("\\:\t\b\t\nHello\n ", string1);
   TST_ASSERT_EQUALS(string1, result);

   string1 = "It was a dark";
   cstring = "\0";
   position = 6;
   result = string1.insert(position, cstring);
   TST_ASSERT_EQUALS("It was a dark", string1);
   TST_ASSERT_EQUALS(string1, result);
}

void testCOLstringReplaceChar(){
   COLstring string;
   char target, replacement;

   string = "";
   target = '\0';
   replacement = '\0';
   string.replace(target, replacement);
   TST_ASSERT_EQUALS("", string);

   string = "";
   target = '\0';
   replacement = 'X';
   string.replace(target, replacement);
   TST_ASSERT_EQUALS("", string);

   string = "It was a dark and stormy night.";
   target = ' ';
   replacement = '\0';
   string.replace(target, replacement);
   TST_ASSERT_EQUALS("It", string);

   string = "It was a dark and stormy night.";
   target = 'i';
   replacement = 'X';
   string.replace(target, replacement);
   TST_ASSERT_EQUALS("It was a dark and stormy nXght.", string);

   string = "It was a dark and stormy night.";
   target = ' ';
   replacement = '\t';
   string.replace(target, replacement);
   TST_ASSERT_EQUALS("It\twas\ta\tdark\tand\tstormy\tnight.", string);

   const int buffer_size = 10;
   const char buffer[buffer_size] = { 9, 3, 0, 2, 4, 0, 7, 0, 0, 3 };
   COLstring BinaryString(buffer, buffer_size);
   BinaryString.replace('\003', '\006');
   const char expected_buffer[buffer_size] = { 9, 6, 0, 2, 4, 0, 7, 0, 0, 6 };
   TST_ASSERT_EQUALS(0, ::memcmp(BinaryString.data(), expected_buffer, buffer_size));
   BinaryString.replace('\000', (char)99);
   const char expected_buffer2[buffer_size] = { 9, 6, 99, 2, 4, 99, 7, 99, 99, 6 };
   TST_ASSERT_EQUALS(0, ::memcmp(BinaryString.data(), expected_buffer2, buffer_size));
}

void testCOLstringIsNull(){
   COLstring string;
   bool result;

   string = "";
   TST_ASSERT_EQUALS(1, string.is_null());

   string = "\0";
   TST_ASSERT_EQUALS(1, string.is_null());

   string = " ";
   TST_ASSERT_EQUALS(0, string.is_null());

   string = "\n";
   TST_ASSERT_EQUALS(0, string.is_null());

   string = "\t";
   TST_ASSERT_EQUALS(0, string.is_null());

   string = "It was a dark\0 and stormy night.";
   TST_ASSERT_EQUALS(0, string.is_null());

   string = "It was a dark and stormy night.";
   string[5] = '\0';
   TST_ASSERT_EQUALS(0, string.is_null());

   string = "hello";
   string[0] = '\0';
   TST_ASSERT_EQUALS(0, string.is_null());

   COLstring test("Hello");
   test[0] = '\0';
   TST_ASSERT_EQUALS(0, string.is_null());
}

void testCOLstringStripAllWithEmbeddedNulls(){
   COLstring String1 = "_A_BCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijk";
   char* Direct = String1.get_buffer();
   Direct[5] = 0;
   Direct[21] = 0;
   String1.stripAll('_');
   TST_ASSERT_EQUALS(3, String1.size());
   TST_ASSERT_EQUALS("ABC", String1);
}

void testCOLstringTrailingStrip(){
   COLstring Message = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"; // must be > 16
   COLstring Expected = Message.strip(COLstring::Trailing, 'A');
   TST_ASSERT_EQUALS(0, Expected.size());
   TST_ASSERT_EQUALS("", Expected);
   TST_ASSERT_EQUALS("xyz", (Expected + "xyz"));
}

void testCOLstringOverlap(){
   COLstring String1("TheQuickBrownFoxJumpsOverTheLazyDog");
   String1 = String1.c_str() + 3;
   TST_ASSERT_EQUALS("QuickBrownFoxJumpsOverTheLazyDog", String1);
}

void testCOLstringWriteBinary(){
   COLstring String1;

   String1.write("\000\001\002\003\004", 5);
   TST_ASSERT_EQUALS(5, String1.size());

   String1.write("\000\007\000", 3);
   TST_ASSERT_EQUALS(8, String1.size());

   String1.write("\000", 1);
   TST_ASSERT_EQUALS(9, String1.size());

   String1.write("ignored", 0);
   TST_ASSERT_EQUALS(9, String1.size());

   const char* const s = String1.c_str();
   TST_ASSERT_EQUALS(0, s[0]);
   TST_ASSERT_EQUALS(1, s[1]);
   TST_ASSERT_EQUALS(2, s[2]);
   TST_ASSERT_EQUALS(3, s[3]);
   TST_ASSERT_EQUALS(4, s[4]);
   TST_ASSERT_EQUALS(0, s[5]);
   TST_ASSERT_EQUALS(7, s[6]);
   TST_ASSERT_EQUALS(0, s[7]);
   TST_ASSERT_EQUALS(0, s[8]);
}

void testCOLstringAppendBinary(){
   COLstring String1;

   String1.append("\000\001\002\003\004", 5);
   TST_ASSERT_EQUALS(5, String1.size());

   String1.append("\000\007\000", 3);
   TST_ASSERT_EQUALS(8, String1.size());

   String1.append("\000", 1);
   TST_ASSERT_EQUALS(9, String1.size());

   String1.append("Ignored", 0);
   TST_ASSERT_EQUALS(9, String1.size());

   const char* const s = String1.c_str();
   TST_ASSERT_EQUALS(0, s[0]);
   TST_ASSERT_EQUALS(1, s[1]);
   TST_ASSERT_EQUALS(2, s[2]);
   TST_ASSERT_EQUALS(3, s[3]);
   TST_ASSERT_EQUALS(4, s[4]);
   TST_ASSERT_EQUALS(0, s[5]);
   TST_ASSERT_EQUALS(7, s[6]);
   TST_ASSERT_EQUALS(0, s[7]);
   TST_ASSERT_EQUALS(0, s[8]);
}

void testCOLstringEmptyStringAssignment(){
   COLstring String = "abc";
   TST_ASSERT_EQUALS("abc", String);
   TST_ASSERT_EQUALS(3, String.size());

   String = "some other string";
   TST_ASSERT_EQUALS(17, String.size());
   TST_ASSERT_EQUALS("some other string", String);

   String = "";
   TST_ASSERT_EQUALS(0, String.size());
   TST_ASSERT_EQUALS("", String);
}

void testCOLstringNullCharPointerAssignment(){
   COLstring String = "Abc";
   TST_ASSERT_EQUALS("Abc", String);
   TST_ASSERT_EQUALS(3, String.size());

   String = (const char*)0;
   TST_ASSERT_EQUALS(0, String.size());
   TST_ASSERT_EQUALS("", String);

   String = "some other string";
   TST_ASSERT_EQUALS(17, String.size());
   TST_ASSERT_EQUALS("some other string", String);

   String.operator=((const char*)0);
   TST_ASSERT_EQUALS(0, String.size());
   TST_ASSERT_EQUALS("", String);
}

void testCOLstringReplaceAllWhiteSpace(){
   const char Replacement = '~';
   const COLstring String = "    ABC        ";
   const COLstring String2 = " ";
   const COLstring String3 = "A B C";

   TST_ASSERT_EQUALS("~ABC~", COLreplaceAllWhiteSpace(String, Replacement));
   TST_ASSERT_EQUALS("~", COLreplaceAllWhiteSpace(String2, Replacement));
   TST_ASSERT_EQUALS("A~B~C", COLreplaceAllWhiteSpace(String3, Replacement));

}

void testCOLstringRemoveAllWhiteSpace(){
   const COLstring String = "    ABC        ";
   const COLstring String2 = " ";
   const COLstring String3 = "A B C";

   TST_ASSERT_EQUALS("ABC", COLremoveAllWhiteSpace(String));
   TST_ASSERT_EQUALS("", COLremoveAllWhiteSpace(String2));
   TST_ASSERT_EQUALS("ABC", COLremoveAllWhiteSpace(String3));
}

void testCOLstringStripChars() {
   const COLstring DeathList = "^%$.\\/@,<>";
   const COLstring String = "會^%$意..字\\/會@@字";
   const COLstring String2 = ",.<>會意#字$%^";
   const COLstring String3 = "#^&$%=@=";

   TST_ASSERT_EQUALS("會意字會字", COLstripChars(String, DeathList));
   TST_ASSERT_EQUALS("會意#字", COLstripChars(String2, DeathList));
   TST_ASSERT_EQUALS("#&==", COLstripChars(String3, DeathList));
}

static void testCOLintegerToStringConversions(){
#define MAX_UINT32   "4294967295"
#define MAX_INT32    "2147483647"
#define MIN_INT32   "-2147483648"

#define MAX_UINT64  "18446744073709551615"
#define MAX_INT64    "9223372036854775807"
#define MIN_INT64   "-9223372036854775808"

   TST_ASSERT_EQUALS( MAX_INT32, COLintToString((int)COL_INT_32_MAX) );
   TST_ASSERT_EQUALS( MIN_INT32, COLintToString((int)COL_INT_32_MIN) );

   TST_ASSERT_EQUALS( MAX_UINT32, COLuint32ToString(COL_UINT_32_MAX) );
   TST_ASSERT_EQUALS( MAX_INT32,   COLint32ToString( COL_INT_32_MAX) );
   TST_ASSERT_EQUALS( MIN_INT32,   COLint32ToString( COL_INT_32_MIN) );

   TST_ASSERT_EQUALS( MAX_UINT64, COLuint64ToString(COL_UINT_64_MAX) );
   TST_ASSERT_EQUALS( MAX_INT64,   COLint64ToString( COL_INT_64_MAX) );
   TST_ASSERT_EQUALS( MIN_INT64,   COLint64ToString( COL_INT_64_MIN) );
}

void testCOLstringClear(){
   int oldCapacity = 0;

   COLstring COLEmptyString(emptyString);
   oldCapacity = COLEmptyString.capacity();
   COLEmptyString.clear();
   TST_ASSERT_EQUALS("", COLEmptyString);
   TST_ASSERT_EQUALS(0, COLEmptyString.size());
   TST_ASSERT_EQUALS(oldCapacity, COLEmptyString.capacity());

   COLstring COLBasicString(basicString);
   oldCapacity = COLBasicString.capacity();
   COLBasicString.clear();
   TST_ASSERT_EQUALS("", COLBasicString);
   TST_ASSERT_EQUALS(0, COLBasicString.size());
   TST_ASSERT_EQUALS(oldCapacity, COLBasicString.capacity());

   COLstring COLShortString(shortString);
   oldCapacity = COLShortString.capacity();
   COLShortString.clear();
   TST_ASSERT_EQUALS("", COLShortString);
   TST_ASSERT_EQUALS(0, COLShortString.size());
   TST_ASSERT_EQUALS(oldCapacity, COLShortString.capacity());

   COLstring COLSymbols(symbols);
   oldCapacity = COLSymbols.capacity();
   COLSymbols.clear();
   TST_ASSERT_EQUALS("", COLSymbols);
   TST_ASSERT_EQUALS(0, COLSymbols.size());
   TST_ASSERT_EQUALS(oldCapacity, COLSymbols.capacity());

   COLstring COLNumbers(numbers);
   oldCapacity = COLNumbers.capacity();
   COLNumbers.clear();
   TST_ASSERT_EQUALS("", COLNumbers);
   TST_ASSERT_EQUALS(0, COLNumbers.size());
   TST_ASSERT_EQUALS(oldCapacity, COLNumbers.capacity());
}

void testCOLstringZero(){
   int oldCapacity = 0;

   COLstring COLEmptyString(emptyString);
   oldCapacity = COLEmptyString.capacity();
   COLEmptyString.zero();
   TST_ASSERT_EQUALS("", COLEmptyString);
   TST_ASSERT_EQUALS(0, COLEmptyString.size());
   TST_ASSERT_EQUALS(oldCapacity, COLEmptyString.capacity());

   COLstring COLBasicString(basicString);
   oldCapacity = COLBasicString.capacity();
   COLBasicString.zero();
   TST_ASSERT_EQUALS("", COLBasicString);
   TST_ASSERT_EQUALS(0, COLBasicString.size());
   TST_ASSERT(COLBasicString.capacity() < oldCapacity);

   COLstring COLShortString(shortString);
   oldCapacity = COLShortString.capacity();
   COLShortString.zero();
   TST_ASSERT_EQUALS("", COLShortString);
   TST_ASSERT_EQUALS(0, COLShortString.size());
   TST_ASSERT(COLShortString.capacity() <= oldCapacity);

   COLstring COLSymbols(symbols);
   oldCapacity = COLSymbols.capacity();
   COLSymbols.zero();
   TST_ASSERT_EQUALS("", COLSymbols);
   TST_ASSERT_EQUALS(0, COLSymbols.size());
   TST_ASSERT(COLSymbols.capacity() <= oldCapacity);

   COLstring COLNumbers(numbers);
   oldCapacity = COLNumbers.capacity();
   COLNumbers.zero();
   TST_ASSERT_EQUALS("", COLNumbers);
   TST_ASSERT_EQUALS(0, COLNumbers.size());
   TST_ASSERT(COLNumbers.capacity() <= oldCapacity);
}

void testCOLstringSwap(){
   COLstring String1 = "1234567890123456789012345678901234567890123";
   const int String1originalSize     = String1.size();
   const int String1originalCapacity = String1.capacity();
   TST_ASSERT_EQUALS(43, String1.size());
   TST_ASSERT(String1.capacity() > 43);

   COLstring String2 = "abcde";
   const int String2originalSize     = String2.size();
   const int String2originalCapacity = String2.capacity();
   TST_ASSERT_EQUALS(5, String2.size());
   TST_ASSERT(String2.capacity() > 5);

   String1.swap(&String2);
   TST_ASSERT_EQUALS(String1originalSize,     String2.size());
   TST_ASSERT_EQUALS(String1originalCapacity, String2.capacity());
   TST_ASSERT_EQUALS(String2originalSize,     String1.size());
   TST_ASSERT_EQUALS(String2originalCapacity, String1.capacity());

   String1.swap(&String2);
   TST_ASSERT_EQUALS(String1originalSize,     String1.size());
   TST_ASSERT_EQUALS(String1originalCapacity, String1.capacity());
   TST_ASSERT_EQUALS(String2originalSize,     String2.size());
   TST_ASSERT_EQUALS(String2originalCapacity, String2.capacity());

   String1.swap(&String1);
   String2.swap(&String2);
   TST_ASSERT_EQUALS(String1originalSize,     String1.size());
   TST_ASSERT_EQUALS(String1originalCapacity, String1.capacity());
   TST_ASSERT_EQUALS(String2originalSize,     String2.size());
   TST_ASSERT_EQUALS(String2originalCapacity, String2.capacity());
}

void testCOLstringSubstrreplace(){
   COLuint32 replaceCount = 0;

   COLstring string = "It was a dark and stormy night.";
   COLstring substring = "";
   COLstring replacementString = "replacement string";

   replaceCount = string.replace(substring, replacementString);

   TST_ASSERT_EQUALS("It was a dark and stormy night.", string);
   TST_ASSERT_EQUALS(0, replaceCount);

   string = "";
   substring = "";
   replacementString = "replacement string";

   replaceCount = string.replace(substring, replacementString);

   TST_ASSERT_EQUALS("", string);
   TST_ASSERT_EQUALS(0, replaceCount);

   string = "It was a dark and stormy night.";
   substring = "it was";
   replacementString = "It was not";

   replaceCount = string.replace(substring, replacementString);

   TST_ASSERT_EQUALS("It was a dark and stormy night.", string);
   TST_ASSERT_EQUALS(0, replaceCount);

   string = "It was a dark and stormy night.";
   substring = "It was";
   replacementString = "It was not";

   replaceCount = string.replace(substring, replacementString);

   TST_ASSERT_EQUALS("It was not a dark and stormy night.", string);
   TST_ASSERT_EQUALS(1, replaceCount);

   string = "abcdefgabcdefabcdeabc";
   substring = "abcdef";
   replacementString = "abcdefg";

   replaceCount = string.replace(substring, replacementString);

   TST_ASSERT_EQUALS("abcdefggabcdefgabcdeabc", string);
   TST_ASSERT_EQUALS(2, replaceCount);

   string = "\n\t\nhello123$0a";
   substring = "\nhello";
   replacementString = "goodbye";

   replaceCount = string.replace(substring, replacementString);

   TST_ASSERT_EQUALS("\n\tgoodbye123$0a", string);
   TST_ASSERT_EQUALS(1, replaceCount);

   string = "\n\t\n hello123$0a";
   substring = "\nhello";
   replacementString = "goodbye";

   replaceCount = string.replace(substring, replacementString);

   TST_ASSERT_EQUALS("\n\t\n hello123$0a", string);
   TST_ASSERT_EQUALS(0, replaceCount);

   TST_ASSERT_EQUALS(5, COLstring("AAAAA").replace("A","A"));
   TST_ASSERT_EQUALS(5, COLstring("AAAAA").replace("A","AA"));
   TST_ASSERT_EQUALS(2, COLstring("AAAAA").replace("AA","A"));
   TST_ASSERT_EQUALS(1, COLstring("AAAAA").replace("AAA","A"));
   TST_ASSERT_EQUALS(1, COLstring("AAAAA").replace("AAA","AA"));
   TST_ASSERT_EQUALS(2, COLstring("AAAAA").replace("AA","AAA"));
   TST_ASSERT_EQUALS(1, COLstring("AAAAA").replace("AAAAA","AAAAAAAAAAAAAAAAAAAAAAAAAAAA"));

   TST_ASSERT_EQUALS(3, COLstring("xAAxA").replace("A","A"));
   TST_ASSERT_EQUALS(4, COLstring("AAxAA").replace("A","AA"));
   TST_ASSERT_EQUALS(1, COLstring("AxAAA").replace("AA","A"));
   TST_ASSERT_EQUALS(0, COLstring("AAxAA").replace("AAA","A"));
   TST_ASSERT_EQUALS(1, COLstring("AxAAA").replace("AAA","A"));
   TST_ASSERT_EQUALS(1, COLstring("AxAAA").replace("AAA","AA"));
   TST_ASSERT_EQUALS(2, COLstring("xAAAA").replace("AA","AAA"));
}

void testCOLstripWhiteSpace(){
   TST_ASSERT_EQUALS( COLstring("foo"), COLstripWhiteSpace("\r\n \t \tfoo\t \t\r\n", true, true));
   TST_ASSERT_EQUALS( COLstring("\r\n \t \tfoo"), COLstripWhiteSpace("\r\n \t \tfoo\t \t\r\n", false, true));
   TST_ASSERT_EQUALS( COLstring("foo\t \t\r\n"), COLstripWhiteSpace("\r\n \t \tfoo\t \t\r\n", true, false));
   TST_ASSERT_EQUALS( COLstring(""), COLstripWhiteSpace(" \r\n\t\n  \t   ", true, false) );
   TST_ASSERT_EQUALS( COLstring(""), COLstripWhiteSpace(" \r\n\t\n  \t   ", false, true) );
   TST_ASSERT_EQUALS( COLstring(""), COLstripWhiteSpace(" \r\n\t\n  \t   ", true, true) );
}

void testCOLlowerCase(){
   TST_ASSERT_EQUALS( COLstring("foo"), COLlowerCase("foo"));
   TST_ASSERT_EQUALS( COLstring("foo"), COLlowerCase("Foo"));
   TST_ASSERT_EQUALS( COLstring("foo"), COLlowerCase("FOO"));
   TST_ASSERT_EQUALS( COLstring(""), COLlowerCase("") );
}

void testCOLupperCase(){
   TST_ASSERT_EQUALS( COLstring("FOO"), COLupperCase("foo"));
   TST_ASSERT_EQUALS( COLstring("FOO"), COLupperCase("Foo"));
   TST_ASSERT_EQUALS( COLstring("FOO"), COLupperCase("FOO"));
   TST_ASSERT_EQUALS( COLstring(""), COLupperCase("") );
}

void testCOLstringRsplit(){
   COLstring string;
   COLstring Out1;
   COLstring Out2;
   const char* token;

   string = "/Users/user/desktop/hello.txt";
   token = "/";
   bool rsplitReturnCode = false;
   rsplitReturnCode = string.rsplit(Out1, Out2, token);
   TST_ASSERT_EQUALS(true, rsplitReturnCode);
   TST_ASSERT_EQUALS("/Users/user/desktop", Out1);
   TST_ASSERT_EQUALS("hello.txt", Out2);

   string = "Hello Goodbye";
   token = " ";
   rsplitReturnCode = string.rsplit(Out1, Out2, token);
   TST_ASSERT_EQUALS(true, rsplitReturnCode);
   TST_ASSERT_EQUALS("Hello", Out1);
   TST_ASSERT_EQUALS("Goodbye", Out2);

   string = "Hello\n Goodbye";
   token = " ";
   rsplitReturnCode = string.rsplit(Out1, Out2, token);
   TST_ASSERT_EQUALS(true, rsplitReturnCode);
   TST_ASSERT_EQUALS("Hello\n", Out1);
   TST_ASSERT_EQUALS("Goodbye", Out2);

   string = "C:\\Users\\Documents and Settings\\user";
   token = "\\";
   rsplitReturnCode = string.rsplit(Out1, Out2, token);
   TST_ASSERT_EQUALS(true, rsplitReturnCode);
   TST_ASSERT_EQUALS("C:\\Users\\Documents and Settings", Out1);
   TST_ASSERT_EQUALS("user", Out2);

   // FogBugz Case 22607 - was an infinite loop
   string = "It was a dark and stormy night.";
   token = "";
   Out1 = "abc";
   Out2 = "xyz";
   rsplitReturnCode = string.rsplit(Out1, Out2, token);
   TST_ASSERT_EQUALS(false, rsplitReturnCode);
   TST_ASSERT_EQUALS("", Out1);
   TST_ASSERT_EQUALS("It was a dark and stormy night.", Out2);

   // FogBugz Case 22610
   string = "MSH|Behrang^Farzan";
   token = "|^";
   Out1 = "abc";
   Out2 = "123";
   rsplitReturnCode = string.rsplit(Out1, Out2, token);
   TST_ASSERT_EQUALS(false, rsplitReturnCode);
   TST_ASSERT_EQUALS("", Out1);
   TST_ASSERT_EQUALS("MSH|Behrang^Farzan", Out2);

   // Should be same as above, since token still isn't found.
   // The difference is only in the length of the token.
   string = "MSH|Behrang^Farzan";
   token = "$";
   Out1 = "abc";
   Out2 = "123";
   rsplitReturnCode = string.rsplit(Out1, Out2, token);
   TST_ASSERT_EQUALS(false, rsplitReturnCode);
   TST_ASSERT_EQUALS("", Out1);
   TST_ASSERT_EQUALS("MSH|Behrang^Farzan", Out2);

   string = "MSH|Behrang|Farzan|Foo|Bar";
   token = "|F";
   Out1 = "abc";
   Out2 = "def";
   rsplitReturnCode = string.rsplit(Out1, Out2, token);
   TST_ASSERT_EQUALS(true, rsplitReturnCode);
   TST_ASSERT_EQUALS("MSH|Behrang|Farzan", Out1);
   TST_ASSERT_EQUALS("oo|Bar", Out2);
   string = Out1;
   Out1 = "abc";
   Out2 = "def";
   rsplitReturnCode = string.rsplit(Out1, Out2, token);
   TST_ASSERT_EQUALS(true, rsplitReturnCode);
   TST_ASSERT_EQUALS("MSH|Behrang", Out1);
   TST_ASSERT_EQUALS("arzan", Out2);
}

void testCOLstringSplit(){
   COLstring string;
   COLstring Out1;
   COLstring Out2;
   const char* token;
   bool status;

   string = "/Users/user/desktop/hello.txt";
   token = "/";
   status = string.split(Out1, Out2, token);
   TST_ASSERT_EQUALS(1, status);
   TST_ASSERT_EQUALS("", Out1);
   TST_ASSERT_EQUALS("Users/user/desktop/hello.txt", Out2);

   string = "Hello Goodbye";
   token = " ";
   status = string.split(Out1, Out2, token);
   TST_ASSERT_EQUALS(1, status);
   TST_ASSERT_EQUALS("Hello", Out1);
   TST_ASSERT_EQUALS("Goodbye", Out2);

   string = "Hello\n Goodbye";
   token = " ";
   status = string.split(Out1, Out2, token);
   TST_ASSERT_EQUALS(1, status);
   TST_ASSERT_EQUALS("Hello\n", Out1);
   TST_ASSERT_EQUALS("Goodbye", Out2);

   string = "C:\\Users\\Documents and Settings\\user";
   token = "\\";
   status = string.split(Out1, Out2, token);
   TST_ASSERT_EQUALS(1, status);
   TST_ASSERT_EQUALS("C:", Out1);
   TST_ASSERT_EQUALS("Users\\Documents and Settings\\user", Out2);

   string = "MSH|Behrang^Farzan";
   token = "|^";
   status = string.split(Out1, Out2, token);
   TST_ASSERT_EQUALS(0, status);
   TST_ASSERT_EQUALS("MSH|Behrang^Farzan", Out1);

   string = "Name:\nUser:\nPassword";
   token = ":\n";
   status = string.split(Out1, Out2, token);
   TST_ASSERT_EQUALS(1, status);
   TST_ASSERT_EQUALS("Name", Out1);
   TST_ASSERT_EQUALS("User:\nPassword", Out2);

   string = "MSH|^$|CPH|^$|";
   token = "|^$|";
   status = string.split(Out1, Out2, token);
   TST_ASSERT_EQUALS(1, status);
   TST_ASSERT_EQUALS("MSH", Out1);
   TST_ASSERT_EQUALS("CPH|^$|", Out2);
}

void testCOLstringSplitAtIndex(){
   COLstring string, Out1, Out2;
   COLuint32 splitIndex;

   string = "It was a dark and stormy night.";
   splitIndex = 0;
   string.splitAtIndex(Out1, Out2, splitIndex);
   TST_ASSERT_EQUALS("", Out1);
   TST_ASSERT_EQUALS("It was a dark and stormy night.", Out2);

   string = "It was a dark and stormy night.";
   splitIndex = 6;
   string.splitAtIndex(Out1, Out2, splitIndex);
   TST_ASSERT_EQUALS("It was", Out1);
   TST_ASSERT_EQUALS(" a dark and stormy night.", Out2);

   string = "It was a dark and stormy night.";
   splitIndex = 50;
   string.splitAtIndex(Out1, Out2, splitIndex);
   TST_ASSERT_EQUALS("It was a dark and stormy night.", Out1);
   TST_ASSERT_EQUALS("", Out2);

   string = "";
   splitIndex = 0;
   string.splitAtIndex(Out1, Out2, splitIndex);
   TST_ASSERT_EQUALS("", Out1);
   TST_ASSERT_EQUALS("", Out2);

   string = "C:\\Program Files\\iNTERFACEWARE";
   splitIndex = 2;
   string.splitAtIndex(Out1, Out2, splitIndex);
   TST_ASSERT_EQUALS("C:", Out1);
   TST_ASSERT_EQUALS("\\Program Files\\iNTERFACEWARE", Out2);

   string = "User\nName\n \t ";
   splitIndex = 10;
   string.splitAtIndex(Out1, Out2, splitIndex);
   TST_ASSERT_EQUALS("User\nName\n", Out1);
   TST_ASSERT_EQUALS(" \t ", Out2);
}

void testCOLstringSplitWhiteSpace(){
   COLarray<COLstring> PartList;

   PartList.clear();
   COLsplitWhiteSpace(&PartList, "hello world");
   TST_ASSERT_EQUALS(2, PartList.size());
   TST_ASSERT_EQUALS(COLstring("hello"), PartList[0]);
   TST_ASSERT_EQUALS(COLstring("world"), PartList[1]);

   // same thing - only non-space chunks count
   PartList.clear();
   COLsplitWhiteSpace(&PartList, "   hello world ");
   TST_ASSERT_EQUALS(2, PartList.size());
   TST_ASSERT_EQUALS(COLstring("hello"), PartList[0]);
   TST_ASSERT_EQUALS(COLstring("world"), PartList[1]);

   // empty string - no chunks
   PartList.clear();
   COLsplitWhiteSpace(&PartList, "");
   TST_ASSERT_EQUALS(0, PartList.size());

   // space

   PartList.clear();
   COLsplitWhiteSpace(&PartList, "foo \t bar zoo", true);
   TST_ASSERT_EQUALS(5, PartList.size());
   TST_ASSERT_EQUALS(COLstring("foo"), PartList[0]);
   TST_ASSERT_EQUALS(COLstring(" \t "), PartList[1]);
   TST_ASSERT_EQUALS(COLstring("bar"), PartList[2]);
   TST_ASSERT_EQUALS(COLstring(" "), PartList[3]);
   TST_ASSERT_EQUALS(COLstring("zoo"), PartList[4]);

   PartList.clear();
   COLsplitWhiteSpace(&PartList, " foo \t bar zoo   ", true);
   TST_ASSERT_EQUALS(7, PartList.size());
   TST_ASSERT_EQUALS(COLstring(" "), PartList[0]);
   TST_ASSERT_EQUALS(COLstring("foo"), PartList[1]);
   TST_ASSERT_EQUALS(COLstring(" \t "), PartList[2]);
   TST_ASSERT_EQUALS(COLstring("bar"), PartList[3]);
   TST_ASSERT_EQUALS(COLstring(" "), PartList[4]);
   TST_ASSERT_EQUALS(COLstring("zoo"), PartList[5]);
   TST_ASSERT_EQUALS(COLstring("   "), PartList[6]);
}

void testCOLstringEquals(){
   TST_ASSERT_EQUALS(true,  COLstring("").equals(""));
   TST_ASSERT_EQUALS(true,  COLstring("ABC").equals("ABC"));
   TST_ASSERT_EQUALS(false, COLstring("ABC").equals("abc"));
   TST_ASSERT_EQUALS(false, COLstring("abc").equals("ABc"));
   TST_ASSERT_EQUALS(false, COLstring("ABC").equals("ABCD"));
   TST_ASSERT_EQUALS(false, COLstring("ABCD").equals("ABC"));

   TST_ASSERT_EQUALS(true,  COLstring("").equals(COLstring("")));
   TST_ASSERT_EQUALS(true,  COLstring("ABC").equals(COLstring("ABC")));
   TST_ASSERT_EQUALS(false, COLstring("ABC").equals(COLstring("abc")));
   TST_ASSERT_EQUALS(false, COLstring("abc").equals("ABC"));
   TST_ASSERT_EQUALS(false, COLstring("ABC").equals(COLstring("ABCD")));
   TST_ASSERT_EQUALS(false, COLstring("ABCD").equals(COLstring("ABC")));
}

void testCOLstringEqualsIgnoreCase(){
   TST_ASSERT_EQUALS(true,  COLstring("").equalsIgnoreCase(""));
   TST_ASSERT_EQUALS(true,  COLstring("ABC").equalsIgnoreCase("ABC"));
   TST_ASSERT_EQUALS(true,  COLstring("ABC").equalsIgnoreCase("abc"));
   TST_ASSERT_EQUALS(true,  COLstring("abc").equalsIgnoreCase("ABC"));
   TST_ASSERT_EQUALS(false, COLstring("ABC").equalsIgnoreCase("ABCD"));
   TST_ASSERT_EQUALS(false, COLstring("ABCD").equalsIgnoreCase("ABC"));

   TST_ASSERT_EQUALS(true,  COLstring("").equalsIgnoreCase(COLstring("")));
   TST_ASSERT_EQUALS(true,  COLstring("ABC").equalsIgnoreCase(COLstring("ABC")));
   TST_ASSERT_EQUALS(true,  COLstring("ABC").equalsIgnoreCase(COLstring("abc")));
   TST_ASSERT_EQUALS(true,  COLstring("abc").equalsIgnoreCase(COLstring("ABC")));
   TST_ASSERT_EQUALS(false, COLstring("ABC").equalsIgnoreCase(COLstring("ABCD")));
   TST_ASSERT_EQUALS(false, COLstring("ABCD").equalsIgnoreCase(COLstring("ABC")));
}

void testCOLstringIcomp(){
   TST_ASSERT_EQUALS(0, COLstring::icompare("A","a"));
   TST_ASSERT_EQUALS(0, COLstring::icompare("a","A"));
   TST_ASSERT_EQUALS(65, COLstring::icompare("A",""));
   TST_ASSERT_EQUALS(-65, COLstring::icompare("","A"));
   TST_ASSERT_EQUALS(0, COLstring("").icompare(""));
   TST_ASSERT_EQUALS(0, COLstring("").icompare(COLstring("")));
   TST_ASSERT_EQUALS(0, COLstring("ABC").icompare("abc"));
   TST_ASSERT_EQUALS(0, COLstring("abc").icompare("ABC"));
   TST_ASSERT_EQUALS(0, COLstring("Abc").icompare("ABc"));
   TST_ASSERT_EQUALS(0, COLstring("abc").icompare("abc"));
   TST_ASSERT_EQUALS(0, COLstring("ABC").icompare(COLstring("abc")));
   TST_ASSERT_EQUALS(0, COLstring("abc").icompare(COLstring("ABC")));
   TST_ASSERT_EQUALS(0, COLstring("Abc").icompare(COLstring("ABc")));
   TST_ASSERT_EQUALS(0, COLstring("abc").icompare(COLstring("abc")));
   TST_ASSERT_EQUALS(65, COLstring("ABCA").icompare("ABC"));
   TST_ASSERT_EQUALS(-65, COLstring("ABC").icompare("ABCA"));
}

// No real tests here - just want to see the COLlog output for dumping buffers
// and to see whether this will compile in Release mode.
void testCOLlogDump(){
   COL_FUNCTION(testCOLlogDump);
   const char buf[] = "\177\176\175\000\001\002\003\004\005\006\007\010\011\012\013\014\015\016\017\020\021\022\023\024\025\026Fred\rFlintstone\njumps over\r\nthe Lazy DOG! (\000) <- nil between parentheses";
   const int len = sizeof(buf)-1;

   COL_DUMP("COL_DUMP", buf, len);

   COL_DBG_DUMP("COL_DBG_DUMP", buf, len);
   COL_TRC_DUMP("COL_TRC_DUMP", buf, len);
   COL_INF_DUMP("COL_INF_DUMP", buf, len);
   COL_WRN_DUMP("COL_WRN_DUMP", buf, len);
   COL_ERR_DUMP("COL_ERR_DUMP", buf, len);

   COL_TRC_DUMP("Test negative len.", buf, -1);
   COL_TRC_DUMP("Test zero len.", buf, 0);
   COL_TRC_DUMP("Test len of 1.", buf, 1);
   COL_TRC_DUMP("Test data is NULL", (void*)NULL, len);
}

static void testShift(){
   COLstring s;
   int Capacity = s.capacity();
   TST_ASSERT_EQUALS(0, s.size());
   TST_ASSERT_EQUALS(Capacity, s.capacity());
   TST_ASSERT(s.capacity() > s.size());

   s.zero();
   s += "123456789012345678901234567890";
   Capacity = s.capacity();
   s.shift(30);
   TST_ASSERT_EQUALS("", s);
   TST_ASSERT_EQUALS(0, s.size());
   TST_ASSERT_EQUALS(Capacity, s.capacity());
   TST_ASSERT(s.capacity() > s.size());
   TST_ASSERT(s.capacity() == Capacity);

   s.zero();
   s += "123456789012345678901234567890";
   Capacity = s.capacity();
   s.shift(29);
   TST_ASSERT_EQUALS("0", s);
   TST_ASSERT_EQUALS(1, s.size());
   TST_ASSERT_EQUALS(Capacity, s.capacity());
   TST_ASSERT(s.capacity() > s.size());
   TST_ASSERT(s.capacity() == Capacity);

   s.zero();
   s += "123456789012345678901234567890";
   Capacity = s.capacity();
   s.shift(28);
   TST_ASSERT_EQUALS("90", s);
   TST_ASSERT_EQUALS(2, s.size());
   TST_ASSERT_EQUALS(Capacity, s.capacity());
   TST_ASSERT(s.capacity() > s.size());
   TST_ASSERT(s.capacity() == Capacity);

   s.zero();
   s += "123456789012345678901234567890";
   Capacity = s.capacity();
   s.shift(27);
   TST_ASSERT_EQUALS("890", s);
   TST_ASSERT_EQUALS(3, s.size());
   TST_ASSERT_EQUALS(Capacity, s.capacity());
   TST_ASSERT(s.capacity() > s.size());
   TST_ASSERT(s.capacity() == Capacity);

   s.zero();
   s += "123456789012345678901234567890";
   Capacity = s.capacity();
   s.shift(1);
   TST_ASSERT_EQUALS("23456789012345678901234567890", s);
   TST_ASSERT_EQUALS(29, s.size());
   TST_ASSERT_EQUALS(Capacity, s.capacity());
   TST_ASSERT(s.capacity() > s.size());
   TST_ASSERT(s.capacity() == Capacity);

   s.zero();
   s += "123456789012345678901234567890";
   Capacity = s.capacity();
   s.shift(2);
   TST_ASSERT_EQUALS("3456789012345678901234567890", s);
   TST_ASSERT_EQUALS(28, s.size());
   TST_ASSERT_EQUALS(Capacity, s.capacity());
   TST_ASSERT(s.capacity() > s.size());
   TST_ASSERT(s.capacity() == Capacity);

   // test COLASSERT
   //s.zero();
   //s += "123456789012345678901234567890";
   //Capacity = s.capacity();
   //s.shift(-1);

   // test COLASSERT
   //s.zero();
   //s += "123456789012345678901234567890";
   //Capacity = s.capacity();
   //s.shift(31);

   s.zero();
   s += "123456789012345678901234567890";
   Capacity = s.capacity();
   s.shift(0);
   TST_ASSERT_EQUALS("123456789012345678901234567890", s);
   TST_ASSERT_EQUALS(30, s.size());
   TST_ASSERT_EQUALS(Capacity, s.capacity());
   TST_ASSERT(s.capacity() > s.size());
   TST_ASSERT(s.capacity() == Capacity);

   const char* b30 = "123456789012345678901234567890";
   s.zero();
   s += b30;
   Capacity = s.capacity();
   int ShiftCount = 0;
   for (int i = 29; i >= 0; --i) {
      s.shift(1);
      ShiftCount++;
      TST_ASSERT_EQUALS(i, s.size());
      TST_ASSERT_EQUALS(b30+ShiftCount, s);
      TST_ASSERT_EQUALS(Capacity, s.capacity());
      TST_ASSERT(s.capacity() > s.size());
      TST_ASSERT(s.capacity() == Capacity);
   }

   TST_ASSERT(s.capacity() == Capacity);
}

static void testShrink(){
   COLstring s;
   int Capacity = s.capacity();
   TST_ASSERT_EQUALS(Capacity, s.capacity());
   for (int i = -20; i < 50; ++i) {
      s.shrink(i);
      TST_ASSERT_EQUALS(Capacity, s.capacity());
   }

   s.zero();
   s += "1234567890";
   TST_ASSERT(s.capacity() >= 11);
   s.setCapacity(500);
   TST_ASSERT(s.capacity() >= 500);
   Capacity = s.capacity();
   for (int i = -20; i < 50; ++i) {
      s.shrink(50);
      TST_ASSERT_EQUALS(10, s.size());
      TST_ASSERT_EQUALS(50, s.capacity());
      TST_ASSERT(s.size() < s.capacity());
      TST_ASSERT(s.capacity() < 100);
   }
   TST_ASSERT_EQUALS("1234567890", s);
   TST_ASSERT(s.capacity() < 100);

   for (int i = -20; i < 70; ++i) {
      s.zero();
      s += "123456789012345";
      TST_ASSERT(s.capacity() >= 15);
      s.setCapacity(500);
      TST_ASSERT(s.capacity() >= 500);
      Capacity = s.capacity();
      s.shrink(i);
      TST_ASSERT_EQUALS("123456789012345", s);
      TST_ASSERT_EQUALS(15, s.size());
      TST_ASSERT(s.size() < s.capacity());
      TST_ASSERT(s.capacity() < 100);
      const int expected = COL_MAX(32, i);
      TST_ASSERT_EQUALS(expected, s.capacity());
   }
   TST_ASSERT_EQUALS("123456789012345", s);
   TST_ASSERT_EQUALS(15, s.size());
   TST_ASSERT(s.size() < s.capacity());
   TST_ASSERT(s.capacity() < 100);

   s.zero();
   s += "12345678901234567890123456789012345678901234567890";
   TST_ASSERT(s.capacity() >= 51);
   s.setCapacity(500);
   TST_ASSERT(s.capacity() >= 500);
   Capacity = s.capacity();
   for (int i = -20; i < 60; ++i) {
      s.shrink(i);
      TST_ASSERT_EQUALS(50, s.size());
      TST_ASSERT(s.size() < s.capacity());
      TST_ASSERT(s.capacity() < Capacity);
      TST_ASSERT(s.capacity() < 100);
   }
   TST_ASSERT_EQUALS("12345678901234567890123456789012345678901234567890", s);
   TST_ASSERT(s.capacity() < 100);

   s.zero();
   s += "12345678901234567890123456789012345678901234567890";
   TST_ASSERT(s.capacity() >= 51);
   s.setCapacity(500);
   TST_ASSERT(s.capacity() >= 500);
   Capacity = s.capacity();
   s.shrink(0);
   TST_ASSERT_EQUALS("12345678901234567890123456789012345678901234567890", s);
   TST_ASSERT_EQUALS(50, s.size());
   TST_ASSERT(s.size() < s.capacity());
   TST_ASSERT_EQUALS(51, s.capacity());

   for (int i = -50; i < 51; ++i) {
      s.zero();
      s += "1234567890123456789012345";
      TST_ASSERT(s.capacity() >= 25+1);
      s.setCapacity(500);
      TST_ASSERT(s.capacity() >= 500);
      s.shrink(i);
      TST_ASSERT_EQUALS("1234567890123456789012345", s);
      TST_ASSERT_EQUALS(25, s.size());
      TST_ASSERT(s.size() < s.capacity());
      //COLcout << "i = " << i << ", s.capacity() = " << s.capacity() << newline;
      const int expected = COL_MAX(32, i);
      TST_ASSERT_EQUALS(expected, s.capacity());
   }
}

static void testStringAccess(){
   COLstring Str1("String with a trailing \\");
   COLcout << "Str1 = " << Str1 << newline;
   char c1 = Str1[Str1.size()-1];
   COLcout << "c1 = " << c1 << newline;
   TST_ASSERT_EQUALS('\\', c1);

   COLstring Str2("String without a trailing slash");
   COLcout << "Str2 = " << Str2 << newline;
   char c2 = Str2[Str2.size()-1];
   COLcout << "c2 = " << c2 << newline;
   TST_ASSERT('\\' != c2);

   // Remove the trailing slash.

   int Size = Str1.size();
   COLstring Trimmed = Str1.remove(Size-1, 1);
   COLcout << "Trimmed = " << Trimmed;
}

static void testStringContains() {
   COLstring SearchText("Some string with random garbage");
   COLstring Pattern = "random";
   TST_ASSERT(COLstringContains(SearchText, Pattern));
   TST_ASSERT(COLstringContains(SearchText.c_str(), Pattern.c_str()));
}

TSTtestable* COLstringTests() {
   TST_START_COLLECTION("COLstringTests")
      TST_ADD_TEST_FUNCTION(testCOLstringAdd);
      TST_ADD_TEST_FUNCTION(testCOLstringAppend);
      TST_ADD_TEST_FUNCTION(testCOLstringCapacity);
      TST_ADD_TEST_FUNCTION(testCOLstringIsWhitespace);
      TST_ADD_TEST_FUNCTION(testCOLstringCase);
      TST_ADD_TEST_FUNCTION(testCOLstringCstr);
      TST_ADD_TEST_FUNCTION(testCOLstringEquality);
      TST_ADD_TEST_FUNCTION(testCOLstringInequality);
      TST_ADD_TEST_FUNCTION(testCOLstringNilCharacterPlusEquals);
      TST_ADD_TEST_FUNCTION(testCOLstringCompare);
      TST_ADD_TEST_FUNCTION(testCOLstringTrailingStrip);
      TST_ADD_TEST_FUNCTION(testCOLstringConstructors);
      TST_ADD_TEST_FUNCTION(testCOLstringSubstring);
      TST_ADD_TEST_FUNCTION(testCOLstringSubstrreplace);
      TST_ADD_TEST_FUNCTION(testCOLstringSink);
      TST_ADD_TEST_FUNCTION(testCOLstringFind);
      TST_ADD_TEST_FUNCTION(testCOLstringMisc);
      TST_ADD_TEST_FUNCTION(testCOLstringEdits);
      TST_ADD_TEST_FUNCTION(testCOLstringAssign);
      TST_ADD_TEST_FUNCTION(testCOLstringSize);
      TST_ADD_TEST_FUNCTION(testCOLstringStripAll);
      TST_ADD_TEST_FUNCTION(testCOLstringRemove);
      TST_ADD_TEST_FUNCTION(testCOLstringReplace);
      TST_ADD_TEST_FUNCTION(testCOLstringPrepend);
      TST_ADD_TEST_FUNCTION(testCOLstringInsert);
      TST_ADD_TEST_FUNCTION(testCOLstringReplaceChar);
      TST_ADD_TEST_FUNCTION(testCOLstringIsNull);
      TST_ADD_TEST_FUNCTION(testCOLstringStripAllWithEmbeddedNulls);
      TST_ADD_TEST_FUNCTION(testCOLstringOverlap);
      TST_ADD_TEST_FUNCTION(testCOLstringWriteBinary);
      TST_ADD_TEST_FUNCTION(testCOLstringAppendBinary);
      TST_ADD_TEST_FUNCTION(testCOLstringEmptyStringAssignment);
      TST_ADD_TEST_FUNCTION(testCOLstringNullCharPointerAssignment);
      TST_ADD_TEST_FUNCTION(testCOLstringReplaceAllWhiteSpace);
      TST_ADD_TEST_FUNCTION(testCOLstringRemoveAllWhiteSpace);
      TST_ADD_TEST_FUNCTION(testCOLstringStripChars);
      TST_ADD_TEST_FUNCTION(testCOLintegerToStringConversions);
      TST_ADD_TEST_FUNCTION(testCOLstringClear);
      TST_ADD_TEST_FUNCTION(testCOLstringZero);
      TST_ADD_TEST_FUNCTION(testCOLstringSwap);
      TST_ADD_TEST_FUNCTION(testCOLstripWhiteSpace);
      TST_ADD_TEST_FUNCTION(testCOLlowerCase);
      TST_ADD_TEST_FUNCTION(testCOLupperCase);
      TST_ADD_TEST_FUNCTION(testCOLstringRsplit);
      TST_ADD_TEST_FUNCTION(testCOLstringSplit);
      TST_ADD_TEST_FUNCTION(testCOLstringSplitAtIndex);
      TST_ADD_TEST_FUNCTION(testCOLstringSplitWhiteSpace);
      TST_ADD_TEST_FUNCTION(testCOLstringIcomp);
      TST_ADD_TEST_FUNCTION(testCOLstringEqualsIgnoreCase);
      TST_ADD_TEST_FUNCTION(testCOLstringEquals);
      TST_ADD_TEST_FUNCTION(testCOLlogDump);
      TST_ADD_TEST_FUNCTION(testShift);
      TST_ADD_TEST_FUNCTION(testShrink);
      TST_ADD_TEST_FUNCTION(testStringAccess);
      TST_ADD_TEST_FUNCTION(testStringContains);
   TST_END_COLLECTION
}
