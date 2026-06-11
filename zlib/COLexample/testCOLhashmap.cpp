//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testCOLhashmap.cpp
//
// Description:
//
// Implementation
//
// Author: Ian Farquharson
// Date:   Thu 02/05/2004
//
//
//---------------------------------------------------------------------------

#include "COLexamplePrecomp.h"
#pragma hdrstop

#include "testCOLhashmap.h"
#include <COL/COLhash.h>

#define MAX_ITEMS 100

static COLindex Hash(const testCOLhashmap::TestType& Value)
{
   return Value;
}

testCOLhashmap::testCOLhashmap()
   : TestCount(0), TestLookup(Hash), Sequence(0)
{
};

void testCOLhashmap::testCheck(const bool Condition)
{
   TestCount++;
   TST_ASSERT_MESSAGE(Condition, TestCount << " test failed" );
}

void testCOLhashmap::testCompare()
{
   TestCount++;

   TST_ASSERT_MESSAGE(TestLookup.size() == Control.size(),TestCount << " test failed with an incorrect size");
   for (COLlistPlace Place = Control.first(); Place != NULL; Place = Control.next(Place))
   {
     COLhashmapPlace LookupListPlace = TestLookup.find(Control[Place]);
     TST_ASSERT_MESSAGE(LookupListPlace != NULL && TestLookup[LookupListPlace] == Control[Place],TestCount << " test failed with an different contents");
   }
}

COLlistPlace testCOLhashmap::find(const TestListType& List, const TestType Item)
{
   for (COLlistPlace Place = List.first(); Place != NULL; Place = List.next(Place))
   {
      if (List[Place] == Item)
      {
         return Place;
      }
   }
   TST_ASSERT_MESSAGE(1 == 0,TestCount << " test failed");
   return NULL;
}

void testCOLhashmap::remove(TestListType& List, const TestType Item)
{
   for (COLlistPlace Place = List.first(); Place != NULL; Place = List.next(Place))
   {
      if (List[Place] == Item)
      {
         List.remove(Place);
         break;
      }
   }
}

void testCOLhashmap::initTest()
{
   TestType Sequence = 0;
   testCompare();
}

void testCOLhashmap::addTest()
{
   // Add

   for (COLindex ListIndex = 0; ListIndex < MAX_ITEMS; ListIndex++)
   {
      Sequence += 3;
      TestLookup.add(Sequence, Sequence);
      Control.add(Sequence);
      testCompare();
   }
}

void testCOLhashmap::removeTest()
{
   // remove
   COLindex ListIndex = 0;
   for (ListIndex = 0; ListIndex < 20; ListIndex++)
   {
      remove(Control, TestLookup[TestLookup.first()]);
      TestLookup.remove(TestLookup.first());
      testCompare();
   }

   for (ListIndex = 0; ListIndex < 20; ListIndex++)
   {
      remove(Control, TestLookup[TestLookup.last()]);
      TestLookup.remove(TestLookup.last());
      testCompare();
   }

   for (ListIndex = 0; ListIndex < 20; ListIndex++)
   {
      remove(Control, TestLookup[TestLookup.next(TestLookup.first())]);
      TestLookup.remove(TestLookup.next(TestLookup.first()));
      testCompare();
   }

   for (ListIndex = 0; ListIndex < 20; ListIndex++)
   {
      remove(Control, TestLookup[TestLookup.previous(TestLookup.last())]);
      TestLookup.remove(TestLookup.previous(TestLookup.last()));
      testCompare();
   }

   for (ListIndex = 0; ListIndex < 20; ListIndex++)
   {
      TestLookup.removeKey(Control[Control.first()]);
      Control.remove(Control.first());
      testCompare();
   }

   testCheck(!TestLookup.removeKey(0));
   testCheck(TestLookup.size() == 0);
}

void testCOLhashmap::clearTest()
{
   // Clear

   for (COLindex ListIndex = 0; ListIndex < MAX_ITEMS; ListIndex++)
   {
      Sequence += 3;
      TestLookup.add(Sequence, Sequence);
      Control.add(Sequence);
      testCompare();
   }

   TestLookup.clear();
   Control.clear();
   testCompare();
   testCheck(TestLookup.size() == 0);
}

