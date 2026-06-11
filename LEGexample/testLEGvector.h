#ifndef __TEST_LEG_VECTOR_H__
#define __TEST_LEG_VECTOR_H__
//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2004 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testLEGvector.h
//
// Description:
//
// A set of utilities to test LEGvector objects.
// This is templated because there used to be multiple implementations of
// LEGvector to exercise.
//
// Copied from COLexample/testCOLarray.h.
//
// Author: Nasron Cheong
// Date:   Thu 02/05/2004
//
//
//---------------------------------------------------------------------------

#include <TST/TSThelpers.h>

#include <LEG/LEGvector.h>

#define MAX_ITEMS 1000

class TSTtestable;

template <class TVector>
class testLEGvector
{
public:
   typename TVector::TContained TestType;

   testLEGvector(): Sequence(0), SizeOfControlVector(0) {};
   virtual ~testLEGvector() {};

   static testLEGvector* create(){ return new testLEGvector; }

   void testCompare();

   void controlMove(const COLindex FromIndex, const COLindex ToIndex);

   void initTest();
      
   void resizeTest();

   void removeTest();

   void insertTest();

   void clearTest();

   void addTest();

   void stackTest();

private:
   testLEGvector(const testLEGvector& VectorTest);  
   testLEGvector& operator=(const testLEGvector& VectorTest);

   TVector TestVector;
   
   typename TVector::TContained  ControlVector[MAX_ITEMS];
   COLindex   SizeOfControlVector;
 
   COLint32 TestCount;

   typename TVector::TContained Sequence;
   
   COLint32 SizeOfVector;
};

template <class TVector>
void testLEGvector<TVector>::testCompare()
{
   TestCount++;
   TST_ASSERT_MESSAGE( TestVector.size() == SizeOfControlVector, TestCount << " test failed with an incorrect size");
   for (COLindex ItemIndex = 0; ItemIndex < SizeOfControlVector; ItemIndex++)
   {
      TST_ASSERT_MESSAGE( TestVector[ItemIndex] == ControlVector[ItemIndex],TestCount << " test failed with incorrect data at index " << ItemIndex );
   }
}

template <class TVector>
void testLEGvector<TVector>::controlMove(const COLindex FromIndex, const COLindex ToIndex)
{
   memmove(ControlVector + ToIndex, ControlVector + FromIndex, (SizeOfControlVector - FromIndex) * sizeof(TestType));
   SizeOfControlVector += ToIndex - FromIndex;
}

template <class TVector>
void testLEGvector<TVector>::initTest()
{
   testCompare();
}

template <class TVector>
void testLEGvector<TVector>::resizeTest()
{
   // Resize
   TestVector.resize(1);
   SizeOfControlVector = 1;
   Sequence++;
   TestVector[0] = Sequence;
   ControlVector[0] = Sequence;
   testCompare();

   TestVector.resize(0);
   SizeOfControlVector = 0;
   testCompare();

   for (SizeOfVector = 3; SizeOfVector <= 300; SizeOfVector += 3)
   {
      TestVector.resize(SizeOfVector);
      SizeOfControlVector = SizeOfVector;
      Sequence++;
      TestVector[SizeOfVector - 3] = Sequence;
      ControlVector[SizeOfVector - 3] = Sequence;
      Sequence++;
      TestVector[SizeOfVector - 2] = Sequence;
      ControlVector[SizeOfVector - 2] = Sequence;
      Sequence++;
      TestVector[SizeOfVector - 1] = Sequence;
      ControlVector[SizeOfVector - 1] = Sequence;
      testCompare();
   }

   for (SizeOfVector = 295; SizeOfVector >= 0; SizeOfVector -= 5)
   {
      TestVector.resize(SizeOfVector);
      SizeOfControlVector = SizeOfVector;
      testCompare();
   }

   TestVector.resize(MAX_ITEMS);
   SizeOfControlVector = MAX_ITEMS;
   for (SizeOfVector = 0; SizeOfVector < MAX_ITEMS; SizeOfVector++)
   {
      Sequence++;
      TestVector[SizeOfVector] = Sequence;
      ControlVector[SizeOfVector] = Sequence;
   }
   testCompare();
}

