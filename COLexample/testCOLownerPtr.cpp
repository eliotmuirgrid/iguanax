//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testCOLownerPtr.cpp
//
// Description:
//
// This is a test of COLauto
//
// Author: George Ma
// Date:   Mon 03/17/2004 
//
//
//---------------------------------------------------------------------------

#include "COLexamplePrecomp.h"
#pragma hdrstop

#include <COL/COLauto.h>
#include <COL/COLstring.h>
#include <COL/COLarray.h>

void testCOLownerPtr()
{
   COLauto<COLstring> pString = new COLstring();

   TST_ASSERT_MESSAGE(pString.isOwner(),"");

   *pString = "abcdef";

   COLauto<COLstring> pString2(pString);

   TST_ASSERT_MESSAGE(!pString.isOwner(),"COLauto copy constuctor failed");
   TST_ASSERT_MESSAGE(pString2.isOwner(),"COLauto copy constuctor failed");
   TST_ASSERT_MESSAGE(pString2->size() == 6,"COLauto::operator->() failed");

   COLauto<COLstring> pString3;

   pString3 = pString2;

   TST_ASSERT_MESSAGE(!pString2.isOwner(),"COLauto operator=(COLauto&) assignment failed");
   TST_ASSERT_MESSAGE(pString3.isOwner(),"COLauto operator=(COLauto&) assignment failed");   
}

// actually of a valgrind memory leak test of COLarray of COLownerPtrs...
void testCOLownerPtrVector()
{
   COLarray< COLauto<COLstring> > VectorOfHeapCOLstrings;

   VectorOfHeapCOLstrings.push_back(new COLstring("Some test string longer than 16 characters."));
   VectorOfHeapCOLstrings.push_back(new COLstring("The sky is blue."));
   VectorOfHeapCOLstrings.push_back(new COLstring("pear"));
   VectorOfHeapCOLstrings.push_back(new COLstring("Little Dog Monday"));

   TST_ASSERT_MESSAGE(VectorOfHeapCOLstrings.size() == 4,"VectorOfHeapCOLstrings size failed");

   VectorOfHeapCOLstrings.remove(2);

   TST_ASSERT_MESSAGE(VectorOfHeapCOLstrings.size() == 3,"VectorOfHeapCOLstrings size failed");

   VectorOfHeapCOLstrings.push_back(new COLstring("Another string."));
   VectorOfHeapCOLstrings.push_back(new COLstring("Yet another string."));

   TST_ASSERT_MESSAGE(VectorOfHeapCOLstrings.size() == 5,"VectorOfHeapCOLstrings size failed");

   VectorOfHeapCOLstrings.remove(0);
   VectorOfHeapCOLstrings.remove(0);

   TST_ASSERT_MESSAGE(VectorOfHeapCOLstrings.size() == 3,"VectorOfHeapCOLstrings size failed");

   // Cause a resize, and let valgrind see if anything bad happens.
   VectorOfHeapCOLstrings.setCapacity(2048);
}

TSTtestable* COLownerPtrTests()
{
   TST_START_COLLECTION("COLauto Tests")
      TST_ADD_TEST_FUNCTION(testCOLownerPtr);
      TST_ADD_TEST_FUNCTION(testCOLownerPtrVector);
   TST_END_COLLECTION
}
