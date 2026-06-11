//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testCOLerror
//
// Description:
//
// Implementation
//
// Author: Matt Lawrence
// Date:   Wednesday, February 4th, 2009 @ 11:24:24 AM
//
//---------------------------------------------------------------------------
#include "COLexamplePrecomp.h"
#pragma hdrstop

#include <TST/TSThelpers.h>
#include <TST/TSTtestCollection.h>
#include <TST/TSTtestFunction.h>

#ifdef _WIN32 // for SetLastError
   #define VC_EXTRALEAN
   #define WIN32_LEAN_AND_MEAN
   #include <windows.h>
   #include <winbase.h>
#endif // _WIN32

#define COLERROR_EXPANSION_CHECK(assert) \
   { \
      bool Success = false; \
      try \
      { \
         assert; \
      } \
      catch (const COLerror& Error) \
      { \
         TST_ASSERT(COLstring(Error.description()).find("FIVE") != npos); \
         TST_ASSERT(COLstring(Error.description()).find("SEVEN") != npos); \
         Success = true; \
      } \
      TST_ASSERT_MESSAGE(Success, #assert << " didn't throw when it should.");\
   }

static void testPreconditionExpansion()
{
#define FIVE 5
#define SEVEN 7
   COLERROR_EXPANSION_CHECK(COLPRECONDITION(FIVE == SEVEN));
   COLERROR_EXPANSION_CHECK(COLMSGPRECONDITION(FIVE == SEVEN, "foo"));
   COLERROR_EXPANSION_CHECK(COLPOSTCONDITION(FIVE == SEVEN));
   COLERROR_EXPANSION_CHECK(COLMSGPOSTCONDITION(FIVE == SEVEN, "foo"));
}

#ifdef _WIN32
static void testThrowWin32Error()
{
   try
   {
      SetLastError(ERROR_ACCESS_DENIED);
      COLthrowWin32Error(COLlastError());
   }
   catch (const COLerror& Error)
   {
      TST_ASSERT_EQUALS(ERROR_ACCESS_DENIED, Error.code());
   }

   try
   {
      SetLastError(ERROR_ACCESS_DENIED);
      COL_THROW_WIN32_ERROR("");
   }
   catch (const COLerror& Error)
   {
      TST_ASSERT_EQUALS(ERROR_ACCESS_DENIED, Error.code());
   }

   try
   {
      SetLastError(ERROR_ACCESS_DENIED);
      COL_THROW_WIN32_ERROR_PLAIN("");
   }
   catch (const COLerror& Error)
   {
      TST_ASSERT_EQUALS(ERROR_ACCESS_DENIED, Error.code());
   }
}
#endif // _WIN32

TSTtestable* COLerrorTests()
{
   TST_START_COLLECTION("COLerrorTests")
      //TST_ADD_TEST_FUNCTION(testPreconditionExpansion);
#ifdef _WIN32
      TST_ADD_TEST_FUNCTION(testThrowWin32Error);
#endif
   TST_END_FIXTURE
};