void testCOLhashmap::iterationTest()
{
   // Iteration

   COLindex ListIndex = 0;

   for (ListIndex = 0; ListIndex < MAX_ITEMS; ListIndex++)
   {
      Sequence += 3;
      TestLookup.add(Sequence, Sequence);
      Control.add(Sequence);
      testCompare();
   }

   for (ListIndex = 0; ListIndex < MAX_ITEMS; ListIndex++)
   {
      TestLookup.removeKey(Control[Control.first()]);
      testCheck(TestLookup.find(Control[Control.first()]) == NULL);
      Control.remove(Control.first());
      testCompare();

      if (ListIndex % 100 == 0)
      {
         COLindex LookupListIndex = 0;
         for (COLhashmapPlace LookupListPlace = TestLookup.first(); LookupListPlace != NULL; LookupListPlace = TestLookup.next(LookupListPlace))
         {
            if (TestLookup.next(LookupListPlace) == NULL)
            {
               testCheck(LookupListPlace == TestLookup.last());
            }
            else
            {
               testCheck(LookupListPlace == TestLookup.previous(TestLookup.next(LookupListPlace)));
            }
            COLlistPlace ListPlace = find(Control, TestLookup[LookupListPlace]);
            testCheck(ListPlace != NULL && TestLookup[LookupListPlace] == Control[ListPlace]);
            LookupListIndex++;
         }
         testCheck(LookupListIndex == (COLuint32) TestLookup.size());
         testCheck(TestLookup.next(NULL) == TestLookup.first());
         testCheck(TestLookup.previous(NULL) == TestLookup.last());
      }
   }
   testCheck(TestLookup.next(NULL) == TestLookup.first());
   testCheck(TestLookup.previous(NULL) == TestLookup.last());
   testCheck(TestLookup.size() == 0);
}

void testCOLhashmap::randomTest()
{
   COLuint32 Counter;

   for (COLindex ListIndex = 0; ListIndex < MAX_ITEMS; ListIndex++)
   {
      for (Counter = rand() % 55 + 1; Counter > 0 && TestLookup.size() < 1000; Counter--)
      {
         TestType Sequence = rand();
         if (TestLookup.add(Sequence, Sequence) != NULL)
         {
            Control.add(Sequence);
         }
      }
      for (Counter = rand() % 50 + 1; Counter > 0 && TestLookup.size() > 0; Counter--)
      {
         COLhashmapPlace Place = TestLookup.first();
         for (COLindex ListIndex2 = rand() % TestLookup.size(); ListIndex2 > 0; ListIndex2--) // interate one less than size() times
         {
            Place = TestLookup.next(Place);
         }
         remove(Control, TestLookup[Place]);
         if (Counter % 2 == 0)
         {
            TestLookup.remove(Place);
         }
         else
         {
            TestLookup.removeKey(TestLookup[Place]);
         }
      }
      testCompare();
   }
}

void testCOLhashmap::mapTest()
{
   TestLookup.clear();
   Control.clear();
   testCompare();
   testCheck(TestLookup.size() == 0);

   TestLookup[1] = 201;
   TestLookup[2] = 202;
   TestLookup[3] = 203;
   testCheck(TestLookup[1] == 201);
   testCheck(TestLookup[2] == 202);
   testCheck(TestLookup[3] == 203);
   testCheck(TestLookup.size() == 3);
   TestLookup[1] = 301;
   TestLookup[2] = 302;
   TestLookup[3] = 303;
   testCheck(TestLookup[1] == 301);
   testCheck(TestLookup[2] == 302);
   testCheck(TestLookup[3] == 303);
   testCheck(TestLookup.size() == 3);
}

