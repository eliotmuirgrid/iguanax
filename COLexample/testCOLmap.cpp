//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testCOLmap.cpp
//
// Description:
//
// Implementation
//
// Author: Ian Farquharson
// Date:   Thu 02/05/2004
//
//---------------------------------------------------------------------------

#ifndef _WIN32
// needed for testEraseWithCOLstringKey to include POSIX remove(const char*) declaration
#include <stdio.h>
#endif

#include "COLexamplePrecomp.h"
#pragma hdrstop

#include "testCOLmap.h"

#define MAX_ITEMS 100

typedef COLint16 TestType;

typedef COLlist<TestType>                 TestListType;
typedef COLmap<TestType, TestType> TestSortedType;

static COLint32 Compare(const TestType& Value1, const TestType& Value2)
{
   return Value1 - Value2;
}

testCOLmap::testCOLmap() : Sequence(0), TestSorted(Compare)
{
};

void testCOLmap::testCheck(const bool Condition)
{
   TestCount++;
   TST_ASSERT_MESSAGE(Condition, TestCount << " test failed");
}

void testCOLmap::testCompare()
{
   TestCount++;
   TST_ASSERT_MESSAGE(Control.size() == TestSorted.size(),TestCount << " test failed with an incorrect size");
   COLlistPlace ListPlace = Control.first();
   COLmapPlace SortedListPlace = TestSorted.first();
   while (ListPlace != NULL && SortedListPlace != NULL)
   {
      TST_ASSERT_MESSAGE(TestSorted[SortedListPlace] == Control[ListPlace], TestCount << " test failed with an different contents");
      ListPlace = Control.next(ListPlace);
      SortedListPlace = TestSorted.next(SortedListPlace);
   }
   TST_ASSERT_MESSAGE(ListPlace == NULL && SortedListPlace == NULL, TestCount << " test failed with an different contents");
}

COLlistPlace testCOLmap::add(TestListType& Control, const TestType Item)
{
   for (COLlistPlace Place = Control.first(); Place != NULL; Place = Control.next(Place))
   {
      if (Control[Place] >= Item)
      {
         return Control.insert(Item, Place);
      }
   }
   return Control.add(Item);
}

void testCOLmap::remove(TestListType& Control, const TestType Item)
{
   for (COLlistPlace Place = Control.first(); Place != NULL; Place = Control.next(Place))
   {
      if (Control[Place] == Item)
      {
         Control.remove(Place);
         break;
      }
   }
}

void testCOLmap::initTest()
{
   Sequence = 0;

   testCompare();
}

void testCOLmap::addTest()
{
   // Add

   COLindex ListIndex = 0;
   for (ListIndex = 0; ListIndex < MAX_ITEMS; ListIndex++)
   {
      Sequence += 3;
      TestSorted.add(Sequence, Sequence);
      add(Control, Sequence);
      testCompare();
   }

   Sequence = 2;
   for (ListIndex = 0; ListIndex < MAX_ITEMS; ListIndex++)
   {
      Sequence += 3;
      TestSorted.add(Sequence, Sequence);
      add(Control, Sequence);
      testCompare();
   }
}

void testCOLmap::stableAddTest()
{
   static const int Keys[] = {
      0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
      9, 8, 7, 6, 5, 4, 3, 2, 1, 0,
      9, 0, 7, 2, 5, 4, 3, 6, 1, 8,
   };
   const int Size = sizeof Keys / sizeof Keys[0];
   COLmap<int,int> Map;

   for(int i=0; i < Size; i++)
   {
      Map.add(Keys[i], i);
   }

   TST_ASSERT_EQUALS_MESSAGE(Size, Map.size(), "Insert failure.");

   COLmapPlace Prev = Map.first();
   COLmapPlace Curr = Map.next(Prev);
   for(; Curr; Prev = Curr, Curr = Map.next(Prev))
   {
      TST_ASSERT_MESSAGE(Map.key(Prev) <= Map.key(Curr), "Basic order wrong.");
      TST_ASSERT_MESSAGE(Map.key(Prev) != Map.key(Curr) || Map[Prev] < Map[Curr], "Stable order wrong.");
   }
}

void testCOLmap::addUniqueTest()
{
   // AddUnique

   Sequence = 0;
   COLindex ListIndex = 0;
   for (ListIndex = 0; ListIndex < MAX_ITEMS; ListIndex++)
   {
      Sequence += 3;
      testCheck(TestSorted.addUnique(Sequence, Sequence) == NULL);
   }
   testCompare();

   Sequence = 1;
   for (ListIndex = 0; ListIndex < MAX_ITEMS; ListIndex++)
   {
      Sequence += 3;
      TestSorted.addUnique(Sequence, Sequence);
      add(Control, Sequence);
      testCompare();
   }
   testCompare();
}

void testCOLmap::removeTest()
{
   // Remove

   COLindex ListIndex = 0;
   for (ListIndex = 0; ListIndex < 20; ListIndex++)
   {
      TestSorted.remove(TestSorted.first());
      Control.remove(Control.first());
      testCompare();
   }

   for (ListIndex = 0; ListIndex < 20; ListIndex++)
   {
      TestSorted.remove(TestSorted.last());
      Control.remove(Control.last());
      testCompare();
   }

   for (ListIndex = 0; ListIndex < 20; ListIndex++)
   {
      TestSorted.remove(TestSorted.next(TestSorted.first()));
      Control.remove(Control.next(Control.first()));
      testCompare();
   }

   for (ListIndex = 0; ListIndex < 20; ListIndex++)
   {
      TestSorted.remove(TestSorted.previous(TestSorted.last()));
      Control.remove(Control.previous(Control.last()));
      testCompare();
   }
}

void testCOLmap::removeKeyTest()
{
   // RemoveKey

   {
      COLlistPlace Place1 = NULL;
      while (Control.size() > 0)
      {
         for (COLindex PlaceIndex = 0; PlaceIndex < 5; PlaceIndex++)
         {
            Place1 = Control.next(Place1);
         }
         if (Place1 == NULL)
         {
            Place1 = Control.next(Place1);
         }
         COLlistPlace Place2 = Place1;
         Place1 = Control.next(Place1);
         TestSorted.removeKey(Control[Place2]);
         Control.remove(Place2);
         testCompare();
      }
   }

   testCheck(!TestSorted.removeKey(0));
   testCheck(TestSorted.size() == 0);
}

