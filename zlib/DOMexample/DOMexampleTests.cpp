//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2004 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DOMexampleTests.h
//
// Description:
//
// A set of utilities to test the DOM
//
// Author: Rob Moyse
// Date:   November 11, 2004
//
//
//---------------------------------------------------------------------------

#include <DOMexample/DOMexamplePreCompiled.h>
#pragma hdrstop

#include <COL/COLstring.h>
#include <COL/COLostream.h>
#include <COL/COLarray.h>

#include <FIL/FILfile.h>
#include <FIL/FILutils.h>

#include <DOM/DOMcomment.h>
#include <DOM/DOMdocument.h>
#include <DOM/DOMnode.h>
#include <DOM/DOMelement.h>
#include <DOM/DOMelementList.h>
#include <DOM/DOMattribute.h>
#include <DOM/DOMtext.h>
#include <DOM/DOMsink.h>

#include <TST/TSThelpers.h>
#include <TST/TSTtestFixture.h>

#include <DOMexample/DOMexampleTests.h>

class DOMexampleTestsPrivate
{
public:
   DOMdocument ExampleDocument;
};

DOMexampleTests::DOMexampleTests()
{
   pMember = new DOMexampleTestsPrivate();
}

DOMexampleTests::~DOMexampleTests()
{
   delete pMember;
}

void DOMexampleTests::loadDocumentOne() {
   COLstring Content;
   FILreadFile("example.xml", &Content);
   DOMsink Sink(pMember->ExampleDocument, Content.size());
   Sink.write(Content.c_str(), Content.size());
}

void DOMexampleTests::traversePatientNames()
{
   DOMelementList Patients = pMember->ExampleDocument.elementsByTagName("patient");
   for (COLuint32 PatientIndex = 0; PatientIndex < Patients.countOfElement(); PatientIndex++)
   {

      COLstring FirstName = Patients[PatientIndex]->attribute("first");
      COLstring MiddleName = Patients[PatientIndex]->attribute("middle");
      COLstring LastName = Patients[PatientIndex]->attribute("last");

      if (PatientIndex == 0)
      {
         TST_ASSERT_MESSAGE
         ( 
            (FirstName == "Henry") && (LastName == "Tran") && (MiddleName == "Luong"), 
            FirstName << " " << MiddleName << " " 
            << LastName << " not expected at index " << PatientIndex 
         );
      }
      else if (PatientIndex == 1)
      {
         TST_ASSERT_MESSAGE
         ( 
            (FirstName == "Rob") && (LastName == "Moyse") && (MiddleName == "Acram"), 
            FirstName << " " << MiddleName << " " 
            << LastName << " not expected at index " << PatientIndex 
         );
      }
      else if (PatientIndex == 2)
      {
         TST_ASSERT_MESSAGE
         ( 
            (FirstName == "Nasron") && (LastName == "Cheong") && (MiddleName == "Siu-Hua"), 
            FirstName << " " << MiddleName << " " 
            << LastName << " not expected at index " << PatientIndex 
         );
      }

   }

}

void DOMexampleTests::traversePatientTests()
{

   COLstring TestResultOne = pMember->ExampleDocument.elementsByTagName("patient")[0]->elementsByTagName("test")[1]->elementsByTagName("result")[0]->childNode(0)->nodeValue();
   TST_ASSERT_MESSAGE( TestResultOne == "Never", "Invalid TestResultOne [ " << TestResultOne << "]" );
   
   COLstring TestResultTwo = pMember->ExampleDocument.elementsByTagName("patient")[1]->elementsByTagName("test")[1]->elementsByTagName("result")[0]->childNode(0)->nodeValue();
   TST_ASSERT_MESSAGE( TestResultTwo == "Sometimes", "Invalid TestResultTwo [ " << TestResultTwo << "]" );

   DOMnode* pCdataNode = pMember->ExampleDocument.elementsByTagName("patient")[2]->elementsByTagName("test")[1]->elementsByTagName("result")[0]->childNode(1);
   TST_ASSERT_MESSAGE( pCdataNode->nodeType() == DOMnode::CDataSectionNode, "Invalid node type.");

}


void DOMexampleTests::modifyPatientTest()
{
   DOMelementList Patients = pMember->ExampleDocument.elementsByTagName("patient");
   for (COLuint32 PatientIndex = 0; PatientIndex < Patients.countOfElement(); PatientIndex++)
   {

      // ----------------------------------------------------------------------
      // Let's change Henry's genius status from inconclusive to positive.
      // ----------------------------------------------------------------------

      if (Patients[PatientIndex]->attribute("first") == "Henry")
      {

         COLstring TestResultOneBefore = Patients[PatientIndex]->elementsByTagName("test")[0]->elementsByTagName("result")[0]->childNode(0)->nodeValue();
         TST_ASSERT_MESSAGE( TestResultOneBefore == "Inconclusive", "Invalid TestResultOneBefore [ " << TestResultOneBefore << "]" );

         DOMelementList Tests = Patients[PatientIndex]->elementsByTagName("test");
         for (COLuint32 TestIndex = 0; TestIndex < Tests.countOfElement(); TestIndex++)
         {
            if (Tests[TestIndex]->attribute("name") == "genius")
            {
               Tests[TestIndex]->elementsByTagName("result")[0]->childNode(0)->setNodeValue("Positive");
            }
         }

         COLstring TestResultOneAfter = Patients[PatientIndex]->elementsByTagName("test")[0]->elementsByTagName("result")[0]->childNode(0)->nodeValue();
         TST_ASSERT_MESSAGE( TestResultOneAfter == "Positive", "Invalid TestResultOneAfter [ " << TestResultOneAfter << "]" );
      }

   }
}