void testCOLhashmap::defaultHashTest()
{
   //String hash test
   COLhashmap<COLstring, COLstring> TestTable(COLhash<COLstring>::defaultHash);
   //COLhashmap<COLstring, COLstring> TestTable; // (COLhash<COLstring>::defaultHash);
   //COLhashmap<COLstring*, COLstring> TestTable2; // (COLhash<COLstring>::defaultHash);


   TestTable.add("KeyTest1", "ValueTest1");
   COLhashmapPlace Place = TestTable.find("KeyTest1");
   TST_ASSERT(Place != 0);
   TST_ASSERT(TestTable[Place] == "ValueTest1");

   {
      //COLuint32 hash test
      COLuint32 Key = 123456789;
      COLhashmap<COLuint32, COLstring> TestTable(COLhash<COLuint32>::defaultHash);
      TestTable.add(Key, "ValueTest1");
      Place = TestTable.find(Key);
      TST_ASSERT(Place != 0);
      TST_ASSERT(TestTable[Place] == "ValueTest1");
   }

   {
      //COLuint16 hash test
      COLuint16 Key1 = 65535;
      COLuint16 Key2 = 1234;
      COLhashmap<COLuint16, COLstring> TestTable(COLhash<COLuint16>::defaultHash);
      TestTable.add(Key1,"ValueTest1");
      TestTable.add(Key2,"ValueTest2");

      Place = TestTable.find(Key1);
      TST_ASSERT(Place != 0);
      TST_ASSERT(TestTable[Place] == "ValueTest1");

      Place = TestTable.find(Key2);
      TST_ASSERT(Place != 0);
      TST_ASSERT(TestTable[Place] == "ValueTest2");
   }

   {
      //COLuint8 hash test
      COLuint8 Key1 = 255;
      COLuint8 Key2 = 254;
      COLhashmap<COLuint8, COLstring>  TestTable(COLhash<COLuint8>::defaultHash);
      TestTable.add(Key1,"ValueTest1");
      TestTable.add(Key2,"ValueTest2");

      Place = TestTable.find(Key1);
      TST_ASSERT(Place != 0);
      TST_ASSERT(TestTable[Place] == "ValueTest1");

      Place = TestTable.find(Key2);
      TST_ASSERT(Place != 0);
      TST_ASSERT(TestTable[Place] == "ValueTest2");
   }


   {
      // COLuint64 hash test
     COLuint64 Key1 = 123456; //655123497861423535;
     COLuint64 Key2 = 123531; //2987234;

     //This does not work on aix
     // COLuint64 Key1 = 655123497861423535;
     // COLuint64 Key2 = 1235312987234;
      COLhashmap<COLuint64, COLstring> TestTable(COLhash<COLuint64>::defaultHash);
      TestTable.add(Key1,"ValueTest1");
      TestTable.add(Key2,"ValueTest2");

      Place = TestTable.find(Key1);
      TST_ASSERT(Place != 0);
      TST_ASSERT(TestTable[Place] == "ValueTest1");

      Place = TestTable.find(Key2);
      TST_ASSERT(Place != 0);
      TST_ASSERT(TestTable[Place] == "ValueTest2");
   }


}

void testCOLhashmap::iterateTest()
{
   COLhashmap<COLuint32,COLstring> Table(COLhash<COLuint32>::defaultHash);
   Table.add(1,"ONE");
   Table.add(2,"TWO");
   Table.add(3,"THREE");

   IteratorTestFuncReturn = 0;
   Table.iterate(&testCOLhashmap::iteratorTestFunc,this);
   TST_ASSERT(IteratorTestFuncReturn == 3);

   IteratorTestFuncReturn = 0;
   COLhashmapPlace First = Table.first();
   COLhashmapPlace Second = Table.next(First);
   Table.iterate(&testCOLhashmap::iteratorTestFunc,this,Second);
   TST_ASSERT(IteratorTestFuncReturn == 2);
   
   IteratorTestFuncReturn = 0;
   COLhashmapPlace Last = Table.last();
   Table.iterate(&testCOLhashmap::iteratorTestFunc,this,Last);
   TST_ASSERT(IteratorTestFuncReturn == 1);

   IteratorTestFuncReturn = 0;
   Table.iterate(&testCOLhashmap::iteratorTestFunc,this,Last,Last);
   TST_ASSERT(IteratorTestFuncReturn == 1);

   IteratorTestFuncReturn = 0;
   Table.iterate(&testCOLhashmap::iteratorTestFunc,this,Second,Second);
   TST_ASSERT(IteratorTestFuncReturn == 1);

   IteratorTestFuncReturn = 0;
   Table.iterate(&testCOLhashmap::iteratorTestFunc,this,First,Last);
   TST_ASSERT(IteratorTestFuncReturn == 3);

   IteratorTestFuncReturn = 0;
   Table.iterate(&testCOLhashmap::iteratorTestFunc,this,First,Second);
   TST_ASSERT(IteratorTestFuncReturn == 2);

   IteratorTestFuncReturn = 0;
   Table.iterate(&testCOLhashmap::iteratorTestFunc,this,First,First);
   TST_ASSERT(IteratorTestFuncReturn == 1);
}

