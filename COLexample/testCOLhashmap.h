#ifndef __TEST_COL_LOOKUP_LIST_H__
#define __TEST_COL_LOOKUP_LIST_H__
//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2004 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testCOLhashmap.h
//
// Description:
//
// A set of utilities to test COLhashmap objects.
//
// Author: Ian Farquharson
// Date:   Thu 02/05/2004
//
//
//---------------------------------------------------------------------------

class TSTtestable;

class testCOLhashmap
{
public:
   typedef COLint16 TestType;
   typedef COLlist<TestType>                 TestListType;
   typedef COLhashmap<TestType, TestType> TestLookupType;

   testCOLhashmap();
   virtual ~testCOLhashmap(){};

   static testCOLhashmap* create(){ return new testCOLhashmap; }

   void testCheck(const bool Condition);
   void testCompare();
   COLlistPlace find(const TestListType& List, const TestType Item);
   void remove(TestListType& List, const TestType Item);

   void initTest();

   void addTest();

   void removeTest();

   void clearTest();

   void iterationTest();

   void randomTest();

   void mapTest();

   void iterateTest();

   void iterateFindTest();

   void assignmentTest();

   void copyTest();

   //static test functions
   static void defaultHashTest();

   static TSTtestable* COLhashmapTests();

private:
   testCOLhashmap(const testCOLhashmap& TestLookupList);
   testCOLhashmap& operator=(const testCOLhashmap& TestLookupList);

   TestListType Control;
   TestLookupType TestLookup;

   void iteratorTestFunc(const COLuint32& Key, COLstring& Value);
   COLuint32 IteratorTestFuncReturn;

   COLuint32 TestCount;
   COLuint32 Sequence;
};

#endif     // end of defensive include
