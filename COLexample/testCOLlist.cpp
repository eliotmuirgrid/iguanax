//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testCOLlist.cpp
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

#include <COL/COLlog.h>
COL_LOG_MODULE;

#include "testCOLlist.h"

void testCOLlist::testCompare()
{
   TestCount++;

   TST_ASSERT_EQUALS(!!!TestList.size(), TestList.empty());

   TST_ASSERT_MESSAGE (TestList.size() == Control.size(), TestCount << " test failed with an incorrect size");

   // old style forward iteration test
   COLlistPlace Place = TestList.first();
   int ItemIndex = 0;
   for (ItemIndex = 0; ItemIndex < Control.size(); ItemIndex++)
   {
      TST_ASSERT_MESSAGE(TestList[Place] == Control[ItemIndex], TestCount << " forward test failed with incorrect data at index " << ItemIndex );
      Place = TestList.next(Place);
   }

   // old style backward iteration test
   Place = TestList.last();
   for (ItemIndex = Control.size()-1; ItemIndex >= 0; ItemIndex--)
   {
      TST_ASSERT_MESSAGE(TestList[Place] == Control[ItemIndex], TestCount << " reverse test failed with incorrect data at index " << ItemIndex );
      Place = TestList.previous(Place);
   }

   // STL style forward iterator
   int i = 0;
   COLlist<testCOLlist::TestType>::iterator fi = TestList.begin();
   for (; fi != TestList.end(); ++fi) {
      TST_ASSERT_MESSAGE(*fi == Control[i],
         TestCount << " forward iterator test failed with incorrect data at index " << i );
      TST_ASSERT_EQUALS(&*fi, fi.operator->());
      *fi = *fi;
      i++;
   }
   TST_ASSERT_EQUALS(i, TestList.size());

   // STL style reverse iterator
   i = TestList.size() - 1;
   COLlist<testCOLlist::TestType>::reverse_iterator ri = TestList.rbegin();
   for (; ri != TestList.rend(); ++ri) {
      TST_ASSERT_MESSAGE(*ri == Control[i],
         TestCount << " reverse iterator test failed with incorrect data at index " << i );
      TST_ASSERT_EQUALS(&*ri, ri.operator->());
      *ri = *ri;
      --i;
   }
   TST_ASSERT_EQUALS(-1, i);

   const COLlist<TestType>& ConstTestList = TestList;

   // STL style const forward iterator
   i = 0;
   COLlist<testCOLlist::TestType>::const_iterator cfi = ConstTestList.begin();
   for (; cfi != ConstTestList.end(); ++cfi) {
      TST_ASSERT_MESSAGE(*cfi == Control[i],
         TestCount << " const forward iterator test failed with incorrect data at index " << i );
      TST_ASSERT_EQUALS(&*cfi, cfi.operator->());
      i++;
   }
   TST_ASSERT_EQUALS(i, ConstTestList.size());

   // STL style const reverse iterator
   i = ConstTestList.size() - 1;
   COLlist<testCOLlist::TestType>::const_reverse_iterator cri = ConstTestList.rbegin();
   for (; cri != ConstTestList.rend(); ++cri) {
      TST_ASSERT_MESSAGE(*cri == Control[i],
         TestCount << " const reverse iterator test failed with incorrect data at index " << i );
      TST_ASSERT_EQUALS(&*cri, cri.operator->());
      --i;
   }
   TST_ASSERT_EQUALS(-1, i);
}

void testCOLlist::initTest()
{
   Sequence = 0;
   testCompare();
}


void testCOLlist::addTest()
{
   // Add

   for (int ListIndex = 0; ListIndex < 10; ListIndex++)
   {
      Sequence++;
      TestList.add(Sequence);
      Control.push_back(Sequence);
   }
   testCompare();
}


void testCOLlist::insertTest()
{
   // Insert

   Sequence++;
   TestList.insert(Sequence, TestList.first());
   Control.insert(Sequence, 0);
   testCompare();

   Sequence++;
   TestList.insert(Sequence, TestList.next(TestList.first()));
   Control.insert(Sequence, 1);
   testCompare();

   Sequence++;
   TestList.insert(Sequence, TestList.next(TestList.next(TestList.first())));
   Control.insert(Sequence, 2);
   testCompare();

   Sequence++;
   TestList.insert(Sequence, TestList.last());
   Control.insert(Sequence, Control.size() - 1);
   testCompare();

   Sequence++;
   TestList.insert(Sequence, 0);
   Control.insert(Sequence, Control.size());
   testCompare();
}