void DOMexampleTests::modifyPatientTestAdd()
{
   DOMelementList Patients = pMember->ExampleDocument.elementsByTagName("patient");
   for (COLuint32 PatientIndex = 0; PatientIndex < Patients.countOfElement(); PatientIndex++)
   {

      // ----------------------------------------------------------------------
      // Let's add a new test and a comment to patient Rob
      // ----------------------------------------------------------------------
      
      if (Patients[PatientIndex]->attribute("first") == "Rob")
      {
      
         Patients[PatientIndex]->appendChild(new DOMtext(Patients[PatientIndex], &pMember->ExampleDocument, "   ")); // formatting text
         DOMelement* pNewTest = (DOMelement*)Patients[PatientIndex]->appendChild(new DOMelement(Patients[PatientIndex], &pMember->ExampleDocument, "test"));
         pNewTest->setAttribute("name", "Stylin'");
         pNewTest->appendChild(new DOMtext(Patients[PatientIndex], &pMember->ExampleDocument, "\n         ")); // formatting text
         DOMelement* pNewResult = (DOMelement*)pNewTest->appendChild(new DOMelement(pNewTest, &pMember->ExampleDocument, "result"));
         DOMtext* pNewResultText = (DOMtext*)pNewResult->appendChild(new DOMtext(pNewResult, &pMember->ExampleDocument, "yes."));
         pNewTest->appendChild(new DOMtext(Patients[PatientIndex], &pMember->ExampleDocument, "\n      ")); // formatting text
         Patients[PatientIndex]->appendChild(new DOMtext(Patients[PatientIndex], &pMember->ExampleDocument, "\n   ")); // formatting text

         // now add a comment prior to the test we just added
         Patients[PatientIndex]->insertBefore(pNewTest, new DOMcomment(Patients[PatientIndex], &pMember->ExampleDocument, "Standard test for stylishness."));
         Patients[PatientIndex]->insertBefore(pNewTest, new DOMtext(Patients[PatientIndex], &pMember->ExampleDocument, "\n      ")); // formatting text

         COLstring NewTestName = Patients[PatientIndex]->elementsByTagName("test")[2]->attribute("name");
         TST_ASSERT_MESSAGE( NewTestName == "Stylin'", "Unexpected NewTestName [ " << NewTestName << "]" );

         COLstring CommentText = Patients[PatientIndex]->elementsByTagName("test")[2]->previousSibling()->previousSibling()->nodeValue();
         TST_ASSERT_MESSAGE( CommentText == "Standard test for stylishness.", "Unexpected CommentText [ " << CommentText << "]" );

      }

   }
}

void DOMexampleTests::modifyPatientTestAppend()
{
   DOMelementList Patients = pMember->ExampleDocument.elementsByTagName("patient");
   for (COLuint32 PatientIndex = 0; PatientIndex < Patients.countOfElement(); PatientIndex++)
   {

      // ----------------------------------------------------------------------
      // Now we want to change Rob's stylin' status
      // ----------------------------------------------------------------------
      
      if (Patients[PatientIndex]->attribute("first") == "Rob")
      {
         ((DOMtext*)Patients[PatientIndex]->elementsByTagName("test")[2]->childNode(1)->childNode(0))->insertData(0, "The answer is decidedly not ");

         COLstring NewTestResult = Patients[PatientIndex]->elementsByTagName("test")[2]->childNode(1)->childNode(0)->nodeValue();
         TST_ASSERT_MESSAGE( NewTestResult == "The answer is decidedly not yes.", "Unexpected NewTestResult [ " << NewTestResult << "]" );
      }

   }
}

void DOMexampleTests::saveDocumentOne()
{
   FILfile OutputFile("output.xml", FILfile::Rewrite);
   COLostream OutputStream(OutputFile);
   OutputStream << pMember->ExampleDocument << flush;
}

TSTtestable* DOMexampleTests::allTests()
{
   TST_START_FIXTURE(DOMexampleTests)
      TST_ADD_FIXTURE_INHERIT("LoadTests")
         TST_ADD_TEST_METHOD(loadDocumentOne)
      TST_END_ADD_COLLECTION
      TST_ADD_FIXTURE_INHERIT("TraversalTests")
         TST_ADD_TEST_METHOD(traversePatientNames)
         TST_ADD_TEST_METHOD(traversePatientTests)
      TST_END_ADD_COLLECTION
      TST_ADD_FIXTURE_INHERIT("ManipulateTests")
         TST_ADD_TEST_METHOD(modifyPatientTest)
         TST_ADD_TEST_METHOD(modifyPatientTestAdd)
         TST_ADD_TEST_METHOD(modifyPatientTestAppend)         
      TST_END_ADD_COLLECTION
      TST_ADD_FIXTURE_INHERIT("SaveTests")
         TST_ADD_TEST_METHOD(saveDocumentOne)
      TST_END_ADD_COLLECTION
   TST_END_FIXTURE

}