void testCOLhashmap::iteratorTestFunc(const COLuint32& Key, COLstring& Value)
{
   if (Key == 1)
   {
      TST_ASSERT(Value == "ONE");
   }
   if (Key == 2)
   {
      TST_ASSERT(Value == "TWO");
   }
   if (Key == 3)
   {
      TST_ASSERT(Value == "THREE");
   }
   IteratorTestFuncReturn++;
}

struct testCOLhashmapFinder
{
   COLhashmapPlace find(const COLuint32& Key, COLstring& Value, COLhashmapPlace CurrentPlace)
   {
      if (Value == FindValue)
      {
         return CurrentPlace;
      }
      return NULL;
   }
   COLstring FindValue;
};

void testCOLhashmap::iterateFindTest()
{
   COLhashmap<COLuint32,COLstring> Table(COLhash<COLuint32>::defaultHash);
   Table.add(1,"ONE");
   Table.add(2,"TWO");
   Table.add(3,"THREE");

   testCOLhashmapFinder Finder;
   Finder.FindValue = "ONE";
   COLhashmapPlace Place = Table.iterate(&testCOLhashmapFinder::find,&Finder);
   TST_ASSERT(Place != NULL);
   TST_ASSERT(Table[Place] == "ONE");
   TST_ASSERT(Table.key(Place) == 1);

   Finder.FindValue = "DUMMY";
   Place = Table.iterate(&testCOLhashmapFinder::find,&Finder);
   TST_ASSERT(Place == NULL);

   Finder.FindValue = "TWO";
   Place = Table.iterate(&testCOLhashmapFinder::find,&Finder);
   TST_ASSERT(Place != NULL);
   TST_ASSERT(Table[Place] == "TWO");
   TST_ASSERT(Table.key(Place) == 2);
}

void testCOLhashmap::assignmentTest()
{
   COLhashmap<COLuint32,COLstring> Table(COLhash<COLuint32>::defaultHash);

   Table.add(1,"ONE");
   TST_ASSERT_EQUALS(1, Table.size());
   Table.add(1,"ONE");
   TST_ASSERT_EQUALS(1, Table.size());
   Table[2] = "TWO";
   TST_ASSERT_EQUALS(2, Table.size());
   Table[2] = "TWO";
   TST_ASSERT_EQUALS(2, Table.size());
   Table.add(2,"TWO");
   TST_ASSERT_EQUALS(2, Table.size());
   Table.add(1,"ONE");
   TST_ASSERT_EQUALS(2, Table.size());
   Table.add(2,"TWO");
   TST_ASSERT_EQUALS(2, Table.size());
   Table.add(2,"TWO");
   TST_ASSERT_EQUALS(2, Table.size());
   Table.add(3,"THREE");
   TST_ASSERT_EQUALS(3, Table.size());
   Table.add(3,"THREE");
   TST_ASSERT_EQUALS(3, Table.size());
   Table.add(3,"THREE");
   TST_ASSERT_EQUALS(3, Table.size());
   Table.add(2,"TWO");
   TST_ASSERT_EQUALS(3, Table.size());

   COLhashmap<COLuint32,COLstring> NewTable;
   TST_ASSERT_EQUALS(0, NewTable.size());
   NewTable = Table;
   TST_ASSERT_EQUALS(3, NewTable.size());
   TST_ASSERT_EQUALS("ONE", NewTable[1]);
   TST_ASSERT_EQUALS("TWO", NewTable[2]);
   TST_ASSERT_EQUALS("THREE", NewTable[3]);
}