void testCOLmap::clearTest()
{
   // Clear

   for (COLindex ListIndex = 0; ListIndex < MAX_ITEMS; ListIndex++)
   {
      Sequence += 3;
      TestSorted.addUnique(Sequence, Sequence);
      add(Control, Sequence);
      testCompare();
   }

   TestSorted.clear();
   Control.clear();
   testCompare();
   testCheck(TestSorted.size() == 0);
}

void testCOLmap::walkingTest()
{
   // Testing Walking
   COLindex ListIndex = 0;
   for (ListIndex = 0; ListIndex < MAX_ITEMS; ListIndex++)
   {
      Sequence += 3;
      TestSorted.add(Sequence, Sequence);
      add(Control, Sequence);
   }
   testCompare();

   for (ListIndex = 0; ListIndex < MAX_ITEMS; ListIndex++)
   {
      TestSorted.removeKey(Control[Control.first()]);
      testCheck(TestSorted.find(Control[Control.first()]) == NULL);
      Control.remove(Control.first());
      testCompare();

      if (ListIndex % 100 == 0)
      {
         COLuint32 SizeOfList = 0;
         for (COLmapPlace Place = TestSorted.first(); Place != NULL; Place = TestSorted.next(Place))
         {
            if (TestSorted.next(Place) == NULL)
            {
               testCheck(Place == TestSorted.last());
            }
            else
            {
               testCheck(Place == TestSorted.previous(TestSorted.next(Place)));
            }
            SizeOfList++;
         }
         testCheck(SizeOfList == (COLuint32) TestSorted.size());
         testCheck(TestSorted.next(NULL) == TestSorted.first());
         testCheck(TestSorted.previous(NULL) == TestSorted.last());
      }
   }
   testCheck(TestSorted.next(NULL) == TestSorted.first());
   testCheck(TestSorted.previous(NULL) == TestSorted.last());
   testCheck(TestSorted.size() == 0);
}

void testCOLmap::findTest()
{
   // Testing Finds
   COLindex ListIndex = 0;
   for (ListIndex = 0; ListIndex < MAX_ITEMS; ListIndex++)
   {
      Sequence += 3;
      TestSorted.add(Sequence, Sequence);
      add(Control, Sequence);
   }
   testCompare();
   COLmapPlace SortedListPlace1 = 0;
   for (SortedListPlace1 = TestSorted.first(); SortedListPlace1 != NULL; SortedListPlace1 = TestSorted.next(SortedListPlace1))
   {
      testCheck(TestSorted.find(TestSorted[SortedListPlace1]) == SortedListPlace1);
      testCheck(TestSorted.find(TestSorted[SortedListPlace1] - 1) == NULL);
      testCheck(TestSorted.find(TestSorted[SortedListPlace1] + 1) == NULL);

      testCheck(TestSorted.findNearest(TestSorted[SortedListPlace1]) == SortedListPlace1);
      testCheck(TestSorted.findNearest(TestSorted[SortedListPlace1] - 1) == SortedListPlace1);
      testCheck(TestSorted.findNearest(TestSorted[SortedListPlace1] + 1) == (SortedListPlace1 == TestSorted.last() ? SortedListPlace1 : TestSorted.next(SortedListPlace1)));

      testCheck(TestSorted.findAbove(TestSorted[SortedListPlace1]) == TestSorted.next(SortedListPlace1));
      testCheck(TestSorted.findAbove(TestSorted[SortedListPlace1] - 1) == SortedListPlace1);
      testCheck(TestSorted.findAbove(TestSorted[SortedListPlace1] + 1) == TestSorted.next(SortedListPlace1));

      testCheck(TestSorted.findBelow(TestSorted[SortedListPlace1]) == TestSorted.previous(SortedListPlace1));
      testCheck(TestSorted.findBelow(TestSorted[SortedListPlace1] - 1) == TestSorted.previous(SortedListPlace1));
      testCheck(TestSorted.findBelow(TestSorted[SortedListPlace1] + 1) == SortedListPlace1);
   }

   for (ListIndex = 0; ListIndex < 500; ListIndex++)
   {
      Sequence -= 6;
      TestSorted.add(Sequence, Sequence);
      add(Control, Sequence);
   }
   testCompare();
   for (ListIndex = 0; ListIndex < 250; ListIndex++)
   {
      Sequence += 12;
      TestSorted.add(Sequence, Sequence);
      add(Control, Sequence);
   }
   testCompare();

   for (SortedListPlace1 = TestSorted.first(); SortedListPlace1 != NULL; SortedListPlace1 = TestSorted.next(SortedListPlace1))
   {
      COLmapPlace SortedListPlace2;
      testCheck(TestSorted[TestSorted.find(TestSorted[SortedListPlace1])] == TestSorted[SortedListPlace1]);
      testCheck(TestSorted.find(TestSorted[SortedListPlace1] - 1) == NULL);
      testCheck(TestSorted.find(TestSorted[SortedListPlace1] + 1) == NULL);

      testCheck(TestSorted[TestSorted.findNearest(TestSorted[SortedListPlace1])] == TestSorted[SortedListPlace1]);
      testCheck(TestSorted[TestSorted.findNearest(TestSorted[SortedListPlace1] - 1)] == TestSorted[SortedListPlace1]);
      SortedListPlace2 = TestSorted.findNearest(TestSorted[SortedListPlace1] + 1);
      if (TestSorted[SortedListPlace2] > TestSorted[SortedListPlace1])
      {
         testCheck(TestSorted[TestSorted.previous(SortedListPlace2)] == TestSorted[SortedListPlace1]);
      }
      else
      {
         testCheck(TestSorted[SortedListPlace2] == TestSorted[SortedListPlace1]);
         testCheck(TestSorted[TestSorted.last()] == TestSorted[SortedListPlace1]);
      }

      SortedListPlace2 = TestSorted.findAbove(TestSorted[SortedListPlace1]);
      testCheck(TestSorted[TestSorted.previous(SortedListPlace2)] == TestSorted[SortedListPlace1]);
      if (SortedListPlace2 == NULL)
      {
         testCheck(TestSorted[TestSorted.last()] == TestSorted[SortedListPlace1]);
      }
      else
      {
         testCheck(TestSorted[SortedListPlace2] > TestSorted[SortedListPlace1]);
      }
      SortedListPlace2 = TestSorted.findAbove(TestSorted[SortedListPlace1] - 1);
      testCheck(TestSorted[SortedListPlace2] == TestSorted[SortedListPlace1]);
      SortedListPlace2 = TestSorted.previous(SortedListPlace2);
      if (SortedListPlace2 == NULL)
      {
         testCheck(TestSorted[TestSorted.first()] == TestSorted[SortedListPlace1]);
      }
      else
      {
         testCheck(TestSorted[SortedListPlace2] < TestSorted[SortedListPlace1]);
      }
      SortedListPlace2 = TestSorted.findAbove(TestSorted[SortedListPlace1] + 1);
      testCheck(TestSorted[TestSorted.previous(SortedListPlace2)] == TestSorted[SortedListPlace1]);
      if (SortedListPlace2 == NULL)
      {
         testCheck(TestSorted[TestSorted.last()] == TestSorted[SortedListPlace1]);
      }
      else
      {
         testCheck(TestSorted[SortedListPlace2] > TestSorted[SortedListPlace1]);
      }

      SortedListPlace2 = TestSorted.findBelow(TestSorted[SortedListPlace1]);
      testCheck(TestSorted[TestSorted.next(SortedListPlace2)] == TestSorted[SortedListPlace1]);
      if (SortedListPlace2 == NULL)
      {
         testCheck(TestSorted[TestSorted.first()] == TestSorted[SortedListPlace1]);
      }
      else
      {
         testCheck(TestSorted[SortedListPlace2] < TestSorted[SortedListPlace1]);
      }
      SortedListPlace2 = TestSorted.findBelow(TestSorted[SortedListPlace1] + 1);
      testCheck(TestSorted[SortedListPlace2] == TestSorted[SortedListPlace1]);
      SortedListPlace2 = TestSorted.next(SortedListPlace2);
      if (SortedListPlace2 == NULL)
      {
         testCheck(TestSorted[TestSorted.last()] == TestSorted[SortedListPlace1]);
      }
      else
      {
         testCheck(TestSorted[SortedListPlace2] > TestSorted[SortedListPlace1]);
      }
      SortedListPlace2 = TestSorted.findBelow(TestSorted[SortedListPlace1] - 1);
      testCheck(TestSorted[TestSorted.next(SortedListPlace2)] == TestSorted[SortedListPlace1]);
      if (SortedListPlace2 == NULL)
      {
         testCheck(TestSorted[TestSorted.first()] == TestSorted[SortedListPlace1]);
      }
      else
      {
         testCheck(TestSorted[SortedListPlace2] < TestSorted[SortedListPlace1]);
      }
   }
}