void testCOLlist::removeTest()
{
   // Remove

   TestList.remove(TestList.first());
   Control.remove(0);
   testCompare();

   TestList.remove(TestList.next(TestList.first()));
   Control.remove(1);
   testCompare();

   TestList.remove(TestList.next(TestList.next(TestList.first())));
   Control.remove(2);
   testCompare();

   TestList.remove(TestList.previous(TestList.last()));
   Control.remove(Control.size() - 2);
   testCompare();

   TestList.remove(TestList.last());
   Control.remove(Control.size() - 1);
   testCompare();

   while (Control.size() > 0)
   {
      TestList.remove(TestList.first());
      Control.remove(0);
      testCompare();
   }
}

void testCOLlist::clearTest()
{
   // Clear

   for (int ListIndex = 0; ListIndex < 10; ListIndex++)
   {
      Sequence++;
      TestList.insert(Sequence, TestList.first());
      Control.insert(Sequence, 0);
   }
   testCompare();

   TestList.clear();
   Control.clear();
   testCompare();
}


void testCOLlist::exchangeTest()
{
   // Exchange

   for (int ListIndex = 0; ListIndex < 10; ListIndex++)
   {
      Sequence++;
      TestList.insert(Sequence, TestList.first());
      Control.insert(Sequence, 0);
   }
   testCompare();

   TestList.exchange(TestList.first(), TestList.first());
   testCompare();

   TestList.exchange(TestList.last(), TestList.last());
   testCompare();

   TestList.exchange(TestList.next(TestList.first()), TestList.next(TestList.first()));
   testCompare();

   TestList.exchange(TestList.first(), TestList.last());
   Sequence = Control[0];
   Control[0] = Control[Control.size() - 1];
   Control[Control.size() - 1] = Sequence;
   testCompare();

   TestList.exchange(TestList.first(), TestList.previous(TestList.last()));
   Sequence = Control[0];
   Control[0] = Control[Control.size() - 2];
   Control[Control.size() - 2] = Sequence;
   testCompare();

   TestList.exchange(TestList.next(TestList.first()), TestList.last());
   Sequence = Control[1];
   Control[1] = Control[Control.size() - 1];
   Control[Control.size() - 1] = Sequence;
   testCompare();

   TestList.exchange(TestList.next(TestList.first()), TestList.previous(TestList.last()));
   Sequence = Control[1];
   Control[1] = Control[Control.size() - 2];
   Control[Control.size() - 2] = Sequence;
   testCompare();

   TestList.exchange(TestList.first(), TestList.next(TestList.first()));
   Sequence = Control[0];
   Control[0] = Control[1];
   Control[1] = Sequence;
   testCompare();

   TestList.exchange(TestList.previous(TestList.last()), TestList.last());
   Sequence = Control[Control.size() - 2];
   Control[Control.size() - 2] = Control[Control.size() - 1];
   Control[Control.size() - 1] = Sequence;
   testCompare();

   TestList.exchange(TestList.next(TestList.first()), TestList.next(TestList.next(TestList.first())));
   Sequence = Control[1];
   Control[1] = Control[2];
   Control[2] = Sequence;
   testCompare();
}