void testCOLhashmap::copyTest()
{
   COLhashmap<COLuint32,COLstring> Table(COLhash<COLuint32>::defaultHash);
   Table.add(1,"ONE");
   Table.add(2,"TWO");
   Table.add(3,"THREE");

   COLhashmap<COLuint32,COLstring> NewTable(Table);
   TST_ASSERT_EQUALS("ONE", NewTable[1]);
   TST_ASSERT_EQUALS("TWO", NewTable[2]);
   TST_ASSERT_EQUALS("THREE", NewTable[3]);
}

static void testStlMethods()
{
   {
      COLhashmap<COLstring,int> MyMap;
  
      MyMap["foo"] = 40;
      MyMap["bar"] += 55;
      MyMap["bar"] = 123;
      MyMap["baz"] += 11;
      MyMap["foo"] -= 333;
      MyMap["bar"] -= 200;
      MyMap["baz"] += 99;
  
      COLhashmap<COLstring,int>::iterator it = MyMap.begin(), E = MyMap.end();
      // output will be unordered
      for (; it != E; ++it) {
         COLcout << "key = " << it->first
                 << ", value = " << it->second << newline;
      }
      if (int Count = MyMap.count("foo")) {
         COLcout << "element foo exists with count of " << Count << newline;
      }
      COLhashmap<COLstring,int>::const_iterator ci = MyMap.find("bar");
      if (ci != MyMap.cend()) {
         COLcout << "Found item with key = " << ci->first
                 << ", value = " << ci->second << newline;
      }
   }

   COLhashmap<char,COLstring> Map(COLhash<char>::defaultHash);
   TST_ASSERT_EQUALS(0, Map.size());
   TST_ASSERT_EQUALS(true, Map.empty());
   TST_ASSERT(Map.begin() == Map.end());

   // test erasing begin() of an empty map
   COLhashmap<char,COLstring>::iterator ei;
   ei = Map.erase(Map.begin());
   TST_ASSERT(ei == Map.end());
   TST_ASSERT_EQUALS(0, Map.size());
   TST_ASSERT_EQUALS(true, Map.empty());
   TST_ASSERT(Map.begin() == Map.end());

   Map.add('D',"ddd");
   TST_ASSERT_EQUALS(1, Map.size());
   TST_ASSERT_EQUALS(false, Map.empty());
   TST_ASSERT(Map.begin() != Map.end());

   // test erasing begin() of a 1 element map
   ei = Map.erase(Map.begin());
   TST_ASSERT(ei == Map.end());
   TST_ASSERT_EQUALS(0, Map.size());
   TST_ASSERT_EQUALS(true, Map.empty());
   TST_ASSERT(Map.begin() == Map.end());

   Map.add('D',"ddd");
   TST_ASSERT_EQUALS(1, Map.size());
   TST_ASSERT_EQUALS(false, Map.empty());
   TST_ASSERT(Map.begin() != Map.end());

   Map.add('B',"bbb");
   TST_ASSERT_EQUALS(2, Map.size());
   TST_ASSERT_EQUALS(false, Map.empty());
   TST_ASSERT(Map.begin() != Map.end());

   Map.add('C',"ccc");
   TST_ASSERT_EQUALS(3, Map.size());
   TST_ASSERT_EQUALS(false, Map.empty());
   TST_ASSERT(Map.begin() != Map.end());

   Map.add('C',"ccc");
   TST_ASSERT_EQUALS(3, Map.size());
   TST_ASSERT_EQUALS(false, Map.empty());
   TST_ASSERT(Map.begin() != Map.end());

   Map.add('B',"bbb");
   TST_ASSERT_EQUALS(3, Map.size());
   TST_ASSERT_EQUALS(false, Map.empty());
   TST_ASSERT(Map.begin() != Map.end());

   Map.add('A',"aaa");
   TST_ASSERT_EQUALS(4, Map.size());
   TST_ASSERT_EQUALS(false, Map.empty());
   TST_ASSERT(Map.begin() != Map.end());

   // Element ordering below depends on the hash function COLhash<char>::defaultHash.
   // If the hash function changes, this test must be changed.

   // forward iteration test
   
   COLhashmap<char,COLstring>::iterator fi = Map.begin();
   COLhashmap<char,COLstring>::const_iterator cfi = Map.cbegin();
   TST_ASSERT_EQUALS('A', fi->first);
   TST_ASSERT_EQUALS('A', (*fi).first);
   (*fi).second = (*fi).second;
   TST_ASSERT_EQUALS("aaa", fi->second);
   TST_ASSERT_EQUALS("aaa", (*fi).second);
   TST_ASSERT_EQUALS('A', cfi->first);
   TST_ASSERT_EQUALS('A', (*cfi).first);
   TST_ASSERT_EQUALS("aaa", cfi->second);
   TST_ASSERT_EQUALS("aaa", (*cfi).second);
   TST_ASSERT(Map.begin() == fi);
   TST_ASSERT(Map.end() != fi);
   TST_ASSERT(cfi == fi);

   char key  = (++fi)->first;
   char ckey = (++cfi)->first;
   TST_ASSERT_EQUALS('C', key);
   TST_ASSERT_EQUALS('C', ckey);
   TST_ASSERT_EQUALS('C', (*fi).first);
   (*fi).second = (*fi).second;
   fi->second = fi->second;
   //(*fi).first = (*fi).first; // correctly does not compile
   //fi->first = fi->first;     // correctly does not compile
   TST_ASSERT_EQUALS('C', fi->first);
   TST_ASSERT_EQUALS("ccc", (*fi).second);
   TST_ASSERT_EQUALS("ccc", fi->second);
   TST_ASSERT_EQUALS('C', (*cfi).first);
   TST_ASSERT_EQUALS('C', cfi->first);
   TST_ASSERT_EQUALS("ccc", (*cfi).second);
   TST_ASSERT_EQUALS("ccc", cfi->second);
   TST_ASSERT(Map.begin() != fi);
   TST_ASSERT(Map.end() != fi);
   TST_ASSERT(Map.cbegin() != cfi);
   TST_ASSERT(Map.cend() != cfi);
   TST_ASSERT(cfi == fi);

   key  = (fi++)->first;
   ckey = (cfi++)->first;
   TST_ASSERT_EQUALS('C', key);
   TST_ASSERT_EQUALS('C', ckey);
   TST_ASSERT_EQUALS('B', (*fi).first);
   (*fi).second = (*fi).second;
   fi->second = fi->second;
   TST_ASSERT_EQUALS('B', fi->first);
   TST_ASSERT_EQUALS("bbb", (*fi).second);
   TST_ASSERT_EQUALS("bbb", fi->second);
   TST_ASSERT_EQUALS('B', (*cfi).first);
   TST_ASSERT_EQUALS('B', cfi->first);
   TST_ASSERT_EQUALS("bbb", (*cfi).second);
   TST_ASSERT_EQUALS("bbb", cfi->second);
   TST_ASSERT(Map.begin() != fi);
   TST_ASSERT(Map.end() != fi);
   TST_ASSERT(Map.cbegin() != cfi);
   TST_ASSERT(Map.cend() != cfi);
   TST_ASSERT(cfi == fi);

   key  = (--fi)->first;
   ckey = (--cfi)->first;
   TST_ASSERT_EQUALS('C', key);
   TST_ASSERT_EQUALS('C', ckey);
   TST_ASSERT_EQUALS('C', (*fi).first);
   (*fi).second = (*fi).second;
   fi->second = fi->second;
   TST_ASSERT_EQUALS('C', fi->first);
   TST_ASSERT_EQUALS("ccc", (*fi).second);
   TST_ASSERT_EQUALS("ccc", fi->second);
   TST_ASSERT_EQUALS('C', (*cfi).first);
   TST_ASSERT_EQUALS('C', cfi->first);
   TST_ASSERT_EQUALS("ccc", (*cfi).second);
   TST_ASSERT_EQUALS("ccc", cfi->second);
   TST_ASSERT(Map.begin() != fi);
   TST_ASSERT(Map.end() != fi);
   TST_ASSERT(Map.cbegin() != cfi);
   TST_ASSERT(Map.cend() != cfi);
   TST_ASSERT(cfi == fi);

   key  = (fi--)->first;
   ckey = (cfi--)->first;
   TST_ASSERT_EQUALS('C', key);
   TST_ASSERT_EQUALS('C', ckey);
   TST_ASSERT_EQUALS('A', fi->first);
   TST_ASSERT_EQUALS('A', (*fi).first);
   (*fi).second = (*fi).second;
   fi->second = fi->second;
   TST_ASSERT_EQUALS("aaa", fi->second);
   TST_ASSERT_EQUALS("aaa", (*fi).second);
   TST_ASSERT_EQUALS('A', cfi->first);
   TST_ASSERT_EQUALS('A', (*cfi).first);
   TST_ASSERT_EQUALS("aaa", cfi->second);
   TST_ASSERT_EQUALS("aaa", (*cfi).second);
   TST_ASSERT(Map.begin() == fi);
   TST_ASSERT(Map.end() != fi);
   TST_ASSERT(Map.cbegin() == cfi);
   TST_ASSERT(Map.cend() != cfi);
   TST_ASSERT(cfi == fi);

   ++fi;
   ++cfi;
   TST_ASSERT_EQUALS('C', (*fi).first);
   (*fi).second = (*fi).second;
   fi->second = fi->second;
   TST_ASSERT_EQUALS('C', fi->first);
   TST_ASSERT_EQUALS("ccc", (*fi).second);
   TST_ASSERT_EQUALS("ccc", fi->second);
   TST_ASSERT_EQUALS('C', (*cfi).first);
   TST_ASSERT_EQUALS('C', cfi->first);
   TST_ASSERT_EQUALS("ccc", (*cfi).second);
   TST_ASSERT_EQUALS("ccc", cfi->second);
   TST_ASSERT(Map.begin() != fi);
   TST_ASSERT(Map.end() != fi);
   TST_ASSERT(cfi == fi);
   TST_ASSERT(Map.cbegin() != cfi);
   TST_ASSERT(Map.cend() != cfi);

   fi++;
   cfi++;
   TST_ASSERT_EQUALS('B', (*fi).first);
   (*fi).second = (*fi).second;
   fi->second = fi->second;
   TST_ASSERT_EQUALS('B', fi->first);
   TST_ASSERT_EQUALS("bbb", (*fi).second);
   TST_ASSERT_EQUALS("bbb", fi->second);
   TST_ASSERT_EQUALS('B', (*cfi).first);
   TST_ASSERT_EQUALS('B', cfi->first);
   TST_ASSERT_EQUALS("bbb", (*cfi).second);
   TST_ASSERT_EQUALS("bbb", cfi->second);
   TST_ASSERT(Map.begin() != fi);
   TST_ASSERT(Map.end() != fi);
   TST_ASSERT(Map.cbegin() != cfi);
   TST_ASSERT(Map.cend() != cfi);
   TST_ASSERT(cfi == fi);

   ++fi;
   ++cfi;
   TST_ASSERT_EQUALS('D', (*fi).first);
   (*fi).second = (*fi).second;
   fi->second = fi->second;
   //(*cfi).first = (*cfi).first;   // correctly does not compile
   //cfi->first = cfi->first;       // correctly does not compile
   //(*cfi).second = (*cfi).second; // correctly does not compile
   //cfi->second = cfi->second;     // correctly does not compile
   TST_ASSERT_EQUALS('D', fi->first);
   TST_ASSERT_EQUALS("ddd", (*fi).second);
   TST_ASSERT_EQUALS("ddd", fi->second);
   TST_ASSERT_EQUALS('D', (*cfi).first);
   TST_ASSERT_EQUALS('D', cfi->first);
   TST_ASSERT_EQUALS("ddd", (*cfi).second);
   TST_ASSERT_EQUALS("ddd", cfi->second);
   TST_ASSERT(Map.begin() != fi);
   TST_ASSERT(Map.end() != fi);
   TST_ASSERT(Map.cbegin() != cfi);
   TST_ASSERT(Map.cend() != cfi);
   TST_ASSERT(cfi == fi);

   ++fi;
   ++cfi;
   TST_ASSERT(Map.begin() != fi);
   TST_ASSERT(Map.end() == fi);
   TST_ASSERT(Map.cbegin() != cfi);
   TST_ASSERT(Map.cend() == cfi);
   TST_ASSERT(cfi == fi);

   // test erasing all elements from begin()
   while (!Map.empty()) {
      ei = Map.erase(Map.begin());
   }
   TST_ASSERT(Map.end() == ei);
   TST_ASSERT(Map.empty() == true);
   TST_ASSERT(Map.size() == 0);

   // Test repopulating the map and erasing elements past begin().
   
   Map['I'] = "iii";
   Map['H'] = "hhh";
   Map['H'] = "hhhh";
   Map['Z'] = "zzz";
   Map['U'] = "uuu";
   Map['J'] = "jjj";
   Map['G'] = "ggg";
   Map['J'] = "jj";
   Map['E'] = "eee";
   Map['Q'] = "qqq";
   Map['I'] = "iiii";
   TST_ASSERT(!Map.empty());
   TST_ASSERT_EQUALS(8, Map.size());

   // test count()
   TST_ASSERT_EQUALS(1, Map.count('Q'));
   TST_ASSERT_EQUALS(1, Map.count('H'));
   TST_ASSERT_EQUALS(1, Map.count('Z'));
   TST_ASSERT_EQUALS(0, Map.count(':'));

   // test erasing elements
   Map.erase('Q');
   TST_ASSERT_EQUALS(7, Map.size());
   Map.erase('Z');
   TST_ASSERT_EQUALS(6, Map.size());

   // test count()
   TST_ASSERT_EQUALS(0, Map.count('Q'));
   TST_ASSERT_EQUALS(1, Map.count('H'));
   TST_ASSERT_EQUALS(0, Map.count('Z'));
   TST_ASSERT_EQUALS(0, Map.count(':'));

   // test erasing non-existent elements
   Map.erase('?');
   TST_ASSERT_EQUALS(6, Map.size());
   Map.erase('\000');
   TST_ASSERT_EQUALS(6, Map.size());

   // make sure iterator returned is valid after erase()
   fi = Map.begin();
   fi++;    // skip the first element
   fi++;    // skip the second element
   ei = fi; // both iterators now pointing to third element
   TST_ASSERT(ei == fi);
   while (Map.size() > 2) {
      ++fi;
      ei = Map.erase(ei);
      TST_ASSERT(ei == fi);
   }
   TST_ASSERT(Map.size() == 2);

   ei = Map.erase(Map.begin());
   TST_ASSERT(Map.size() == 1);

   ei = Map.erase(Map.begin());
   TST_ASSERT(Map.size() == 0);
}

TSTtestable* testCOLhashmap::COLhashmapTests()
{
   TST_START_FIXTURE(testCOLhashmap)
      TST_ADD_TEST_METHOD(initTest);
      TST_ADD_TEST_METHOD(addTest);
      TST_ADD_TEST_METHOD(removeTest);
      TST_ADD_TEST_METHOD(clearTest);
      TST_ADD_TEST_METHOD(iterationTest);
      TST_ADD_TEST_METHOD(randomTest);
      TST_ADD_TEST_METHOD(mapTest);
      TST_ADD_TEST_FUNCTION(defaultHashTest);
      TST_ADD_TEST_METHOD(iterateTest);
      TST_ADD_TEST_METHOD(iterateFindTest);
      TST_ADD_TEST_METHOD(assignmentTest);
      TST_ADD_TEST_METHOD(copyTest);
      TST_ADD_TEST_FUNCTION(testStlMethods);
   TST_END_FIXTURE
}
