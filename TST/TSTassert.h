#ifndef __TST_ASSERT_H__
#define __TST_ASSERT_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2006 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TSTassert
//
// Description:
//
// Typed and reusable assertions. Prints nice expected/actual messages.
//
// Author: Eva Kwan
// Date:   Monday, October 30th, 2006 @ 01:10:42 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

class TSTassertPrivate;

class TSTassert 
{
public:
   TSTassert();
   // This is non virtual because we are assuming no classes will inherit from this class
   ~TSTassert();

   static void assertTrue(bool Expression);
   static void assertTrue(bool Expression,  const COLstring& Message);

   static void assertEquals(const COLstring& Expected, const COLstring& Actual);
   static void assertEquals(const COLstring& Expected, const COLstring& Actual, const COLstring& Message);

   static void assertContains(const COLstring& ExpectedSubstring, const COLstring& ActualString);
   static void assertContains(const COLstring& ExpectedSubstring, const COLstring& ActualString, const COLstring& Message);

private:
   TSTassertPrivate* pMember;
   TSTassert(const TSTassert& Orig); // not allowed
   TSTassert& operator=(const TSTassert& Orig); // not allowed

   static COLstring createCompleteMessage(const COLstring& Expected, const COLstring& Actual, const COLstring& Message);
};

#endif // end of defensive include
