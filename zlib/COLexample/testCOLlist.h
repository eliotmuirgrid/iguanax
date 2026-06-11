#ifndef __TEST_COL_LIST_H__
#define __TEST_COL_LIST_H__
//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2004 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testCOLlist.h
//
// Description:
//
// A set of utilities to test COLlist objects.
//
// Author: Ian Farquharson
// Date:   Thu 02/05/2004
//
//
//---------------------------------------------------------------------------

class TSTtestable;

class testCOLlist
{
public:
   typedef COLint16 TestType;

   testCOLlist() : TestCount(0), Sequence(0) {};
   virtual ~testCOLlist() {};

   static testCOLlist* create(){ return new testCOLlist; }

   void testCompare();

   void initTest();

   void removeTest();

   void insertTest();

   void clearTest();

   void addTest();

   void exchangeTest();

   static TSTtestable* COLlistTests();

private:
   testCOLlist(const testCOLlist& TestList);
   testCOLlist& operator=(const testCOLlist& TestList);

   COLarray<TestType>  Control;
   COLlist<TestType>   TestList;

   COLarray<TestType>  TestVector;
   COLint32 Sequence;

   COLint32 TestCount;
};

#endif     // end of defensive include
