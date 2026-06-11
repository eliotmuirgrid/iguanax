//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testCOLarray.cpp
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   9 May 2013
//
//
//---------------------------------------------------------------------------

#include "COLexamplePrecomp.h"
#pragma hdrstop


#define MAX_ITEMS 1000

class TSTtestable;

template <class TVector>
class testCOLarray
{
public:
   typename TVector::TContained TestType;

   testCOLarray(): Sequence(0), SizeOfControlVector(0) {};
   virtual ~testCOLarray() {};

   static testCOLarray* create(){ return new testCOLarray; }

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
   testCOLarray(const testCOLarray& VectorTest);  
   testCOLarray& operator=(const testCOLarray& VectorTest);

   TVector TestVector;
   
   typename TVector::TContained  ControlVector[MAX_ITEMS];
   COLindex   SizeOfControlVector;
 
   COLint32 TestCount;

   typename TVector::TContained Sequence;
   
   COLint32 SizeOfVector;
};

template <class TVector>
void testCOLarray<TVector>::testCompare()
{
   TestCount++;
   TST_ASSERT_MESSAGE( TestVector.size() == SizeOfControlVector, TestCount << " test failed with an incorrect size");
   for (COLindex ItemIndex = 0; ItemIndex < SizeOfControlVector; ItemIndex++)
   {
      TST_ASSERT_MESSAGE( TestVector[ItemIndex] == ControlVector[ItemIndex],TestCount << " test failed with incorrect data at index " << ItemIndex );
   }
}

template <class TVector>
void testCOLarray<TVector>::controlMove(const COLindex FromIndex, const COLindex ToIndex)
{
   memmove(ControlVector + ToIndex, ControlVector + FromIndex, (SizeOfControlVector - FromIndex) * sizeof(TestType));
   SizeOfControlVector += ToIndex - FromIndex;
}

template <class TVector>
void testCOLarray<TVector>::initTest()
{
   testCompare();
}

template <class TVector>
void testCOLarray<TVector>::resizeTest()
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
void testCOLarray<TVector>::removeTest()
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
void testCOLarray<TVector>::insertTest()
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
void testCOLarray<TVector>::clearTest()
{
   // Clear

   TestVector.clear();
   SizeOfControlVector = 0;
   testCompare();

}