static void testCOLlistSTLmethods()
{
   // construct a COLlist, verify it is empty
   COLlist<COLstring> List;
   TST_ASSERT_EQUALS(0, List.size());
   TST_ASSERT_EQUALS(true, List.empty());

   {
      // test iterator on empty COLlist
      COLlist<COLstring>::iterator emptyListIter = List.begin();
      TST_ASSERT(List.begin() == emptyListIter);
      TST_ASSERT(emptyListIter == List.begin());
   }

   // test push_front() on empty COLlist

   List.push_front("world");
   TST_ASSERT_EQUALS(1, List.size());
   TST_ASSERT_EQUALS(false, List.empty());
   TST_ASSERT_EQUALS("world", List.front());
   TST_ASSERT_EQUALS("world", List.back());

   // test push_front() and pop_front()

   List.push_front("big");
   TST_ASSERT_EQUALS(2, List.size());
   TST_ASSERT_EQUALS(false, List.empty());
   TST_ASSERT_EQUALS("big", List.front());
   TST_ASSERT_EQUALS("world", List.back());

   List.push_front("Hello");
   TST_ASSERT_EQUALS(3, List.size());
   TST_ASSERT_EQUALS(false, List.empty());
   TST_ASSERT_EQUALS("Hello", List.front());
   TST_ASSERT_EQUALS("world", List.back());

   List.pop_front();
   TST_ASSERT_EQUALS(2, List.size());
   TST_ASSERT_EQUALS(false, List.empty());
   TST_ASSERT_EQUALS("big", List.front());
   TST_ASSERT_EQUALS("world", List.back());

   List.pop_front();
   TST_ASSERT_EQUALS(1, List.size());
   TST_ASSERT_EQUALS(false, List.empty());
   TST_ASSERT_EQUALS("world", List.front());
   TST_ASSERT_EQUALS("world", List.back());

   List.pop_front();
   TST_ASSERT_EQUALS(0, List.size());
   TST_ASSERT_EQUALS(true, List.empty());

   // test push_back() on empty COLlist

   List.push_back("something");
   TST_ASSERT_EQUALS(1, List.size());
   TST_ASSERT_EQUALS(false, List.empty());
   TST_ASSERT_EQUALS("something", List.front());
   TST_ASSERT_EQUALS("something", List.back());

   List.push_back("the");
   TST_ASSERT_EQUALS(2, List.size());
   TST_ASSERT_EQUALS(false, List.empty());
   TST_ASSERT_EQUALS("something", List.front());
   TST_ASSERT_EQUALS("the", List.back());

   // test push_back(), pop_front() and pop_back()

   List.pop_front();
   TST_ASSERT_EQUALS(1, List.size());
   TST_ASSERT_EQUALS(false, List.empty());
   TST_ASSERT_EQUALS("the", List.front());
   TST_ASSERT_EQUALS("the", List.back());

   List.push_back("quick");
   TST_ASSERT_EQUALS(2, List.size());
   TST_ASSERT_EQUALS(false, List.empty());
   TST_ASSERT_EQUALS("the", List.front());
   TST_ASSERT_EQUALS("quick", List.back());

   List.pop_back();
   TST_ASSERT_EQUALS(1, List.size());
   TST_ASSERT_EQUALS(false, List.empty());
   TST_ASSERT_EQUALS("the", List.front());
   TST_ASSERT_EQUALS("the", List.back());

   List.pop_back();
   TST_ASSERT_EQUALS(0, List.size());
   TST_ASSERT_EQUALS(true, List.empty());

   List.push_back("The");
   TST_ASSERT_EQUALS(1, List.size());
   TST_ASSERT_EQUALS(false, List.empty());
   TST_ASSERT_EQUALS("The", List.front());
   TST_ASSERT_EQUALS("The", List.back());
   TST_ASSERT_EQUALS(List.front(), List.back());
   TST_ASSERT_EQUALS(&List.front(), &List.back());

   COLlist<COLstring>::iterator it = List.insert(List.end(), COLstring("slow"));
   TST_ASSERT_EQUALS(2, List.size());
   TST_ASSERT_EQUALS(false, List.empty());
   TST_ASSERT_EQUALS("The", List.front());
   TST_ASSERT_EQUALS("slow", List.back());
   TST_ASSERT_EQUALS("slow", *it);

   it = List.insert(it, COLstring("very"));
   TST_ASSERT_EQUALS(3, List.size());
   TST_ASSERT_EQUALS(false, List.empty());
   TST_ASSERT_EQUALS("The", List.front());
   TST_ASSERT_EQUALS("slow", List.back());
   TST_ASSERT_EQUALS("very", *it);

   it = List.erase(it);
   TST_ASSERT_EQUALS(2, List.size());
   TST_ASSERT_EQUALS(false, List.empty());
   TST_ASSERT_EQUALS("The", List.front());
   TST_ASSERT_EQUALS("slow", List.back());
   TST_ASSERT_EQUALS("slow", *it);

   List.push_back("brown");
   TST_ASSERT_EQUALS(3, List.size());
   TST_ASSERT_EQUALS(false, List.empty());
   TST_ASSERT_EQUALS("The", List.front());
   TST_ASSERT_EQUALS("brown", List.back());

   List.pop_front();
   TST_ASSERT_EQUALS(2, List.size());
   TST_ASSERT_EQUALS(false, List.empty());
   TST_ASSERT_EQUALS("slow", List.front());
   TST_ASSERT_EQUALS("brown", List.back());

   List.push_back("fox");
   TST_ASSERT_EQUALS(3, List.size());
   TST_ASSERT_EQUALS(false, List.empty());
   TST_ASSERT_EQUALS("slow", List.front());
   TST_ASSERT_EQUALS("fox", List.back());

   List.push_front("THE");
   TST_ASSERT_EQUALS(4, List.size());
   TST_ASSERT_EQUALS(false, List.empty());
   TST_ASSERT_EQUALS("THE", List.front());
   TST_ASSERT_EQUALS("fox", List.back());

   // test the (forward) iterator and (forward) const_iterator

   COLlist<COLstring>::iterator fi = List.begin();
   TST_ASSERT(List.begin() == fi);
   TST_ASSERT(fi == List.begin());
   TST_ASSERT("THE" == *fi);

   COLlist<COLstring>::const_iterator cfi = List.begin();
   //TST_ASSERT(List.begin() == cfi);
   TST_ASSERT(cfi == List.begin());
   TST_ASSERT("THE" == *cfi);
   TST_ASSERT(List.begin() == fi);
   TST_ASSERT(List.end() != fi);

   COLstring obj = *++fi;
   COLstring cobj = *++cfi;
   TST_ASSERT_EQUALS("slow", obj);
   TST_ASSERT_EQUALS("slow", cobj);
   TST_ASSERT("slow" == *fi);
   TST_ASSERT("slow" == *cfi);
   TST_ASSERT(List.begin() != fi);
   TST_ASSERT(List.end() != fi);

   obj  = *fi++;
   cobj = *cfi++;
   TST_ASSERT_EQUALS("slow", obj);
   TST_ASSERT_EQUALS("slow", cobj);
   TST_ASSERT("brown" == *fi);
   TST_ASSERT("brown" == *cfi);
   TST_ASSERT(List.begin() != fi);
   TST_ASSERT(List.end() != fi);

   obj  = *fi--;
   cobj = *cfi--;
   TST_ASSERT_EQUALS("brown", obj);
   TST_ASSERT_EQUALS("brown", cobj);
   TST_ASSERT("slow" == *fi);
   TST_ASSERT("slow" == *cfi);
   TST_ASSERT(List.begin() != fi);
   TST_ASSERT(List.end() != fi);

   ++fi;
   ++cfi;
   TST_ASSERT("brown" == *fi);
   TST_ASSERT("brown" == *cfi);
   TST_ASSERT(List.begin() != fi);
   TST_ASSERT(List.end() != fi);

   fi++;
   cfi++;
   TST_ASSERT("fox" == *fi);
   TST_ASSERT("fox" == *cfi);
   TST_ASSERT(List.begin() != fi);
   TST_ASSERT(List.end() != fi);

   ++fi;
   ++cfi;
   TST_ASSERT(List.end() == fi);
   TST_ASSERT(fi == List.end());
   //TST_ASSERT(List.end() == cfi);
   TST_ASSERT(cfi == List.end());
   TST_ASSERT(List.cend() == cfi);
   TST_ASSERT(cfi == List.cend());
   TST_ASSERT(List.begin() != fi);

   TST_ASSERT_EQUALS("THE", List.front());
   TST_ASSERT_EQUALS("fox", List.back());

   // test the reverse_iterator and const_reverse_iterator

   COLlist<COLstring>::reverse_iterator ri = List.rbegin();
   COLlist<COLstring>::const_reverse_iterator cri = List.crbegin();
   TST_ASSERT(cri == List.rbegin());
   TST_ASSERT(cri == ri);
   TST_ASSERT("fox" == *ri);
   TST_ASSERT("fox" == *cri);

   obj  = *++ri;
   cobj = *++cri;
   TST_ASSERT_EQUALS("brown", obj);
   TST_ASSERT_EQUALS("brown", cobj);
   TST_ASSERT("brown" == *ri);
   TST_ASSERT("brown" == *cri);

   obj  = *ri++;
   cobj = *cri++;
   TST_ASSERT_EQUALS("brown", obj);
   TST_ASSERT_EQUALS("brown", cobj);
   TST_ASSERT("slow" == *ri);
   TST_ASSERT("slow" == *cri);

   obj  = *ri--;
   cobj = *cri--;
   TST_ASSERT_EQUALS("slow", obj);
   TST_ASSERT_EQUALS("slow", cobj);
   TST_ASSERT("brown" == *ri);
   TST_ASSERT("brown" == *cri);

   obj  = *--ri;
   cobj = *--cri;
   TST_ASSERT_EQUALS("fox", obj);
   TST_ASSERT_EQUALS("fox", cobj);
   TST_ASSERT("fox" == *ri);
   TST_ASSERT("fox" == *cri);

   obj  = *++ri;
   cobj = *++cri;
   TST_ASSERT_EQUALS("brown", obj);
   TST_ASSERT_EQUALS("brown", cobj);
   TST_ASSERT("brown" == *ri);
   TST_ASSERT("brown" == *cri);

   ++ri;
   ++cri;
   TST_ASSERT("slow" == *ri);
   TST_ASSERT("slow" == *cri);

   ri++;
   cri++;
   TST_ASSERT("THE" == *ri);
   TST_ASSERT("THE" == *cri);

   ++ri;
   ++cri;
   TST_ASSERT(List.rend() == ri);
   TST_ASSERT(ri == List.rend());
   //TST_ASSERT(List.rend() == cri);
   TST_ASSERT(cri == List.rend());
   TST_ASSERT(List.crend() == cri);
   TST_ASSERT(cri == List.crend());
   TST_ASSERT("THE" == *List.begin());
   TST_ASSERT("THE" == *List.cbegin());
   TST_ASSERT("fox" == *List.rbegin());
   TST_ASSERT("fox" == *List.crbegin());

   // test insert at the beginning of the list
   it = List.insert(List.begin(), "not");
   TST_ASSERT_EQUALS("not", List.front());
   TST_ASSERT_EQUALS("fox", List.back());
   TST_ASSERT_EQUALS("not", *it);
   TST_ASSERT_EQUALS(5, List.size());

   // test insert at the beginning of the list
   it = List.insert(List.begin(), "Absolutely");
   TST_ASSERT_EQUALS("Absolutely", List.front());
   TST_ASSERT_EQUALS("fox", List.back());
   TST_ASSERT_EQUALS("Absolutely", *it);
   TST_ASSERT_EQUALS(6, List.size());

   // test erasing first element
   it = List.erase(it);
   TST_ASSERT(it == List.begin());
   TST_ASSERT_EQUALS("not", List.front());
   TST_ASSERT_EQUALS("fox", List.back());
   TST_ASSERT_EQUALS("not", *it);
   TST_ASSERT_EQUALS(5, List.size());

   // test erasing first element
   it = List.erase(it);
   TST_ASSERT(it == List.begin());
   TST_ASSERT_EQUALS("THE", List.front());
   TST_ASSERT_EQUALS("fox", List.back());
   TST_ASSERT_EQUALS("THE", *it);
   TST_ASSERT_EQUALS(4, List.size());

   // test erasing first element
   it = List.erase(it);
   TST_ASSERT(it == List.begin());
   TST_ASSERT_EQUALS("slow", List.front());
   TST_ASSERT_EQUALS("fox", List.back());
   TST_ASSERT_EQUALS("slow", *it);
   TST_ASSERT_EQUALS(3, List.size());

   // test erasing first element
   it = List.erase(it);
   TST_ASSERT(it == List.begin());
   TST_ASSERT_EQUALS("brown", List.front());
   TST_ASSERT_EQUALS("fox", List.back());
   TST_ASSERT_EQUALS("brown", *it);
   TST_ASSERT_EQUALS(2, List.size());

   // test erasing first element
   it = List.erase(it);
   TST_ASSERT(it == List.begin());
   TST_ASSERT_EQUALS("fox", List.front());
   TST_ASSERT_EQUALS("fox", List.back());
   TST_ASSERT_EQUALS("fox", *it);
   TST_ASSERT_EQUALS(1, List.size());
   TST_ASSERT_EQUALS(false, List.empty());
      
   // test erasing first element
   it = List.erase(it);
   TST_ASSERT(it == List.begin());
   TST_ASSERT_EQUALS(0, List.size());
   TST_ASSERT_EQUALS(true, List.empty());
}