void testCOLmap::iterateAndDeleteTest()
{
   COLmap<TestType, TestType> Map(Compare);
   for (int i = 31; i--;)
   {
      Map.add(i, 1000-i);
   }
   TST_ASSERT(Map.size() == 31);
   for (COLmapPlace Place = Map.first(); Place;)
   {
      if (Map[Place] % 2 == 1)
      {
         COLmapPlace NextPlace = Map.next(Place);
         Map.remove(Place);
         Place = NextPlace;
         continue;
      }
      Place = Map.next(Place);
   }
   TST_ASSERT(Map.size() == 16);
   {
      // make a useless block for hpux-ita aCC since it complains that
      // "Place" has already been declared in the current scope
      for (COLmapPlace Place = Map.first(); Place;)
      {
         COLmapPlace NextPlace = Map.next(Place);
         Map.remove(Place);
         Place = NextPlace;
      }
   }
   TST_ASSERT(Map.size() == 0);
}

void testCOLmap::randomTest()
{
   COLuint32 Counter;
  
   for (COLindex ListIndex = 0; ListIndex < MAX_ITEMS; ListIndex++)
   {
      for (Counter = rand() % 55 + 1; Counter > 0 && TestSorted.size() < 1000; Counter--)
      {
         TestType Sequence = rand();
         TestSorted.add(Sequence, Sequence);
         add(Control, Sequence);
      }
      for (Counter =  rand() % 50 + 1; Counter > 0 && TestSorted.size() > 0; Counter--)
      {
         COLmapPlace Place = TestSorted.first();
         for (COLindex ListIndex = rand() % TestSorted.size(); ListIndex > 0; ListIndex--) // interate one less than size() times
         {
            Place = TestSorted.next(Place);
         }
         remove(Control, TestSorted[Place]);
         if (Counter % 2 == 0)
         {
            TestSorted.remove(Place);
         }
         else
         {
            TestSorted.removeKey(TestSorted[Place]);
         }
      }
      testCompare();
   }
}

static void testFindFirst()
{
   COLmap<COLstring, COLuint32> TestList;
   int i = 0;
   TestList.add("A",i++);
   for(;i < 100; TestList.add("B",i++));
   TestList.add("C",i++);
   
   //This is to make sure that we're not lucky in
   //terms of matching the first value
   COLmapPlace Place = TestList.find("B");
   TST_ASSERT(TestList[Place] != 1);
   
   Place = TestList.findFirst("B");
   TST_ASSERT(Place);
   TST_ASSERT_EQUALS(1, TestList[Place]);
   int reference = 1;
   COLmapPlace LastPlace = TestList.findAbove("B");
   TST_ASSERT_EQUALS("C",TestList.key(LastPlace));
   for (; Place && Place != LastPlace; Place = TestList.next(Place))
   {
      TST_ASSERT_EQUALS(reference, TestList[Place]);
      TST_ASSERT_EQUALS("B", TestList.key(Place));
      reference++;
   }
   TST_ASSERT_EQUALS(reference, TestList[Place]);
   TST_ASSERT_EQUALS("C", TestList.key(Place));
   
   Place = TestList.findFirst("@");
   TST_ASSERT(!Place);

   TestList.clear();
   TestList.add("B",1);
   TestList.add("B",2);
   Place = TestList.findFirst("A");
   TST_ASSERT(!Place);
   
   TestList.clear();
   TestList.add("A",1);
   Place = TestList.findFirst("A");
   TST_ASSERT(Place == TestList.first());

   TestList.clear();
   Place = TestList.findFirst("A");
   TST_ASSERT(!Place);
}