template <class TVector>
void testLEGvector<TVector>::removeTest()
{
   // Remove

   TestVector.remove(0);
   controlMove(1, 0);
   testCompare();

   TestVector.remove(1);
   controlMove(2, 1);
   testCompare();

   SizeOfVector = TestVector.size() / 2;
   TestVector.remove(SizeOfVector);
   controlMove(SizeOfVector + 1, SizeOfVector);
   testCompare();

   TestVector.remove(TestVector.size() - 2);
   controlMove(SizeOfControlVector - 1, SizeOfControlVector - 2);
   testCompare();

   TestVector.remove(TestVector.size() - 1);
   SizeOfControlVector--;
   testCompare();
}

template <class TVector>
void testLEGvector<TVector>::insertTest()
{
   // Insert

   Sequence++;
   TestVector.insert(Sequence, 0);
   controlMove(0, 1);
   ControlVector[0] = Sequence;
   testCompare();

   Sequence++;
   TestVector.insert(Sequence, 1);
   controlMove(1, 2);
   ControlVector[1] = Sequence;
   testCompare();

   SizeOfVector = TestVector.size() / 2;
   Sequence++;
   TestVector.insert(Sequence, SizeOfVector);
   controlMove(SizeOfVector, SizeOfVector + 1);
   ControlVector[SizeOfVector] = Sequence;
   testCompare();

   Sequence++;
   TestVector.insert(Sequence, TestVector.size() - 1);
   controlMove(SizeOfControlVector - 1, SizeOfControlVector);
   ControlVector[SizeOfControlVector - 2] = Sequence;
   testCompare();

   Sequence++;
   TestVector.insert(Sequence, TestVector.size());
   SizeOfControlVector++;
   ControlVector[SizeOfControlVector - 1] = Sequence;
   testCompare();
}

template <class TVector>
void testLEGvector<TVector>::clearTest()
{
   // Clear

   TestVector.fullClear();
   SizeOfControlVector = 0;
   testCompare();

}

template <class TVector>
void testLEGvector<TVector>::addTest()
{
   // Add

   for (SizeOfVector = 0; SizeOfVector < 300; SizeOfVector++)
   {
      Sequence++;
      TestVector.add(Sequence);
      SizeOfControlVector++;
      ControlVector[SizeOfControlVector - 1] = Sequence;
      testCompare();
   }
}

#define COL_TESTS\
   TST_ADD_TEST_METHOD(initTest);\
   TST_ADD_TEST_METHOD(resizeTest);\
   TST_ADD_TEST_METHOD(removeTest);\
   TST_ADD_TEST_METHOD(insertTest);\
   TST_ADD_TEST_METHOD(clearTest);\
   TST_ADD_TEST_METHOD(addTest);

static TSTtestable* LEGvectorSelectTests()
{
   typedef testLEGvector< LEGvector<COLint32> > testLEGvectorSelect;
   TST_START_FIXTURE(testLEGvectorSelect)
      COL_TESTS
   TST_END_FIXTURE
}

static TSTtestable* LEGvectorSmallTests()
{
   typedef testLEGvector< LEGvector<COLint16> > testLEGvectorSmall;
   TST_START_FIXTURE(testLEGvectorSmall)
      COL_TESTS
   TST_END_FIXTURE
}

static TSTtestable* LEGvectorLargeTests()
{
   typedef testLEGvector< LEGvector<COLint64> > testLEGvectorLarge;
   TST_START_FIXTURE(testLEGvectorLarge)
      COL_TESTS
   TST_END_FIXTURE
}

class InteriorPointing
{
public:
   InteriorPointing(){
      m_pMyself = this;
      WasCopyCtorCalled = false;
   }
   InteriorPointing(const InteriorPointing& Orig){
      m_pMyself = this;
      WasCopyCtorCalled = true;
   }
   bool WasCopyCtorCalled;
   const InteriorPointing* m_pMyself;
private:
   InteriorPointing& operator=(const InteriorPointing& Orig);
};