#include <COL/COLqueue.h>

static void testCOLqueueOriginalMethods()
{
   // construct a COLqueue, verify it is empty
   COLqueue<COLstring> Queue;
   TST_ASSERT_EQUALS(0, Queue.size());
   TST_ASSERT_EQUALS(true, Queue.empty());

   Queue.push_back("something");
   TST_ASSERT_EQUALS(1, Queue.size());
   TST_ASSERT_EQUALS(false, Queue.empty());
   TST_ASSERT_EQUALS("something", Queue.front());
   TST_ASSERT_EQUALS("something", Queue.back());

   Queue.push_back("the");
   TST_ASSERT_EQUALS(2, Queue.size());
   TST_ASSERT_EQUALS(false, Queue.empty());
   TST_ASSERT_EQUALS("something", Queue.front());
   TST_ASSERT_EQUALS("the", Queue.back());

   Queue.pop_front();
   TST_ASSERT_EQUALS(1, Queue.size());
   TST_ASSERT_EQUALS(false, Queue.empty());
   TST_ASSERT_EQUALS("the", Queue.front());
   TST_ASSERT_EQUALS("the", Queue.back());

   Queue.pop_front();
   TST_ASSERT_EQUALS(0, Queue.size());
   TST_ASSERT_EQUALS(true, Queue.empty());

   Queue.push_back() = "quick";
   TST_ASSERT_EQUALS(1, Queue.size());
   TST_ASSERT_EQUALS(false, Queue.empty());
   TST_ASSERT_EQUALS("quick", Queue.front());
   TST_ASSERT_EQUALS("quick", Queue.back());

   Queue.push_back() = "The";
   TST_ASSERT_EQUALS(2, Queue.size());
   TST_ASSERT_EQUALS(false, Queue.empty());
   TST_ASSERT_EQUALS("quick", Queue.front());
   TST_ASSERT_EQUALS("The", Queue.back());

   Queue.push_back("brown");
   TST_ASSERT_EQUALS(3, Queue.size());
   TST_ASSERT_EQUALS(false, Queue.empty());
   TST_ASSERT_EQUALS("quick", Queue.front());
   TST_ASSERT_EQUALS("brown", Queue.back());

   Queue.pop_front();
   TST_ASSERT_EQUALS(2, Queue.size());
   TST_ASSERT_EQUALS(false, Queue.empty());
   TST_ASSERT_EQUALS("The", Queue.front());
   TST_ASSERT_EQUALS("brown", Queue.back());

   Queue.pop_front();
   TST_ASSERT_EQUALS(1, Queue.size());
   TST_ASSERT_EQUALS(false, Queue.empty());
   TST_ASSERT_EQUALS("brown", Queue.front());
   TST_ASSERT_EQUALS("brown", Queue.back());

   Queue.pop_front();
   TST_ASSERT_EQUALS(0, Queue.size());
   TST_ASSERT_EQUALS(true, Queue.empty());

   Queue.push_back() = "quick";
   TST_ASSERT_EQUALS(1, Queue.size());
   TST_ASSERT_EQUALS(false, Queue.empty());
   TST_ASSERT_EQUALS("quick", Queue.front());
   TST_ASSERT_EQUALS("quick", Queue.back());

   Queue.push_back() = "The";
   TST_ASSERT_EQUALS(2, Queue.size());
   TST_ASSERT_EQUALS(false, Queue.empty());
   TST_ASSERT_EQUALS("quick", Queue.front());
   TST_ASSERT_EQUALS("The", Queue.back());

   Queue.clear();
   TST_ASSERT_EQUALS(0, Queue.size());
   TST_ASSERT_EQUALS(true, Queue.empty());
}

