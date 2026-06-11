#ifndef __TEST_COLMAP_H__
#define __TEST_COLMAP_H__
//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testCOLmap.h
//
// Description:
//
// A set of utilities to test COLmap objects.
//
// Author: Ian Farquharson
// Date:   Thu 02/05/2004
//
//
//---------------------------------------------------------------------------

class COLdataType;

class TSTtestable;

class testCOLmap
{
public:
   typedef COLint16 TestType;

   typedef COLlist<TestType>                 TestListType;
   typedef COLmap<TestType, TestType> TestSortedType;

   testCOLmap();
   virtual ~testCOLmap(){};

   static testCOLmap* create(){ return new testCOLmap; }

   void initTest();
   void addTest();
   void stableAddTest();
   void addUniqueTest();
   void removeTest();
   void removeKeyTest();
   void clearTest();
   void walkingTest();
   void findTest();
   void iterateAndDeleteTest();
   void randomTest();

   static TSTtestable* COLmapTests();

private:
   testCOLmap(const testCOLmap& TestSortedList);
   testCOLmap& operator=(const testCOLmap& TestSortedList);

   void testCompare();
   void testCheck(const bool Condition);
   COLlistPlace add(TestListType& Control, const TestType Item);
   void remove(TestListType& Control, const TestType Item);

   TestListType    Control;
   TestSortedType  TestSorted;

   COLint32  TestCount;
   COLint32  Sequence;
};

#endif     // end of defensive include
