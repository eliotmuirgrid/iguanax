//---------------------------------------------------------------------------
// Copyright (C) 2014-2014 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testDOMunbalanced.h
//
// Description:
//
// Tests for balanced tags.
//
// Author: Eliot Muir and Jonathan Marshall
// Date:   Mon 23 Jun 2014 09:29:02 EDT
//---------------------------------------------------------------------------

#include <DOMexample/DOMexamplePreCompiled.h>
#pragma hdrstop

#include <COL/COLstring.h>
#include <COL/COLostream.h>
#include <COL/COLarray.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <FIL/FILutils.h>

#include <DOM/DOMcomment.h>
#include <DOM/DOMdocument.h>
#include <DOM/DOMnode.h>
#include <DOM/DOMelement.h>
#include <DOM/DOMelementList.h>
#include <DOM/DOMattribute.h>
#include <DOM/DOMtext.h>
#include <DOM/DOMsink.h>
#include <DOM/DOMutils.h>

#include <TST/TSTtestFunction.h>
#include <TST/TSThelpers.h>
#include <TST/TSTtestFixture.h>

#include <DOMexample/DOMexampleTests.h>

void testDOMunbalancedTest(const COLstring& FileName, const COLstring& ExpectedErrorMessage){
   COL_FUNCTION(testDOMunbalancedTest);
   COLstring Content;
   FILreadFile(FileName, &Content);
   DOMdocument Document;
   DOMsink Sink(Document, Content.size());
   Sink.write(Content.c_str(), Content.size());
   COL_VAR(Document);
   COLstring ErrorMessage = DOMunbalancedTagWarning(Document, Sink);
   COL_VAR(ErrorMessage);
   TST_ASSERT_EQUALS(ExpectedErrorMessage,  ErrorMessage);
}

void testDOMunbalancedSimple(){
   COL_FUNCTION(testDOCunbalancedSimple);
   testDOMunbalancedTest("unbalanced.xml", "2 tags were not closed.  Tags affected: patient kin ");
   testDOMunbalancedTest("unbalanced1.xml", "patient tag was not closed.");
   testDOMunbalancedTest("unbalanced2.xml", "7 tags were not closed.  Tags affected: patient name last name first b dob ");
   testDOMunbalancedTest("unbalanced3.xml", "11 tags were not closed.  Tags affected: patient name last name first name first last x b dob ");
}

TSTtestable* testDOMbalanced(){
   TST_START_COLLECTION("testDOMbalanced");
      TST_ADD_TEST_FUNCTION(testDOMunbalancedSimple);
   TST_END_COLLECTION
}