static void copyTest()
{
   COLmap<COLuint32,COLstring> Table;
   Table.add(5,"Five");
   Table.add(2,"TWO");
   Table.add(4,"four");
   Table.add(6,"Six");
   Table.add(3,"THREE");
   Table.add(1,"ONE");
   TST_ASSERT_EQUALS(6, Table.size());

   {
      //
      // test element iteration order
      //
      COLmap<COLuint32,COLstring>::iterator it = Table.begin();
      TST_ASSERT_EQUALS(Table.cbegin(), it);
      TST_ASSERT_EQUALS(Table.begin(), it);
      TST_ASSERT(Table.cend() != it);
      TST_ASSERT(Table.end() != it);
      TST_ASSERT_EQUALS(1, it->first);
      TST_ASSERT_EQUALS("ONE", it->second);
   
      ++it;
      TST_ASSERT(Table.cbegin() != it);
      TST_ASSERT(Table.begin() != it);
      TST_ASSERT(Table.cend() != it);
      TST_ASSERT(Table.end() != it);
      TST_ASSERT_EQUALS(2, it->first);
      TST_ASSERT_EQUALS("TWO", it->second);
   
      it++;
      TST_ASSERT(Table.cbegin() != it);
      TST_ASSERT(Table.begin() != it);
      TST_ASSERT(Table.cend() != it);
      TST_ASSERT(Table.end() != it);
      TST_ASSERT_EQUALS(3, it->first);
      TST_ASSERT_EQUALS("THREE", it->second);
   
      ++it;
      TST_ASSERT(Table.cbegin() != it);
      TST_ASSERT(Table.begin() != it);
      TST_ASSERT(Table.cend() != it);
      TST_ASSERT(Table.end() != it);
      TST_ASSERT_EQUALS(4, it->first);
      TST_ASSERT_EQUALS("four", it->second);
   
      it++;
      TST_ASSERT(Table.cbegin() != it);
      TST_ASSERT(Table.begin() != it);
      TST_ASSERT(Table.cend() != it);
      TST_ASSERT(Table.end() != it);
      TST_ASSERT_EQUALS(5, it->first);
      TST_ASSERT_EQUALS("Five", it->second);
   
      ++it;
      TST_ASSERT(Table.cbegin() != it);
      TST_ASSERT(Table.begin() != it);
      TST_ASSERT(Table.cend() != it);
      TST_ASSERT(Table.end() != it);
      TST_ASSERT_EQUALS(6, it->first);
      TST_ASSERT_EQUALS("Six", it->second);
   
      ++it;
      TST_ASSERT_EQUALS(Table.cend(), it);
      TST_ASSERT_EQUALS(Table.end(), it);
   }

   // test copy ctor
   COLmap<COLuint32,COLstring> NewTable1(Table);
   TST_ASSERT_EQUALS(6, NewTable1.size());
   TST_ASSERT_EQUALS("ONE",   NewTable1[1]);
   TST_ASSERT_EQUALS("TWO",   NewTable1[2]);
   TST_ASSERT_EQUALS("THREE", NewTable1[3]);
   TST_ASSERT_EQUALS("four",  NewTable1[4]);
   TST_ASSERT_EQUALS("Five",  NewTable1[5]);
   TST_ASSERT_EQUALS("Six",   NewTable1[6]);

   // test operator=
   COLmap<COLuint32,COLstring> NewTable2;
   NewTable2 = NewTable1;
   TST_ASSERT_EQUALS(6, NewTable2.size());
   TST_ASSERT_EQUALS("ONE",   NewTable2[1]);
   TST_ASSERT_EQUALS("TWO",   NewTable2[2]);
   TST_ASSERT_EQUALS("THREE", NewTable2[3]);
   TST_ASSERT_EQUALS("four",  NewTable2[4]);
   TST_ASSERT_EQUALS("Five",  NewTable2[5]);
   TST_ASSERT_EQUALS("Six",   NewTable2[6]);

   {
      //
      // test element iteration order
      //
      COLmap<COLuint32,COLstring>::const_iterator ci = NewTable2.begin();
      TST_ASSERT_EQUALS(NewTable2.cbegin(), ci);
      TST_ASSERT_EQUALS(NewTable2.begin(), ci);
      TST_ASSERT(NewTable2.cend() != ci);
      TST_ASSERT(NewTable2.end() != ci);
      TST_ASSERT_EQUALS(1, ci->first);
      TST_ASSERT_EQUALS("ONE", ci->second);
   
      ++ci;
      TST_ASSERT(NewTable2.cbegin() != ci);
      TST_ASSERT(NewTable2.begin() != ci);
      TST_ASSERT(NewTable2.cend() != ci);
      TST_ASSERT(NewTable2.end() != ci);
      TST_ASSERT_EQUALS(2, ci->first);
      TST_ASSERT_EQUALS("TWO", ci->second);
   
      ci++;
      TST_ASSERT(NewTable2.cbegin() != ci);
      TST_ASSERT(NewTable2.begin() != ci);
      TST_ASSERT(NewTable2.cend() != ci);
      TST_ASSERT(NewTable2.end() != ci);
      TST_ASSERT_EQUALS(3, ci->first);
      TST_ASSERT_EQUALS("THREE", ci->second);
   
      ++ci;
      TST_ASSERT(NewTable2.cbegin() != ci);
      TST_ASSERT(NewTable2.begin() != ci);
      TST_ASSERT(NewTable2.cend() != ci);
      TST_ASSERT(NewTable2.end() != ci);
      TST_ASSERT_EQUALS(4, ci->first);
      TST_ASSERT_EQUALS("four", ci->second);
   
      ci++;
      TST_ASSERT(NewTable2.cbegin() != ci);
      TST_ASSERT(NewTable2.begin() != ci);
      TST_ASSERT(NewTable2.cend() != ci);
      TST_ASSERT(NewTable2.end() != ci);
      TST_ASSERT_EQUALS(5, ci->first);
      TST_ASSERT_EQUALS("Five", ci->second);
   
      ++ci;
      TST_ASSERT(NewTable2.cbegin() != ci);
      TST_ASSERT(NewTable2.begin() != ci);
      TST_ASSERT(NewTable2.cend() != ci);
      TST_ASSERT(NewTable2.end() != ci);
      TST_ASSERT_EQUALS(6, ci->first);
      TST_ASSERT_EQUALS("Six", ci->second);
   
      ++ci;
      TST_ASSERT_EQUALS(NewTable2.cend(), ci);
      TST_ASSERT_EQUALS(NewTable2.end(), ci);
   }
}