static void testCOLqueueSTLmethods()
{
   // construct a COLqueue, verify it is empty
   COLqueue<COLstring> Queue;
   TST_ASSERT_EQUALS(0, Queue.size());
   TST_ASSERT_EQUALS(true, Queue.empty());

   Queue.push("something");
   TST_ASSERT_EQUALS(1, Queue.size());
   TST_ASSERT_EQUALS(false, Queue.empty());
   TST_ASSERT_EQUALS("something", Queue.front());
   TST_ASSERT_EQUALS("something", Queue.back());

   Queue.push("the");
   TST_ASSERT_EQUALS(2, Queue.size());
   TST_ASSERT_EQUALS(false, Queue.empty());
   TST_ASSERT_EQUALS("something", Queue.front());
   TST_ASSERT_EQUALS("the", Queue.back());

   Queue.pop();
   TST_ASSERT_EQUALS(1, Queue.size());
   TST_ASSERT_EQUALS(false, Queue.empty());
   TST_ASSERT_EQUALS("the", Queue.front());
   TST_ASSERT_EQUALS("the", Queue.back());

   Queue.pop();
   TST_ASSERT_EQUALS(0, Queue.size());
   TST_ASSERT_EQUALS(true, Queue.empty());

   {
      COLstring& Element = Queue.push();
      Element = "quick";
   }
   TST_ASSERT_EQUALS(1, Queue.size());
   TST_ASSERT_EQUALS(false, Queue.empty());
   TST_ASSERT_EQUALS("quick", Queue.front());
   TST_ASSERT_EQUALS("quick", Queue.back());

   Queue.push("The");
   TST_ASSERT_EQUALS(2, Queue.size());
   TST_ASSERT_EQUALS(false, Queue.empty());
   TST_ASSERT_EQUALS("quick", Queue.front());
   TST_ASSERT_EQUALS("The", Queue.back());

   Queue.push() = "brown";
   TST_ASSERT_EQUALS(3, Queue.size());
   TST_ASSERT_EQUALS(false, Queue.empty());
   TST_ASSERT_EQUALS("quick", Queue.front());
   TST_ASSERT_EQUALS("brown", Queue.back());

   Queue.pop();
   TST_ASSERT_EQUALS(2, Queue.size());
   TST_ASSERT_EQUALS(false, Queue.empty());
   TST_ASSERT_EQUALS("The", Queue.front());
   TST_ASSERT_EQUALS("brown", Queue.back());

   Queue.pop();
   TST_ASSERT_EQUALS(1, Queue.size());
   TST_ASSERT_EQUALS(false, Queue.empty());
   TST_ASSERT_EQUALS("brown", Queue.front());
   TST_ASSERT_EQUALS("brown", Queue.back());

   Queue.pop();
   TST_ASSERT_EQUALS(0, Queue.size());
   TST_ASSERT_EQUALS(true, Queue.empty());

   Queue.push("something");
   TST_ASSERT_EQUALS(1, Queue.size());
   TST_ASSERT_EQUALS(false, Queue.empty());
   TST_ASSERT_EQUALS("something", Queue.front());
   TST_ASSERT_EQUALS("something", Queue.back());

   Queue.push("the");
   TST_ASSERT_EQUALS(2, Queue.size());
   TST_ASSERT_EQUALS(false, Queue.empty());
   TST_ASSERT_EQUALS("something", Queue.front());
   TST_ASSERT_EQUALS("the", Queue.back());

   Queue.clear();
   TST_ASSERT_EQUALS(0, Queue.size());
   TST_ASSERT_EQUALS(true, Queue.empty());
}

