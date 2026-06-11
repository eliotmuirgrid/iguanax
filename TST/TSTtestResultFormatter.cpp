//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TSTtestResultFormatter
//
// Description:
//
// Description: Formats a Result object into a string.
//
// Author: Nasron Cheong
// Date:   Feb 2004 
//
//---------------------------------------------------------------------------

#include "TSTprecomp.h"
#pragma hdrstop

#include "TSTtestResultFormatter.h"
#include "TSTtestResult.h"

class TSTtestResultFormatterPrivate
{
public:
   TSTtestResultFormatterPrivate(){};
   virtual ~TSTtestResultFormatterPrivate(){};

   void printOn(TSTtestResult& TestResult, COLsink& Sink,const COLstring& Indent)
   {
      COLstring NextIndent = Indent;
      NextIndent += "   ";
      COLostream Writer(Sink);
      Writer << Indent << '[' << TestResult.testName() << ":";
      if (TestResult.succeeded())
      {
         Writer << "Succeeded";
      }
      else
      {
         Writer << "FAILED";
      }      
      Writer << " : ]" << newline;
      if (TestResult.message().size() > 0)
      {
         Writer << newline << TestResult.message() << newline;
      }
      if (TestResult.countOfSubTestResult() > 0)
      {
         for (COLuint32 TestIndex = 0; TestIndex < TestResult.countOfSubTestResult(); TestIndex++)
         {
            printOn(TestResult.subTestResult(TestIndex),Sink,NextIndent);
         }
      }

      Writer << flush;
   }
};

void TSTshowRunnableFailuresNonRoot
(
   const TSTtestResult&   TestResult,
   COLostream&      Stream,
   const COLstring& Parent
)
{
   if (TestResult.succeeded())
   {
      // we only care about failures!
      return;
   }
   if (TestResult.countOfSubTestResult() > 0)
   {
      for (COLuint32 TestIndex = 0; TestIndex < TestResult.countOfSubTestResult(); TestIndex++)
      {
         TSTtestResult& SubResult = TestResult.subTestResult(TestIndex);
         COLstring ParentPath(Parent);
         ParentPath += TestResult.testName();
         ParentPath += '/';
         TSTshowRunnableFailuresNonRoot(SubResult,Stream,ParentPath);
      }
   }
   else
   {
      Stream << Parent << "[" << TestResult.testName() << "]" << newline;
   }
}

void TSTshowRunnableFailures
(
   const TSTtestResult&   TestResult,
   COLostream&      Stream
)
{
   if (TestResult.succeeded())
   {
      // we only care about failures!
      return;
   }
   for (COLuint32 TestIndex = 0; TestIndex < TestResult.countOfSubTestResult(); TestIndex++)
   {
      TSTtestResult& SubResult = TestResult.subTestResult(TestIndex);
      TSTshowRunnableFailuresNonRoot(SubResult,Stream,"");
   }
}

void TSTtestResultFormatter::printOn(TSTtestResult& TestResult, COLsink& Sink)
{
   pMember->printOn(TestResult,Sink,"");
}

void TSTtestResultFormatter::showFailures(TSTtestResult& TestResult, COLostream& Stream)
{
   if (!TestResult.succeeded())
   {
      // Important the next two lines are used by our autobuild system to 
      // identify the part of unit test output we want to put in the build report.
   
      Stream << newline << "-oOo-" << newline;
      COLuint32 CountOfFailure = TestResult.countOfFailure();
      if (CountOfFailure == 1)
      {
         Stream << "Failed 1 test." << newline;
      }
      else
      {
         Stream << newline << "Failed " << TestResult.countOfFailure() << " tests:" << newline;
      }
      
      TSTshowRunnableFailures(TestResult, Stream);
      Stream << newline << "-oOo-" << newline;
      Stream << flush;
   }
   else
   {
      Stream << newline << "COMPLETE SUCCESS!" << newline;
   }
}

TSTtestResultFormatter::TSTtestResultFormatter()
{
   pMember = new TSTtestResultFormatterPrivate;
}

TSTtestResultFormatter::~TSTtestResultFormatter()
{
   delete pMember;
}
