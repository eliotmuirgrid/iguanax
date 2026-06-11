//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2006 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testTSTtestSpecificationParser
//
// Description: Parses a string into a testSpecification
//
// Author: Nasron Cheong
// Date:   March 2004 
//
//---------------------------------------------------------------------------

#include <TSTexample/TSTexamplePrecompiled.h>
#pragma hdrstop

#include <TSTexample/TSTtestClass2.h>
#include <TST/TSTflags.h>

void testTSTspecEquality()
{
   TSTtestSpecification Spec;
   TSTtestSpecification Control;
   
   Spec.pushBackPath("path1");
   Spec.pushBackPath("path2");
   Spec.pushBackPath("path3");

   Control.pushBackPath("path1");
   Control.pushBackPath("path2");
   Control.pushBackPath("path3");

   Spec.testOptions().setRunAll(false);
   Control.testOptions().setRunAll(false);

   Spec.testOptions().addSubTest("subtest1");
   Spec.testOptions().addSubTest("subtest2");
   Spec.testOptions().addSubTest("subtest3");
   Spec.testOptions().addSubTest("subtest4");

   Control.testOptions().addSubTest("subtest1");
   Control.testOptions().addSubTest("subtest2");
   Control.testOptions().addSubTest("subtest3");
   Control.testOptions().addSubTest("subtest4");

   TST_ASSERT(Spec == Control);

   Control.clear();
   Control.pushBackPath("fake1");

   TST_ASSERT(Spec != Control);
}



void testTSTspecParserExpected()
{
   TSTtestSpecification Spec;
   TSTtestSpecification Control;
   TSTtestSpecificationParser Parser;

   Parser.createSpecification("/dir1/dir2/[abc and def]",Spec);
   Control.pushBackPath("dir1");
   Control.pushBackPath("dir2");
   Control.testOptions().addSubTest("abc");
   Control.testOptions().addSubTest("def");
   Control.testOptions().setRunAll(false);
   
   TST_ASSERT(Control == Spec);
   Spec.clear();
   Control.clear();
   
   Parser.createSpecification("/dir1 a  /   dir2  b   /   dir3   c/[abc and def and ghi and jkl]",Spec);   
   Control.pushBackPath("dir1 a");
   Control.pushBackPath("dir2  b");
   Control.pushBackPath("dir3   c");   
   Control.testOptions().addSubTest("abc");
   Control.testOptions().addSubTest("def");
   Control.testOptions().addSubTest("ghi");
   Control.testOptions().addSubTest("jkl");
   Control.testOptions().setRunAll(false);

   TST_ASSERT(Control == Spec);

   Control.clear();
   Spec.clear();
   Parser.createSpecification("dir1/dir2/[abc def and ghi jkl]",Spec);
   Control.pushBackPath("dir1");
   Control.pushBackPath("dir2");
   Control.testOptions().addSubTest("abc def");
   Control.testOptions().addSubTest("ghi jkl");
   Control.testOptions().setRunAll(false);
   TST_ASSERT(Control == Spec);

   Control.clear();
   Spec.clear();
   Parser.createSpecification("dir1/dir2/[*]",Spec);
   Control.pushBackPath("dir1");
   Control.pushBackPath("dir2");
   Control.testOptions().setRunAll(true);
   TST_ASSERT(Control == Spec);
   Control.testOptions().setRunAll(false);
   TST_ASSERT(Control != Spec);   

   Control.clear();
   Spec.clear();
   Parser.createSpecification("//////dir1//////dir2/////[abc def and ghi jkl]",Spec);
   Control.pushBackPath("dir1");
   Control.pushBackPath("dir2");
   Control.testOptions().addSubTest("abc def");
   Control.testOptions().addSubTest("ghi jkl");
   Control.testOptions().setRunAll(false);
   TST_ASSERT(Control == Spec);

   Control.clear();
   Spec.clear();
   Parser.createSpecification("/All Tests/[*]",Spec);
   Control.pushBackPath("All Tests");
   Control.testOptions().setRunAll(true);
   TST_ASSERT(Control == Spec);

   Control.clear();
   Spec.clear();
   Parser.createSpecification("/All Tests/[abc]",Spec);
   Control.pushBackPath("All Tests");
   Control.testOptions().addSubTest("abc");   
   Control.testOptions().setRunAll(false);
   TST_ASSERT(Control == Spec);

   Control.clear();
   Spec.clear();
   Parser.createSpecification("/dir 1/dir2/dir3/[abc and efg and]",Spec);
   Control.pushBackPath("dir 1");
   Control.pushBackPath("dir2");
   Control.pushBackPath("dir3");
   Control.testOptions().addSubTest("abc");
   Control.testOptions().addSubTest("efg");
   Control.testOptions().setRunAll(false);
   TST_ASSERT(Control == Spec);
}

void testTSTspecParserUnexpected()
{
   TSTtestSpecification Spec;
   TSTtestSpecificationParser Parser;

   bool Failed = false;
   try
   {
      Parser.createSpecification("/dir 1/dir2/dir3/",Spec);
   }
   catch (COLerror&) 
   {
      Failed = true;
   }
   TST_ASSERT(Failed);

   Failed = false;
   try
   {
      Parser.createSpecification("/dir 1/dir2/dir3/[**]",Spec);
   }
   catch (COLerror&) 
   {
      Failed = true;
   }
   TST_ASSERT(Failed);

   Failed = false;
   try
   {
      Parser.createSpecification("/dir 1/dir2/dir3/[]",Spec);
   }
   catch (COLerror&) 
   {
      Failed = true;
   }
   TST_ASSERT(Failed);

   Failed = false;
   try
   {
      Parser.createSpecification("/dir 1/dir2/dir3/[abc and * gef]",Spec);
   }
   catch (COLerror&) 
   {
      Failed = true;
   }
   TST_ASSERT(Failed);
}

void testTSTrunner()
{
   TSTtestRunner TestRunner;
   TSTtestResult TestResult;

   TSTflags Flags;

   TestRunner.runSpecifiedTestsInString(TSTtestCollection::globalCollection(),TestResult,"TSTspecificationParser Tests/[testTSTspecEquality]", Flags);
}

//void testTSTfixture()
//{
//   TSTtestFixture<TSTtestClass2>("testTSTfixture",TSTtestClass2::tests2());

   

//}

TSTtestable* TSTspecificationParserTests()
{
   TST_START_COLLECTION("TSTspecificationParser Tests")
   TST_ADD_TEST_FUNCTION(testTSTrunner);
   TST_ADD_TEST_FUNCTION(testTSTspecEquality);
   TST_ADD_TEST_FUNCTION(testTSTspecParserExpected);
   TST_ADD_TEST_FUNCTION(testTSTspecParserUnexpected);
   TST_END_COLLECTION
}