#include <COL/COLstack.h>

static void testCOLstack()
{
   // construct a COLstack, verify it is empty
   COLstack<COLstring> Stack;
   TST_ASSERT_EQUALS(0, Stack.size());
   TST_ASSERT_EQUALS(true, Stack.empty());

   Stack.push("something");
   TST_ASSERT_EQUALS(1, Stack.size());
   TST_ASSERT_EQUALS(false, Stack.empty());
   TST_ASSERT_EQUALS("something", Stack.top());

   Stack.push("the");
   TST_ASSERT_EQUALS(2, Stack.size());
   TST_ASSERT_EQUALS(false, Stack.empty());
   TST_ASSERT_EQUALS("the", Stack.top());

   Stack.pop();
   TST_ASSERT_EQUALS(1, Stack.size());
   TST_ASSERT_EQUALS(false, Stack.empty());
   TST_ASSERT_EQUALS("something", Stack.top());

   Stack.pop();
   TST_ASSERT_EQUALS(0, Stack.size());
   TST_ASSERT_EQUALS(true, Stack.empty());

   {
      COLstring& Element = Stack.push();
      Element = "quick";
   }
   TST_ASSERT_EQUALS(1, Stack.size());
   TST_ASSERT_EQUALS(false, Stack.empty());
   TST_ASSERT_EQUALS("quick", Stack.top());

   Stack.push("Notwithstanding");
   TST_ASSERT_EQUALS(2, Stack.size());
   TST_ASSERT_EQUALS(false, Stack.empty());
   TST_ASSERT_EQUALS("Notwithstanding", Stack.top());

   Stack.push() = "brown";
   TST_ASSERT_EQUALS(3, Stack.size());
   TST_ASSERT_EQUALS(false, Stack.empty());
   TST_ASSERT_EQUALS("brown", Stack.top());

   Stack.pop();
   TST_ASSERT_EQUALS(2, Stack.size());
   TST_ASSERT_EQUALS(false, Stack.empty());
   TST_ASSERT_EQUALS("Notwithstanding", Stack.top());

   Stack.pop();
   TST_ASSERT_EQUALS(1, Stack.size());
   TST_ASSERT_EQUALS(false, Stack.empty());
   TST_ASSERT_EQUALS("quick", Stack.top());

   Stack.pop();
   TST_ASSERT_EQUALS(0, Stack.size());
   TST_ASSERT_EQUALS(true, Stack.empty());

   Stack.push("other");
   TST_ASSERT_EQUALS(1, Stack.size());
   TST_ASSERT_EQUALS(false, Stack.empty());
   TST_ASSERT_EQUALS("other", Stack.top());

   Stack.push("okay");
   TST_ASSERT_EQUALS(2, Stack.size());
   TST_ASSERT_EQUALS(false, Stack.empty());
   TST_ASSERT_EQUALS("okay", Stack.top());

   Stack.clear();
   TST_ASSERT_EQUALS(0, Stack.size());
   TST_ASSERT_EQUALS(true, Stack.empty());
}

