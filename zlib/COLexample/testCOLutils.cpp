//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testCOLmath.cpp
//
// Description:
//
// This is a test of COLutils
//
// Author: Andrew Vajoczki
// Date:   Feb 12, 2009
//
//---------------------------------------------------------------------------

#include "COLexamplePrecomp.h"
#pragma hdrstop

#include <COL/COLutils.h>
#include <COL/COLmath.h>
#include <TST/TSTtestCollection.h>
#include <TST/TSTtestFunction.h>
#include <TST/TSThelpers.h>

//
// Not tests in the traditional sense -
// just time them externally from the commandline using the "time" command.
// There are no hard guarantees about duration for COLsleep() - just best effort.
//

void testCOLsleep50()
{
   COLsleep(50);
}

#ifdef _WIN32
#   define putenv _putenv
#endif

void testCOLexpandEnvironmentVariables()
{
   // these have to leak
   putenv((char*)"VALUE=<-v-a-l-u-e->");
   putenv((char*)"EMPTY=");
   putenv((char*)"LONG=It was the best of times, it was the blurst of times.");
   putenv((char*)"VALUE1=<value1>");
   putenv((char*)"VALUE11=<value11>");
   putenv((char*)"v937=Computer");
   putenv((char*)"_expando_=${VALUE}");

   TST_ASSERT_EQUALS("", COLexpandEnvironmentVariables(""));
   TST_ASSERT_EQUALS("Hello <value1>, Computer and <-v-a-l-u-e->.", COLexpandEnvironmentVariables("Hello ${VALUE1}, ${v937} and ${VALUE}."));
   TST_ASSERT_EQUALS("abc", COLexpandEnvironmentVariables("abc"));
   TST_ASSERT_EQUALS("$", COLexpandEnvironmentVariables("$"));
   TST_ASSERT_EQUALS("${", COLexpandEnvironmentVariables("${"));
   TST_ASSERT_EQUALS("$abc$", COLexpandEnvironmentVariables("$abc$"));
   TST_ASSERT_EQUALS("jk", COLexpandEnvironmentVariables("j${}k")); // ${} should not be defined
   TST_ASSERT_EQUALS(" unfinished ${abc", COLexpandEnvironmentVariables(" unfinished ${abc"));
   TST_ASSERT_EQUALS("<-v-a-l-u-e->", COLexpandEnvironmentVariables("${VALUE}"));
   TST_ASSERT_EQUALS("abc<-v-a-l-u-e->", COLexpandEnvironmentVariables("abc${VALUE}"));
   TST_ASSERT_EQUALS("abc<-v-a-l-u-e->def", COLexpandEnvironmentVariables("abc${VALUE}def"));
   TST_ASSERT_EQUALS("", COLexpandEnvironmentVariables("${EMPTY}"));
   TST_ASSERT_EQUALS("x", COLexpandEnvironmentVariables("x${EMPTY}"));
   TST_ASSERT_EQUALS("y", COLexpandEnvironmentVariables("${EMPTY}y"));
   TST_ASSERT_EQUALS("xy", COLexpandEnvironmentVariables("x${EMPTY}y"));
   TST_ASSERT_EQUALS("ab", COLexpandEnvironmentVariables("a${NOT_DEFINED_ENV_VAR}b"));
   TST_ASSERT_EQUALS("'It was the best of times, it was the blurst of times.'", COLexpandEnvironmentVariables("'${LONG}'"));
   TST_ASSERT_EQUALS("some <value11><value1>!", COLexpandEnvironmentVariables("some ${VALUE11}${VALUE1}!"));

   // demonstrate that it is only single pass
   TST_ASSERT_EQUALS("${VALUE}", COLexpandEnvironmentVariables("${_expando_}"));
   TST_ASSERT_EQUALS("blah blah $${VALUE}}", COLexpandEnvironmentVariables("blah blah $${_expando_}}"));
}

TSTtestable* COLutilsTests()
{
   TST_START_COLLECTION("COLutilsTests")
      TST_ADD_TEST_FUNCTION(testCOLexpandEnvironmentVariables);
      TST_ADD_TEST_FUNCTION(testCOLsleep50);
   TST_END_COLLECTION
}
