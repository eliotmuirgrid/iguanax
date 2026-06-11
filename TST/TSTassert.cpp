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
#include "TSTprecomp.h"
#pragma hdrstop

#include "TSTassert.h"
#include "TSThelpers.h"

class TSTassertPrivate
{
public:

};

TSTassert::TSTassert()
{
   pMember = new TSTassertPrivate;
}

TSTassert::~TSTassert()
{
   delete pMember;
}


//---------------------------------------------------------------------------
void TSTassert::assertTrue(bool Expression)
{
   assertTrue(Expression, "");
}

void TSTassert::assertTrue(bool Expression, const COLstring& Message)
{
   TST_ASSERT_MESSAGE
   (
      Expression,
      Message
   );
}

void TSTassert::assertEquals
(
   const COLstring& Expected, 
   const COLstring& Actual
)
{
   assertEquals(Expected, Actual, "");
}

void TSTassert::assertEquals
(
   const COLstring& Expected, 
   const COLstring& Actual, 
   const COLstring& Message
)
{
   COLstring CompleteMessage = createCompleteMessage(Expected, Actual, Message);

   TST_ASSERT_MESSAGE
   (
      Expected == Actual,
      CompleteMessage
   );
}

void TSTassert::assertContains
(
   const COLstring& ExpectedSubstring, 
   const COLstring& ActualString
)
{
   assertContains(ExpectedSubstring, ActualString, "");
}

void TSTassert::assertContains
(
   const COLstring& ExpectedSubstring, 
   const COLstring& ActualString, 
   const COLstring& Message
)
{
   COLstring CompleteMessage = createCompleteMessage(ExpectedSubstring, ActualString, Message);
   bool SubstringFound = ActualString.find(ExpectedSubstring) != npos;

   TST_ASSERT_MESSAGE
   (
      SubstringFound,
      CompleteMessage
   );
}

COLstring TSTassert::createCompleteMessage
(
   const COLstring& Expected, 
   const COLstring& Actual, 
   const COLstring& Message
)
{
   COLstring CompleteMessage;
   COLostream Stream(CompleteMessage);

   Stream << Message << newline;
   Stream << "Expected:    [" << Expected << "]" << newline;
   Stream << "Actual:      [" << Actual   << "]" << newline;

   return CompleteMessage;
}