static void testStlMethods()
{
   COLmap<char,COLstring> Map;
   TST_ASSERT_EQUALS(0, Map.size());
   TST_ASSERT_EQUALS(true, Map.empty());
   TST_ASSERT(Map.begin() == Map.end());

   // test erasing begin() of an empty map
   COLmap<char,COLstring>::iterator ei;
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

   // add a duplicate key
   Map.add('C',"cee");
   TST_ASSERT_EQUALS(4, Map.size());
   TST_ASSERT_EQUALS(false, Map.empty());
   TST_ASSERT(Map.begin() != Map.end());

   // add a duplicate key
   Map.add('B',"bee");
   TST_ASSERT_EQUALS(5, Map.size());
   TST_ASSERT_EQUALS(false, Map.empty());
   TST_ASSERT(Map.begin() != Map.end());

   Map.add('A',"aaa");
   TST_ASSERT_EQUALS(6, Map.size());
   TST_ASSERT_EQUALS(false, Map.empty());
   TST_ASSERT(Map.begin() != Map.end());

   // Element ordering below depends on the hash function COLhash<char>::defaultHash.
   // If the hash function changes, this test must be changed.

   // forward iteration test
   
   COLmap<char,COLstring>::iterator fi = Map.begin();
   COLmap<char,COLstring>::const_iterator cfi = Map.cbegin();
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

   COLstring val  = (fi++)->second;
   COLstring cval = (cfi++)->second;
   TST_ASSERT_EQUALS("aaa", val);
   TST_ASSERT_EQUALS("aaa", cval);
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

   val  = (fi++)->second;
   cval = (cfi++)->second;
   TST_ASSERT_EQUALS("bbb", val);
   TST_ASSERT_EQUALS("bbb", cval);
   TST_ASSERT_EQUALS('B', (*fi).first);
   (*fi).second = (*fi).second;
   fi->second = fi->second;
   TST_ASSERT_EQUALS('B', fi->first);
   TST_ASSERT_EQUALS("bee", (*fi).second);
   TST_ASSERT_EQUALS("bee", fi->second);
   TST_ASSERT_EQUALS('B', (*cfi).first);
   TST_ASSERT_EQUALS('B', cfi->first);
   TST_ASSERT_EQUALS("bee", (*cfi).second);
   TST_ASSERT_EQUALS("bee", cfi->second);
   TST_ASSERT(Map.begin() != fi);
   TST_ASSERT(Map.end() != fi);
   TST_ASSERT(Map.cbegin() != cfi);
   TST_ASSERT(Map.cend() != cfi);
   TST_ASSERT(cfi == fi);

   val  = (++fi)->second;
   cval = (++cfi)->second;
   TST_ASSERT_EQUALS("ccc", val);
   TST_ASSERT_EQUALS("ccc", cval);
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

   val  = (++fi)->second;
   cval = (++cfi)->second;
   TST_ASSERT_EQUALS("cee", val);
   TST_ASSERT_EQUALS("cee", cval);
   TST_ASSERT_EQUALS('C', (*fi).first);
   (*fi).second = (*fi).second;
   fi->second = fi->second;
   //(*fi).first = (*fi).first; // correctly does not compile
   //fi->first = fi->first;     // correctly does not compile
   TST_ASSERT_EQUALS('C', fi->first);
   TST_ASSERT_EQUALS("cee", (*fi).second);
   TST_ASSERT_EQUALS("cee", fi->second);
   TST_ASSERT_EQUALS('C', (*cfi).first);
   TST_ASSERT_EQUALS('C', cfi->first);
   TST_ASSERT_EQUALS("cee", (*cfi).second);
   TST_ASSERT_EQUALS("cee", cfi->second);
   TST_ASSERT(Map.begin() != fi);
   TST_ASSERT(Map.end() != fi);
   TST_ASSERT(Map.cbegin() != cfi);
   TST_ASSERT(Map.cend() != cfi);
   TST_ASSERT(cfi == fi);

   val  = (--fi)->second;
   cval = (--cfi)->second;
   TST_ASSERT_EQUALS("ccc", val);
   TST_ASSERT_EQUALS("ccc", cval);
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

   val  = (fi--)->second;
   cval = (cfi--)->second;
   TST_ASSERT_EQUALS("ccc", val);
   TST_ASSERT_EQUALS("ccc", cval);
   TST_ASSERT_EQUALS('B', (*fi).first);
   (*fi).second = (*fi).second;
   fi->second = fi->second;
   TST_ASSERT_EQUALS('B', fi->first);
   TST_ASSERT_EQUALS("bee", (*fi).second);
   TST_ASSERT_EQUALS("bee", fi->second);
   TST_ASSERT_EQUALS('B', (*cfi).first);
   TST_ASSERT_EQUALS('B', cfi->first);
   TST_ASSERT_EQUALS("bee", (*cfi).second);
   TST_ASSERT_EQUALS("bee", cfi->second);
   TST_ASSERT(Map.begin() != fi);
   TST_ASSERT(Map.end() != fi);
   TST_ASSERT(Map.cbegin() != cfi);
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
   TST_ASSERT_EQUALS('C', (*fi).first);
   (*fi).second = (*fi).second;
   fi->second = fi->second;
   TST_ASSERT_EQUALS('C', fi->first);
   TST_ASSERT_EQUALS("cee", (*fi).second);
   TST_ASSERT_EQUALS("cee", fi->second);
   TST_ASSERT_EQUALS('C', (*cfi).first);
   TST_ASSERT_EQUALS('C', cfi->first);
   TST_ASSERT_EQUALS("cee", (*cfi).second);
   TST_ASSERT_EQUALS("cee", cfi->second);
   TST_ASSERT(Map.begin() != fi);
   TST_ASSERT(Map.end() != fi);
   TST_ASSERT(cfi == fi);
   TST_ASSERT(Map.cbegin() != cfi);
   TST_ASSERT(Map.cend() != cfi);

   fi++;
   cfi++;
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

   // test erasing elements
   Map.erase('Q');
   TST_ASSERT_EQUALS(7, Map.size());
   Map.erase('Z');
   TST_ASSERT_EQUALS(6, Map.size());

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

static void testEraseWithCOLstringKey()
{
   COLmap<COLstring,int> m;
   TST_ASSERT_EQUALS(0, m.size());
   const COLstring one = "one";
   const COLstring two = "two";
   const COLstring three = "three";
   const COLstring four = "four";
   m.add(one,1);
   TST_ASSERT_EQUALS(1, m.size());
   m.add(two,2);
   TST_ASSERT_EQUALS(2, m.size());
   m.add(three,3);
   TST_ASSERT_EQUALS(3, m.size());
   m.add(four,4);
   TST_ASSERT_EQUALS(4, m.size());

   // Compile error on Mac compiler at one point due to the unrelated
   // global remove(const char*) function used by COLmap.
   // Fixed in COLmap.h by qualifying member function with this->
   m.erase(two);

   TST_ASSERT_EQUALS(3, m.size());
   m.erase("one");
   TST_ASSERT_EQUALS(2, m.size());
   m.erase(COLstring("three"));
   TST_ASSERT_EQUALS(1, m.size());
   m.erase(two);
   TST_ASSERT_EQUALS(1, m.size());
   m.erase("one");
   TST_ASSERT_EQUALS(1, m.size());
   m.erase(COLstring("three"));
   TST_ASSERT_EQUALS(1, m.size());
   m.erase("four");
   TST_ASSERT_EQUALS(0, m.size());
}

static void testMultimapMethods()
{
   {
      COLmap<COLstring,int> MyMap;
  
      // map style with unique keys
      MyMap["foo"] = 123;
      MyMap["bar"] = 88;
      MyMap["baz"] = -400;
  
      // multimap style with non-unique keys
      MyMap.emplace("orange", 84);
      MyMap.emplace("green", 99);
      MyMap.emplace("green", 111);
  
      COLmap<COLstring,int>::iterator it = MyMap.begin(), E = MyMap.end();
      // output will be ordered by key
      for (; it != E; ++it) {
         COLcout << "key = " << it->first
                 << ", value = " << it->second << newline;
      }
      if (int Count = MyMap.count("green")) {
         COLcout << "element green exists with count of " << Count << newline;
      }
      COLmap<COLstring,int>::const_iterator ci = MyMap.find("bar");
      if (ci != MyMap.cend()) {
         COLcout << "Found item with key = " << ci->first
                 << ", value = " << ci->second << newline;
      }
   }

   COLmap<char,COLstring> myMultimap;

   // test empty map
   TST_ASSERT_EQUALS(0, myMultimap.count('a'));
   TST_ASSERT( myMultimap.end() == myMultimap.lower_bound('a') );
   TST_ASSERT( myMultimap.end() == myMultimap.upper_bound('a') );

   myMultimap.add('a',"A1");

   // test single element map

   TST_ASSERT_EQUALS(0,            myMultimap.count('0'));
   TST_ASSERT_EQUALS("A1",         myMultimap.lower_bound('0')->second);
   TST_ASSERT_EQUALS("A1",         myMultimap.upper_bound('0')->second);

   TST_ASSERT_EQUALS(1,            myMultimap.count('a'));
   TST_ASSERT_EQUALS("A1",         myMultimap.lower_bound('a')->second);
   TST_ASSERT( myMultimap.end() == myMultimap.upper_bound('a') );

   TST_ASSERT_EQUALS(0,            myMultimap.count('c'));
   TST_ASSERT( myMultimap.end() == myMultimap.lower_bound('c') );
   TST_ASSERT( myMultimap.end() == myMultimap.upper_bound('c') );

   // multiple elements

   myMultimap.add('b',"B1");
   myMultimap.add('c',"C1");

   TST_ASSERT_EQUALS(1, myMultimap.count('a'));
   TST_ASSERT_EQUALS(1, myMultimap.count('b'));
   TST_ASSERT_EQUALS(1, myMultimap.count('c'));

   myMultimap.add('c',"C2");

   TST_ASSERT_EQUALS(1, myMultimap.count('a'));
   TST_ASSERT_EQUALS(1, myMultimap.count('b'));
   TST_ASSERT_EQUALS(2, myMultimap.count('c'));

   myMultimap.add('c',"C3");

   TST_ASSERT_EQUALS(1, myMultimap.count('a'));
   TST_ASSERT_EQUALS(1, myMultimap.count('b'));
   TST_ASSERT_EQUALS(3, myMultimap.count('c'));

   myMultimap.add('d',"D1");
   myMultimap.add('e',"E1");
   myMultimap.add('c',"C4");

   TST_ASSERT_EQUALS(1, myMultimap.count('a'));
   TST_ASSERT_EQUALS(1, myMultimap.count('b'));
   TST_ASSERT_EQUALS(4, myMultimap.count('c'));

   myMultimap.add('x',"X1");
   myMultimap.add('c',"C5");

   TST_ASSERT_EQUALS(1, myMultimap.count('a'));
   TST_ASSERT_EQUALS(1, myMultimap.count('b'));
   TST_ASSERT_EQUALS(5, myMultimap.count('c'));

   // test non-const lower_bound and upper_bound
   {
      COLmap<char,COLstring>* mm = &myMultimap;
      COLmap<char,COLstring>::iterator it, lo, up;
      lo = mm->lower_bound('b'); // lo points to b
      up = mm->upper_bound('d'); // up points to e (not d)
   
      it = lo;
      TST_ASSERT(it != up);
      TST_ASSERT_EQUALS('b', it->first);
      TST_ASSERT_EQUALS("B1", it->second);
   
      ++it;
      TST_ASSERT(it != up);
      TST_ASSERT_EQUALS('c', it->first);
      TST_ASSERT_EQUALS("C1", it->second);
   
      ++it;
      TST_ASSERT(it != up);
      TST_ASSERT_EQUALS('c', it->first);
      TST_ASSERT_EQUALS("C2", it->second);
   
      ++it;
      TST_ASSERT(it != up);
      TST_ASSERT_EQUALS('c', it->first);
      TST_ASSERT_EQUALS("C3", it->second);
   
      ++it;
      TST_ASSERT(it != up);
      TST_ASSERT_EQUALS('c', it->first);
      TST_ASSERT_EQUALS("C4", it->second);
   
      ++it;
      TST_ASSERT(it != up);
      TST_ASSERT_EQUALS('c', it->first);
      TST_ASSERT_EQUALS("C5", it->second);
   
      ++it;
      TST_ASSERT(it != up);
      TST_ASSERT_EQUALS('d', it->first);
      TST_ASSERT_EQUALS("D1", it->second);
   
      ++it;
      TST_ASSERT(it == up);
      TST_ASSERT(it != mm->end());
   
      TST_ASSERT_EQUALS("A1", mm->lower_bound('0')->second); // special case - before first key
      TST_ASSERT_EQUALS("A1", mm->lower_bound('a')->second); // special case - first key
      TST_ASSERT_EQUALS("B1", mm->lower_bound('b')->second);
      TST_ASSERT_EQUALS("C1", mm->lower_bound('c')->second);
      TST_ASSERT_EQUALS("D1", mm->lower_bound('d')->second);
      TST_ASSERT_EQUALS("E1", mm->lower_bound('e')->second);
      TST_ASSERT_EQUALS("X1", mm->lower_bound('f')->second);
      TST_ASSERT_EQUALS("X1", mm->lower_bound('w')->second);
      TST_ASSERT_EQUALS("X1", mm->lower_bound('x')->second);
      TST_ASSERT( mm->end() == mm->lower_bound('y') );
   
      TST_ASSERT_EQUALS("B1", mm->upper_bound('a')->second);
      TST_ASSERT_EQUALS("C1", mm->upper_bound('b')->second);
      TST_ASSERT_EQUALS("D1", mm->upper_bound('c')->second);
      TST_ASSERT_EQUALS("E1", mm->upper_bound('d')->second);
      TST_ASSERT_EQUALS("X1", mm->upper_bound('e')->second);
      TST_ASSERT_EQUALS("X1", mm->upper_bound('f')->second);
      TST_ASSERT_EQUALS("X1", mm->upper_bound('w')->second);
      TST_ASSERT( mm->end() == mm->upper_bound('x') );
      TST_ASSERT( mm->end() == mm->upper_bound('y') );
   }

   // test const lower_bound and upper_bound
   {
      const COLmap<char,COLstring>* mm = &myMultimap;
      COLmap<char,COLstring>::const_iterator it, lo, up;
      lo = mm->lower_bound('b'); // lo points to b
      up = mm->upper_bound('d'); // up points to e (not d)
   
      it = lo;
      TST_ASSERT(it != up);
      TST_ASSERT_EQUALS('b', it->first);
      TST_ASSERT_EQUALS("B1", it->second);
   
      ++it;
      TST_ASSERT(it != up);
      TST_ASSERT_EQUALS('c', it->first);
      TST_ASSERT_EQUALS("C1", it->second);
   
      ++it;
      TST_ASSERT(it != up);
      TST_ASSERT_EQUALS('c', it->first);
      TST_ASSERT_EQUALS("C2", it->second);
   
      ++it;
      TST_ASSERT(it != up);
      TST_ASSERT_EQUALS('c', it->first);
      TST_ASSERT_EQUALS("C3", it->second);
   
      ++it;
      TST_ASSERT(it != up);
      TST_ASSERT_EQUALS('c', it->first);
      TST_ASSERT_EQUALS("C4", it->second);
   
      ++it;
      TST_ASSERT(it != up);
      TST_ASSERT_EQUALS('c', it->first);
      TST_ASSERT_EQUALS("C5", it->second);
   
      ++it;
      TST_ASSERT(it != up);
      TST_ASSERT_EQUALS('d', it->first);
      TST_ASSERT_EQUALS("D1", it->second);
   
      ++it;
      TST_ASSERT(it == up);
      TST_ASSERT(it != mm->end());
   
      TST_ASSERT_EQUALS("A1", mm->lower_bound('0')->second); // special case - before first key
      TST_ASSERT_EQUALS("A1", mm->lower_bound('a')->second); // special case - first key
      TST_ASSERT_EQUALS("B1", mm->lower_bound('b')->second);
      TST_ASSERT_EQUALS("C1", mm->lower_bound('c')->second);
      TST_ASSERT_EQUALS("D1", mm->lower_bound('d')->second);
      TST_ASSERT_EQUALS("E1", mm->lower_bound('e')->second);
      TST_ASSERT_EQUALS("X1", mm->lower_bound('f')->second);
      TST_ASSERT_EQUALS("X1", mm->lower_bound('w')->second);
      TST_ASSERT_EQUALS("X1", mm->lower_bound('x')->second);
      TST_ASSERT( mm->end() == mm->lower_bound('y') );
   
      TST_ASSERT_EQUALS("B1", mm->upper_bound('a')->second);
      TST_ASSERT_EQUALS("C1", mm->upper_bound('b')->second);
      TST_ASSERT_EQUALS("D1", mm->upper_bound('c')->second);
      TST_ASSERT_EQUALS("E1", mm->upper_bound('d')->second);
      TST_ASSERT_EQUALS("X1", mm->upper_bound('e')->second);
      TST_ASSERT_EQUALS("X1", mm->upper_bound('f')->second);
      TST_ASSERT_EQUALS("X1", mm->upper_bound('w')->second);
      TST_ASSERT( mm->end() == mm->upper_bound('x') );
      TST_ASSERT( mm->end() == mm->upper_bound('y') );
   }

   // test const equal_range
   {
      const COLmap<char,COLstring>* const_mm = &myMultimap;

      // Test non-existant element whose key is less than the first key.
      // Unexpected perhaps, but this is actually correct
      TST_ASSERT_EQUALS( 'a', const_mm->equal_range('0')->first->first );
      TST_ASSERT_EQUALS("A1", const_mm->equal_range('0')->first->second );
      TST_ASSERT_EQUALS( 'a', const_mm->equal_range('0')->second->first );
      TST_ASSERT_EQUALS("A1", const_mm->equal_range('0')->second->second );

      TST_ASSERT_EQUALS( 'a', const_mm->equal_range('a')->first->first );
      TST_ASSERT_EQUALS("A1", const_mm->equal_range('a')->first->second );
      TST_ASSERT_EQUALS( 'b', const_mm->equal_range('a')->second->first );
      TST_ASSERT_EQUALS("B1", const_mm->equal_range('a')->second->second );

      TST_ASSERT_EQUALS( 'b', const_mm->equal_range('b')->first->first );
      TST_ASSERT_EQUALS("B1", const_mm->equal_range('b')->first->second );
      TST_ASSERT_EQUALS( 'c', const_mm->equal_range('b')->second->first );
      TST_ASSERT_EQUALS("C1", const_mm->equal_range('b')->second->second );

      TST_ASSERT_EQUALS( 'c', const_mm->equal_range('c')->first->first );
      TST_ASSERT_EQUALS("C1", const_mm->equal_range('c')->first->second );
      TST_ASSERT_EQUALS( 'd', const_mm->equal_range('c')->second->first );
      TST_ASSERT_EQUALS("D1", const_mm->equal_range('c')->second->second );

      // unexpected perhaps, but this is actually correct
      TST_ASSERT_EQUALS( 'x', const_mm->equal_range('t')->first->first );
      TST_ASSERT_EQUALS("X1", const_mm->equal_range('t')->first->second );
      TST_ASSERT_EQUALS( 'x', const_mm->equal_range('t')->second->first );
      TST_ASSERT_EQUALS("X1", const_mm->equal_range('t')->second->second );

      TST_ASSERT_EQUALS( 'x', const_mm->equal_range('x')->first->first );
      TST_ASSERT_EQUALS("X1", const_mm->equal_range('x')->first->second );
      TST_ASSERT( const_mm->end() == const_mm->equal_range('x')->second );

      // Test non-existant element whose key is greater than the last key.
      TST_ASSERT( const_mm->end() == const_mm->equal_range('z')->first );
      TST_ASSERT( const_mm->end() == const_mm->equal_range('z')->second );

      // test count(Key)
      TST_ASSERT_EQUALS(0, const_mm->count('0'));
      TST_ASSERT_EQUALS(0, const_mm->count('@'));
      TST_ASSERT_EQUALS(1, const_mm->count('a'));
      TST_ASSERT_EQUALS(1, const_mm->count('b'));
      TST_ASSERT_EQUALS(5, const_mm->count('c'));
      TST_ASSERT_EQUALS(1, const_mm->count('d'));
      TST_ASSERT_EQUALS(1, const_mm->count('e'));
      TST_ASSERT_EQUALS(0, const_mm->count('f'));
      TST_ASSERT_EQUALS(0, const_mm->count('t'));
      TST_ASSERT_EQUALS(0, const_mm->count('w'));
      TST_ASSERT_EQUALS(1, const_mm->count('x'));
      TST_ASSERT_EQUALS(0, const_mm->count('y'));
      TST_ASSERT_EQUALS(0, const_mm->count('z'));
   }

   // test non-const equal_range
   {
      COLmap<char,COLstring>* non_const_mm = &myMultimap;

      // Test non-existant element whose key is less than the first key.
      // Unexpected perhaps, but this is actually correct
      TST_ASSERT_EQUALS( 'a', non_const_mm->equal_range('0')->first->first );
      TST_ASSERT_EQUALS("A1", non_const_mm->equal_range('0')->first->second );
      TST_ASSERT_EQUALS( 'a', non_const_mm->equal_range('0')->second->first );
      TST_ASSERT_EQUALS("A1", non_const_mm->equal_range('0')->second->second );

      TST_ASSERT_EQUALS( 'a', non_const_mm->equal_range('a')->first->first );
      TST_ASSERT_EQUALS("A1", non_const_mm->equal_range('a')->first->second );
      TST_ASSERT_EQUALS( 'b', non_const_mm->equal_range('a')->second->first );
      TST_ASSERT_EQUALS("B1", non_const_mm->equal_range('a')->second->second );

      TST_ASSERT_EQUALS( 'b', non_const_mm->equal_range('b')->first->first );
      TST_ASSERT_EQUALS("B1", non_const_mm->equal_range('b')->first->second );
      TST_ASSERT_EQUALS( 'c', non_const_mm->equal_range('b')->second->first );
      TST_ASSERT_EQUALS("C1", non_const_mm->equal_range('b')->second->second );

      TST_ASSERT_EQUALS( 'c', non_const_mm->equal_range('c')->first->first );
      TST_ASSERT_EQUALS("C1", non_const_mm->equal_range('c')->first->second );
      TST_ASSERT_EQUALS( 'd', non_const_mm->equal_range('c')->second->first );
      TST_ASSERT_EQUALS("D1", non_const_mm->equal_range('c')->second->second );

      // unexpected perhaps, but this is actually correct
      TST_ASSERT_EQUALS( 'x', non_const_mm->equal_range('t')->first->first );
      TST_ASSERT_EQUALS("X1", non_const_mm->equal_range('t')->first->second );
      TST_ASSERT_EQUALS( 'x', non_const_mm->equal_range('t')->second->first );
      TST_ASSERT_EQUALS("X1", non_const_mm->equal_range('t')->second->second );

      TST_ASSERT_EQUALS( 'x', non_const_mm->equal_range('x')->first->first );
      TST_ASSERT_EQUALS("X1", non_const_mm->equal_range('x')->first->second );
      TST_ASSERT( non_const_mm->end() == non_const_mm->equal_range('x')->second );

      // Test non-existant element whose key is greater than the last key.
      TST_ASSERT( non_const_mm->end() == non_const_mm->equal_range('z')->first );
      TST_ASSERT( non_const_mm->end() == non_const_mm->equal_range('z')->second );
   }
}

TSTtestable* testCOLmap::COLmapTests()
{
   TST_START_FIXTURE(testCOLmap)
      TST_ADD_TEST_METHOD(initTest);
      TST_ADD_TEST_METHOD(addTest);
      TST_ADD_TEST_METHOD(stableAddTest);
      TST_ADD_TEST_METHOD(addUniqueTest);
      TST_ADD_TEST_METHOD(removeTest);
      TST_ADD_TEST_METHOD(removeKeyTest);
      TST_ADD_TEST_METHOD(clearTest);
      TST_ADD_TEST_METHOD(walkingTest);
      TST_ADD_TEST_METHOD(findTest);
      TST_ADD_TEST_METHOD(iterateAndDeleteTest);
      TST_ADD_TEST_METHOD(randomTest);
      TST_ADD_TEST_FUNCTION(testFindFirst);
      TST_ADD_TEST_FUNCTION(copyTest);
      TST_ADD_TEST_FUNCTION(testStlMethods);
      TST_ADD_TEST_FUNCTION(testEraseWithCOLstringKey);
      TST_ADD_TEST_FUNCTION(testMultimapMethods);
   TST_END_FIXTURE
}