#include <COL/COLthreadQueue.h>

// not a comprehensive test - just test the basics
static void testCOLthreadQueueInSingleThread()
{
   COLstring Item;

   // construct a COLqueue, verify it is empty
   COLthreadQueue<COLstring> Queue;
   TST_ASSERT_EQUALS(0, Queue.size());
   TST_ASSERT_EQUALS(true, Queue.empty());

   Queue.put("one");
   TST_ASSERT_EQUALS(1, Queue.size());
   TST_ASSERT_EQUALS(false, Queue.empty());

   Queue.put("Two");
   TST_ASSERT_EQUALS(2, Queue.size());
   TST_ASSERT_EQUALS(false, Queue.empty());

   Item = Queue.take();
   TST_ASSERT_EQUALS("one", Item);
   TST_ASSERT_EQUALS(1, Queue.size());
   TST_ASSERT_EQUALS(false, Queue.empty());

   Item = Queue.take();
   TST_ASSERT_EQUALS("Two", Item);
   TST_ASSERT_EQUALS(0, Queue.size());
   TST_ASSERT_EQUALS(true, Queue.empty());

   Queue.put("quick");
   TST_ASSERT_EQUALS(1, Queue.size());
   TST_ASSERT_EQUALS(false, Queue.empty());

   Queue.put("brown");
   TST_ASSERT_EQUALS(2, Queue.size());
   TST_ASSERT_EQUALS(false, Queue.empty());

   Queue.put("fox");
   TST_ASSERT_EQUALS(3, Queue.size());
   TST_ASSERT_EQUALS(false, Queue.empty());

   Item = Queue.take();
   TST_ASSERT_EQUALS("quick", Item);
   TST_ASSERT_EQUALS(2, Queue.size());
   TST_ASSERT_EQUALS(false, Queue.empty());

   Item = Queue.take();
   TST_ASSERT_EQUALS("brown", Item);
   TST_ASSERT_EQUALS(1, Queue.size());
   TST_ASSERT_EQUALS(false, Queue.empty());

   Queue.put("also");
   TST_ASSERT_EQUALS(2, Queue.size());
   TST_ASSERT_EQUALS(false, Queue.empty());

   Item = Queue.take();
   TST_ASSERT_EQUALS("fox", Item);
   TST_ASSERT_EQUALS(1, Queue.size());
   TST_ASSERT_EQUALS(false, Queue.empty());

   Queue.put("jumped");
   TST_ASSERT_EQUALS(2, Queue.size());
   TST_ASSERT_EQUALS(false, Queue.empty());

   Queue.put("again");
   TST_ASSERT_EQUALS(3, Queue.size());
   TST_ASSERT_EQUALS(false, Queue.empty());

   Item = Queue.take();
   TST_ASSERT_EQUALS("also", Item);
   TST_ASSERT_EQUALS(2, Queue.size());
   TST_ASSERT_EQUALS(false, Queue.empty());

   Item = Queue.take();
   TST_ASSERT_EQUALS("jumped", Item);
   TST_ASSERT_EQUALS(1, Queue.size());
   TST_ASSERT_EQUALS(false, Queue.empty());

   Item = Queue.take();
   TST_ASSERT_EQUALS("again", Item);
   TST_ASSERT_EQUALS(0, Queue.size());
   TST_ASSERT_EQUALS(true, Queue.empty());
}

TSTtestable* testCOLlist::COLlistTests()
{
   TST_START_FIXTURE(testCOLlist)
      TST_ADD_TEST_METHOD(initTest);
      TST_ADD_TEST_METHOD(addTest);
      TST_ADD_TEST_METHOD(insertTest);
      TST_ADD_TEST_METHOD(removeTest);
      TST_ADD_TEST_METHOD(clearTest);
      TST_ADD_TEST_METHOD(exchangeTest);
      TST_ADD_TEST_FUNCTION(testCOLlistSTLmethods);
      TST_ADD_TEST_FUNCTION(testCOLqueueOriginalMethods);
      TST_ADD_TEST_FUNCTION(testCOLqueueSTLmethods);
      TST_ADD_TEST_FUNCTION(testCOLstack);
      TST_ADD_TEST_FUNCTION(testCOLthreadQueueInSingleThread);
   TST_END_FIXTURE
}