template <class TVector>
void testCOLarray<TVector>::addTest()
{
   // Add

   for (SizeOfVector = 0; SizeOfVector < 300; SizeOfVector++)
   {
      Sequence++;
      TestVector.push_back(Sequence);
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

static TSTtestable* COLarraySelectTests()
{
   typedef testCOLarray< COLarray<COLint32> > testCOLarraySelect;
   TST_START_FIXTURE(testCOLarraySelect)
      COL_TESTS
   TST_END_FIXTURE
}

static TSTtestable* COLarraySmallTests()
{
   typedef testCOLarray< COLarray<COLint16> > testCOLarraySmall;
   TST_START_FIXTURE(testCOLarraySmall)
      COL_TESTS
   TST_END_FIXTURE
}

static TSTtestable* COLarrayLargeTests()
{
   typedef testCOLarray< COLarray<COLint64> > testCOLarrayLarge;
   TST_START_FIXTURE(testCOLarrayLarge)
      COL_TESTS
   TST_END_FIXTURE
}

// Basic testing for COLarray<COLuint8>.
// Note: COLuint8 == char.
static void testCOLuint8(){
   COLarray<COLuint8> CharVector;
   TST_ASSERT_EQUALS(0, CharVector.size());
   
   CharVector.insert('a', 0);
   TST_ASSERT_EQUALS(1, CharVector.size());
   CharVector.push_back('b');
   TST_ASSERT_EQUALS(2, CharVector.size());
   CharVector.insert('d', 2);
   TST_ASSERT_EQUALS(3, CharVector.size());
   CharVector.insert('c', 2);
   TST_ASSERT_EQUALS(4, CharVector.size());

   TST_ASSERT_EQUALS('a', CharVector[0]);
   TST_ASSERT_EQUALS('b', CharVector[1]);
   TST_ASSERT_EQUALS('c', CharVector[2]);
   TST_ASSERT_EQUALS('d', CharVector[3]);

   {
      COLuint8& CharRef = CharVector.push_back();
      CharRef = 'e';
   }
   TST_ASSERT_EQUALS(5, CharVector.size());
   TST_ASSERT_EQUALS('e', CharVector[4]);

   CharVector.remove(4);
   TST_ASSERT_EQUALS(4, CharVector.size());
   TST_ASSERT_EQUALS('d', CharVector[3]);

   CharVector.remove(0);
   TST_ASSERT_EQUALS(3, CharVector.size());
   TST_ASSERT_EQUALS('b', CharVector[0]);

   CharVector.remove(1);
   TST_ASSERT_EQUALS('d', CharVector[1]);

   CharVector.clear();
   TST_ASSERT_EQUALS(0, CharVector.size());
}

// Basic testing 
static void testPointer(){
   COLarray<char*> CharStarVector;
   TST_ASSERT_EQUALS(0, CharStarVector.size());

   CharStarVector.insert((char*)"a", 0);
   TST_ASSERT_EQUALS(1, CharStarVector.size());
   CharStarVector.push_back((char*)"b");
   TST_ASSERT_EQUALS(2, CharStarVector.size());
   CharStarVector.insert((char*)"d", 2);
   TST_ASSERT_EQUALS(3, CharStarVector.size());
   CharStarVector.insert((char*)"c", 2);
   TST_ASSERT_EQUALS(4, CharStarVector.size());

   TST_ASSERT_EQUALS(COLstring("a"), CharStarVector[0]);
   TST_ASSERT_EQUALS(COLstring("b"), CharStarVector[1]);
   TST_ASSERT_EQUALS(COLstring("c"), CharStarVector[2]);
   TST_ASSERT_EQUALS(COLstring("d"), CharStarVector[3]);

   {
      char*& pChar = CharStarVector.push_back();
      pChar = (char*)"e";
   }
   TST_ASSERT_EQUALS(5, CharStarVector.size());
   TST_ASSERT_EQUALS(COLstring("e"), CharStarVector[4]);

   CharStarVector.remove(4);
   TST_ASSERT_EQUALS(4, CharStarVector.size());
   TST_ASSERT_EQUALS(COLstring("d"), CharStarVector[3]);

   CharStarVector.remove(0);
   TST_ASSERT_EQUALS(3, CharStarVector.size());
   TST_ASSERT_EQUALS(COLstring("b"), CharStarVector[0]);

   CharStarVector.remove(1);
   TST_ASSERT_EQUALS(COLstring("d"), CharStarVector[1]);

   CharStarVector.clear();
   TST_ASSERT_EQUALS(0, CharStarVector.size());
}

static TSTtestable* testSimpleTypes(){
   TST_START_COLLECTION("testSimpleTypes");
      TST_ADD_TEST_FUNCTION(testCOLuint8);
      TST_ADD_TEST_FUNCTION(testPointer);
   TST_END_FIXTURE
}

static void testAssignment(){
   COLarray<COLstring> OrigVect;
   OrigVect.push_back("It");
   OrigVect.push_back("was");
   OrigVect.push_back("a");

   COLarray<COLstring> NewVect = OrigVect;
   TST_ASSERT_EQUALS(3, NewVect.size());
   TST_ASSERT_EQUALS("It", NewVect[0]);
   TST_ASSERT_EQUALS("was", NewVect[1]);
   TST_ASSERT_EQUALS("a", NewVect[2]);

   COLarray<COLstring> EmptyVect;
   NewVect = EmptyVect;
   TST_ASSERT_EQUALS(0, NewVect.size());
}

static void testCapacity(){
   // Initial capacity.
   COLarray<COLstring> Vect;
   TST_ASSERT_EQUALS(0, Vect.capacity());
   
   // Explicitly setting to 0.
   Vect.setCapacity(0);
   TST_ASSERT_EQUALS(0, Vect.capacity());

   // Adding something to force capacity increase.
   Vect.push_back("It");
   TST_ASSERT(Vect.capacity() > 0);

   // Setting capacity lower than current size.
   Vect.setCapacity(0);
   TST_ASSERT(Vect.capacity() > 0);

   // Setting capacity to something large.
   Vect.setCapacity(2048);
   TST_ASSERT_EQUALS(2048, Vect.capacity());
}

static void testIterate(){
   COLarray<COLstring> Vect;

   // iterate empty vector
   for (COLstring* pString = Vect.begin(); pString != Vect.end(); pString++){
      COL_ERROR_STREAM("Empty vector's begin() allowed iteration", COLerror::PreCondition);
   }

   Vect.push_back("It");
   Vect.push_back("was");
   Vect.push_back("a");
   
   // iterator
   {
      int StringIndex = 0;
      for (COLstring* pString = Vect.begin(); pString != Vect.end(); pString++){
         TST_ASSERT_EQUALS(Vect[StringIndex++], *pString);
      }
   }

   // const_iterator
   {
      int StringIndex = 0;
      for (const COLstring* pString = Vect.begin(); pString != Vect.end(); pString++){
         TST_ASSERT_EQUALS(Vect[StringIndex++], *pString);
      }
   }
}

static void testPushBack(){
   COLarray<COLstring> Vect;

   // push_back with no param.
   COLstring& Pushed = Vect.push_back();
   Pushed = "It";
   TST_ASSERT_EQUALS("It", Vect[0]);

   // push_back with param.
   COLstring NextPushed = Vect.push_back("was");
   TST_ASSERT_EQUALS("was", Vect[1]);
   TST_ASSERT_EQUALS("was", NextPushed);
}

static void testPopBack(){
   COLarray<COLstring> Vect;
   Vect.push_back("It");
   Vect.push_back("was");
   Vect.push_back("a");

   Vect.pop_back();
   Vect.pop_back();
   Vect.pop_back();
   TST_ASSERT_EQUALS(0, Vect.size());

   Vect.pop_back(); // shouldn't throw
}

static void testClear(){
   COLarray<COLstring> Vect;

   Vect.clear();
   TST_ASSERT_EQUALS(0, Vect.size());

   Vect.push_back("It");
   Vect.push_back("was");
   Vect.push_back("a");

   Vect.clear();
   TST_ASSERT_EQUALS(0, Vect.size());
}

static void testErase(){
   COLarray<COLstring> Vect;
   Vect.push_back("It");
   Vect.push_back("was");
   Vect.push_back("a");

   int StringIndex = 0;
   for (COLstring* pString = Vect.begin(); pString != Vect.end();){
      pString = Vect.erase(pString);
      // don't increment pString after a call to erase().
   }

   TST_ASSERT_EQUALS(0, Vect.size());

   // Erase NULL
   Vect.erase(NULL); // no effect
}

static void testRemove(){
   COLarray<COLstring> Vect;
   Vect.push_back("It");
   Vect.push_back("was");
   Vect.push_back("a");

   Vect.remove(0);
   TST_ASSERT_EQUALS("was", Vect[0]);

   Vect.remove(1);
   TST_ASSERT_EQUALS("was", Vect[0]);

   Vect.remove(0);
   TST_ASSERT_EQUALS(0, Vect.size());
}

static void testResize(){
   COLarray<COLstring> Vect;
   Vect.resize(4);
   TST_ASSERT_EQUALS(4, Vect.size());
   TST_ASSERT(Vect.capacity() >= 4);

   Vect[0] = "It";

   // Make sure "It" is still there after a resize.
   Vect.resize(8);
   TST_ASSERT_EQUALS("It", Vect[0]);

   // Make sure "It" is still there after a resize.
   Vect.resize(1);
   TST_ASSERT_EQUALS("It", Vect[0]);

   Vect.resize(0);
   TST_ASSERT_EQUALS(0, Vect.size());

   // Make sure "It" is no longer there.
   Vect.resize(1);
   TST_ASSERT_EQUALS("", Vect[0]);
}

static void testInsertOneParam(){
   COLarray<COLstring> Vect;

   // This causes a crash now.
   // Insert out of bounds on empty vector.
   //try {
   //   Vect.insert(1);
   //} catch (const COLerror& Error){
      //TST_ASSERT_EQUALS(COLerror::PreCondition, Error.code());
   //}

   // Insert at valid index on empty vector.
   Vect.insert(0);
   Vect[0] = "It";
   
   // Insert at index 0.
   Vect.insert(0);
   TST_ASSERT_EQUALS("", Vect[0]);
   TST_ASSERT_EQUALS("It", Vect[1]);

   // Insert at end.
   Vect.insert(2);
   TST_ASSERT_EQUALS("", Vect[2]);

   // Insert out of bounds on non-empty vector.
   //try {
   //   Vect.insert(4);
   //} catch (const COLerror& Error){
   //   TST_ASSERT_EQUALS(COLerror::PreCondition, Error.code());
  // }

   // Insert in middle.
   Vect[1] = "was";
   Vect.insert(1);
   TST_ASSERT_EQUALS("", Vect[1]);
}

static void testInsertTwoParam(){
   COLarray<COLstring> Vect;

   // Insert out of bounds on empty vector.
   //try {
   //   Vect.insert("It", 1);
   //} catch (const COLerror& Error){
   //   TST_ASSERT_EQUALS(COLerror::PreCondition, Error.code());
   //}

   // Insert at valid index on empty vector.
   Vect.insert("It", 0);

   // Insert at index 0.
   Vect.insert("was", 0);
   TST_ASSERT_EQUALS("was", Vect[0]);
   TST_ASSERT_EQUALS("It", Vect[1]);

   // Insert at end.
   Vect.insert("a", 2);
   TST_ASSERT_EQUALS("a", Vect[2]);

   // Insert out of bounds on non-empty vector.
   //try {
   //   Vect.insert("dark", 4);
   //} catch (const COLerror& Error){
   //   TST_ASSERT_EQUALS(COLerror::PreCondition, Error.code());
  // }

   // Insert in middle.
   Vect.insert("dark", 1);
   TST_ASSERT_EQUALS("dark", Vect[1]);
}

static void testBack(){
   COLarray<COLstring> Vect;

   //try {
   //   Vect.back();
   //} catch (const COLerror& Error){
   //   TST_ASSERT_EQUALS(COLerror::PreCondition, Error.code());
   //}

   Vect.push_back("It");
   TST_ASSERT_EQUALS("It", Vect.back());

   Vect.push_back("was");
   TST_ASSERT_EQUALS("was", Vect.back());
}

static void testFront(){
   COLarray<COLstring> Vect;

   //try {
   //   Vect.front();
   //} catch (const COLerror& Error){
   //   TST_ASSERT_EQUALS(COLerror::PreCondition, Error.code());
   //}

   Vect.push_back("It");
   TST_ASSERT_EQUALS("It", Vect.front());

   Vect.push_back("was");
   TST_ASSERT_EQUALS("It", Vect.front());
}

static void testGetArray(){
   COLarray<COLstring> Vect;

   TST_ASSERT_EQUALS(0, Vect.getArray());

   Vect.push_back("It");
   TST_ASSERT_EQUALS("It", *Vect.getArray());

   Vect.push_back("was");
   TST_ASSERT_EQUALS("It", Vect.getArray()[0]);
   TST_ASSERT_EQUALS("was", Vect.getArray()[1]);
}

static void testIndexOf(){
   COLarray<COLstring> Vect;

   // Empty vector.
   TST_ASSERT_EQUALS(-1, Vect.indexOf("It"));

   Vect.push_back("It");

   // One item in the vector, matches search.
   TST_ASSERT_EQUALS(0, Vect.indexOf("It"));

   // One item in the vector, doesn't match search.
   TST_ASSERT_EQUALS(-1, Vect.indexOf("was"));

   Vect.push_back("was");
   Vect.push_back("a");
   Vect.push_back("dark");
   
   // Item at end.
   TST_ASSERT_EQUALS(3, Vect.indexOf("dark"));

   // Item in middle.
   TST_ASSERT_EQUALS(2, Vect.indexOf("a"));
}

static TSTtestable* testCOLstring(){
   TST_START_COLLECTION("testCOLstring");
      TST_ADD_TEST_FUNCTION(testAssignment);
      TST_ADD_TEST_FUNCTION(testCapacity);
      TST_ADD_TEST_FUNCTION(testIterate);
      TST_ADD_TEST_FUNCTION(testPushBack);
      TST_ADD_TEST_FUNCTION(testPopBack);
      TST_ADD_TEST_FUNCTION(testClear);
      TST_ADD_TEST_FUNCTION(testErase);
      TST_ADD_TEST_FUNCTION(testRemove);
      TST_ADD_TEST_FUNCTION(testResize);
      TST_ADD_TEST_FUNCTION(testInsertOneParam);
      TST_ADD_TEST_FUNCTION(testInsertTwoParam);
      TST_ADD_TEST_FUNCTION(testBack);
      TST_ADD_TEST_FUNCTION(testFront);
      TST_ADD_TEST_FUNCTION(testGetArray);
      TST_ADD_TEST_FUNCTION(testIndexOf);
   TST_END_FIXTURE
}

// This class contains the minimal interface in order to
// allow usage in COLarray; namely a no-param constructor
// and a copy constructor.
// It has an interior-pointing member, to illustrate the
// importance of calling copy constructors rather than
// simply moving the the object in memory.
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
   COLarray<InteriorPointing> Vect;
   Vect.push_back();
   TST_ASSERT_EQUALS(&Vect[0], Vect[0].m_pMyself);
   TST_ASSERT_EQUALS(false, Vect[0].WasCopyCtorCalled);

   // Set capacity to something that will undoubtedly trigger
   // moving the vector items in memory.
   Vect.setCapacity(2048);
   TST_ASSERT_EQUALS(&Vect[0], Vect[0].m_pMyself);
   TST_ASSERT_EQUALS(true, Vect[0].WasCopyCtorCalled);
}

#ifdef COLarray_MEMSET_DESTRUCTED
static void testCOLarray_MEMSET_DESTRUCTED(){
   int seven = 7;
   COLarray<int*> pvec;
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

static TSTtestable* testMemoryManagement(){
   TST_START_COLLECTION("testMemoryManagement");
      TST_ADD_TEST_FUNCTION(testInteriorPointingClass);
#ifdef COLarray_MEMSET_DESTRUCTED
      TST_ADD_TEST_FUNCTION(testCOLarray_MEMSET_DESTRUCTED);
#endif
   TST_END_FIXTURE
}

TSTtestable* COLarrayTests()
{
   TSTtestCollection* pCollection = new TSTtestCollection("testCOLarray");
      pCollection->addTest(COLarraySelectTests());
      pCollection->addTest(COLarraySmallTests());
      pCollection->addTest(COLarrayLargeTests());
      pCollection->addTest(testSimpleTypes());
      pCollection->addTest(testCOLstring());
      pCollection->addTest(testMemoryManagement());
   return pCollection;
}