// This test ensures that we actually call the copy constructor
// for the class.
static void testInteriorPointingClass(){
   LEGvector<InteriorPointing> Vect;
   Vect.push_back();
   TST_ASSERT_EQUALS(&Vect[0], Vect[0].m_pMyself);
   TST_ASSERT_EQUALS(false, Vect[0].WasCopyCtorCalled);

   // Set capacity to something that will undoubtedly trigger
   // moving the vector items in memory.
   // We assert the opposite of what COLarray's behaviour would
   // be, since LEGvector is here to maintain the old "memory sliding"
   // (see LEGvector.h).
   Vect.changeCapacity(2048);
   TST_ASSERT(&Vect[0] != Vect[0].m_pMyself);
   TST_ASSERT_EQUALS(false, Vect[0].WasCopyCtorCalled);
}

#ifdef LEGvector_MEMSET_DESTRUCTED
static void testLEGvector_MEMSET_DESTRUCTED(){
   int seven = 7;
   LEGvector<int*> pvec;
   TST_ASSERT_EQUALS(0, pvec.size());
   pvec.push_back(&seven);
   TST_ASSERT_EQUALS(1, pvec.size());
   int** ppSeven = pvec.begin();
   TST_ASSERT_EQUALS(7, *pvec[0]);
   TST_ASSERT_EQUALS(7, **ppSeven);
   pvec.pop_back();
   TST_ASSERT_EQUALS(0, pvec.size());
   TST_ASSERT(&seven != *ppSeven);
}
#endif

// valgrind memory leak test of LEGvector of COLauto.
void testCOLownerPtrVector()
{
   LEGvector< COLauto<COLstring> > VectorOfHeapCOLstrings;

   VectorOfHeapCOLstrings.add(new COLstring("Some test string longer than 16 characters."));
   VectorOfHeapCOLstrings.add(new COLstring("The sky is blue."));
   VectorOfHeapCOLstrings.add(new COLstring("pear"));
   VectorOfHeapCOLstrings.add(new COLstring("Little Dog Monday"));

   TST_ASSERT_MESSAGE(VectorOfHeapCOLstrings.size() == 4,"VectorOfHeapCOLstrings size failed");

   VectorOfHeapCOLstrings.remove(2);

   TST_ASSERT_MESSAGE(VectorOfHeapCOLstrings.size() == 3,"VectorOfHeapCOLstrings size failed");

   VectorOfHeapCOLstrings.add(new COLstring("Another string."));
   VectorOfHeapCOLstrings.add(new COLstring("Yet another string."));

   TST_ASSERT_MESSAGE(VectorOfHeapCOLstrings.size() == 5,"VectorOfHeapCOLstrings size failed");

   VectorOfHeapCOLstrings.remove(0);
   VectorOfHeapCOLstrings.remove(0);

   TST_ASSERT_MESSAGE(VectorOfHeapCOLstrings.size() == 3,"VectorOfHeapCOLstrings size failed");

   // Cause a resize, and let valgrind see if anything bad happens.
   VectorOfHeapCOLstrings.changeCapacity(2048);
}

static TSTtestable* testMemoryManagement(){
   TST_START_COLLECTION("testMemoryManagement");
      TST_ADD_TEST_FUNCTION(testInteriorPointingClass);
      TST_ADD_TEST_FUNCTION(testCOLownerPtrVector);
#ifdef LEGvector_MEMSET_DESTRUCTED
      TST_ADD_TEST_FUNCTION(testLEGvector_MEMSET_DESTRUCTED);
#endif
   TST_END_FIXTURE
}

static TSTtestable* LEGvectorTests()
{
   TSTtestCollection* pCollection = new TSTtestCollection("testLEGvector");
   pCollection->addTest(LEGvectorSelectTests());
   pCollection->addTest(LEGvectorSmallTests());
   pCollection->addTest(LEGvectorLargeTests());
   pCollection->addTest(testMemoryManagement());
   return pCollection;
}

#endif     // end of defensive include
