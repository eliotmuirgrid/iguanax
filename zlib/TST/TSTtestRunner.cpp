//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TSTtestRunner
//
// Description:
//
// Recieves a TSTtestable object, runs a test on it, and modifies a TSTtestResult object
//
// Author: Nasron Cheong
// Date:   Feb 2004 
//
//---------------------------------------------------------------------------

#include "TSTprecomp.h"
#pragma hdrstop

#include "TSTtestRunner.h"
#include "TSTtestResult.h"
#include "TSTtestCollection.h"
#include "TSTtestSpecification.h"
#include "TSTtestableOptions.h"
#include "TSTtestSpecificationParser.h"

#include <time.h>

class TSTtestRunnerPrivate
{
public:
   TSTtestRunnerPrivate(TSTtestRunner* ipOwner) : pOwner(ipOwner)
   {
      COLPRECONDITION(pOwner != NULL);
   };
   virtual ~TSTtestRunnerPrivate(){};

   bool runSingleTest(const COLstring& TestPath, TSTtestable& Test, TSTtestResult& Result, const TSTflags& Flags)
   {
      COLPRECONDITION(&Test.testType() != &TSTtypeTestCollection::instance());
      COLstring Message;
      bool Succeeded = true;
      COLostream Stream(Message);
      time_t Start = time(NULL);
      pOwner->onNextTest(TestPath, Test.testName());
      try
      {
         Test.runTest(Flags);
         time_t End = time(NULL);
         if (End-Start > 0){
            COLcout << " [" << End-Start << " secs]"; 
         }
         COLcout << newline;
      }
      catch(TSTexception& Error)
      {
         COLcout << newline;
         Succeeded = false;
         Stream << Error;
      } catch(COLerror& Error)
      {
         COLcout << newline;
         Succeeded = false;
         Stream << "UNEXPECTED EXCEPTION: ";
         Stream << Error;
      }
      Result.setMessage(Message);
      return Succeeded;
   }

   bool runSetUp(TSTtestCollection& pCollection, TSTtestResult& Result, const TSTflags& Flags)
   {
      COLPRECONDITION(pCollection.setUp());
      
      COLstring Message;
      bool Succeeded = true;
      COLostream Stream(Message);
      try
      {
         pCollection.runSetUp(Flags);
      }
      catch(TSTexception& Error)
      {
         Succeeded = false;
         Stream << Error;
      }
      catch(COLerror& Error)
      {
         Succeeded = false;
         Stream << "UNEXPECTED EXCEPTION: ";
         Stream << Error;
      }
      Result.setMessage(Message);
      return Succeeded;
   }

   bool runTearDown(TSTtestCollection& pCollection, COLstring& ResultMessage, const TSTflags& Flags)
   {
      COLPRECONDITION(pCollection.tearDown());
      
      bool Succeeded = true;
      COLostream Stream(ResultMessage);
      try
      {
         pCollection.runTearDown(Flags);
      }
      catch(TSTexception& Error)
      {
         Succeeded = false;
         Stream << Error;
      }
      catch(COLerror& Error)
      {
         Succeeded = false;
         Stream << "UNEXPECTED EXCEPTION: ";
         Stream << Error;
      }
      return Succeeded;
   }
   
   TSTtestable* findSubTest(TSTtestCollection* pCollection, const COLstring& TestName)
   {
      // To make it easier on users, I'll force the names to lower case so that CaPItaLiZation doesn't matter
      COLstring SubTestName = TestName;
      SubTestName.toLowerCase();
      for (COLuint32 TestIndex = 0; TestIndex < pCollection->countOfTest(); TestIndex++)
      {
         COLstring Name = pCollection->test(TestIndex)->testName();
         Name.toLowerCase();
         if (SubTestName == Name)
         {
            return pCollection->test(TestIndex);
         }
      }
      return 0; 
   }

private:
   TSTtestRunner* pOwner;
};

TSTtestRunner::TSTtestRunner()
{
   pMember = new TSTtestRunnerPrivate(this);
}

TSTtestRunner::~TSTtestRunner()
{
   delete pMember;
}

COLstring TSTaddPath(const COLstring& BasePath, const COLstring& TestName)
{
   if(BasePath.is_null())
   {
      return TestName;
   }
   return BasePath + "/" + TestName;
}

bool TSTtestRunner::runAll(TSTtestable& Test, TSTtestResult& Result, const TSTflags& Flags, COLuint32 MaxDepth)
{
   // this one is called from the top
   return runAll("/", Test, Result, Flags, MaxDepth);
}

bool TSTtestRunner::runAll(const COLstring& TestPath, TSTtestable& Test, TSTtestResult& Result, const TSTflags& Flags, COLuint32 MaxDepth)
{
   Result.setTestName(Test.testName());
   bool Succeeded = true;
   if (Test.testType().isDerivedFrom(TSTtypeTestCollection::instance()))
   {
      TSTtestCollection* pCollection = (TSTtestCollection*)&Test;
      //run test on collections are used to initialize their contents
      //see TSTtestFixture as an example
      pCollection->runTest(Flags);
      
      bool AllPassed = true;
      COLuint32 NextMaxDepth = MaxDepth-1;
      if (NextMaxDepth)
      {
         COLstring TestSubPath;
         if(TestPath == "/") // "root" is a special path given from the top -
         {
            TestSubPath = ""; // to ignore the top-level collection name when displaying testspecs in progress (so that we see pure testspecs)
         }
         else
         {
            TestSubPath = TSTaddPath(TestPath, pCollection->testName());
         }
         
         for (COLuint32 TestIndex = 0; TestIndex < pCollection->countOfTest(); TestIndex++)
         {
            TSTtestResult& NewResult = Result.push_back();
            NewResult.setSucceeded(true);
            if(pCollection->setUpNeeded())
            {
               Succeeded = pMember->runSetUp(*pCollection, NewResult, Flags);
               // if the SetUp fails we do not run any of the tests in the collection
               if(!Succeeded)
               {
                  NewResult.setTestName(pCollection->setUp()->testName());
                  NewResult.setSucceeded(false);
                  Result.setSucceeded(false);
                  return Succeeded;
               }
            }
            if (!runAll(TestSubPath, *pCollection->test(TestIndex),NewResult,Flags, NextMaxDepth))
            {
               if (AllPassed)
               {
                  AllPassed = false;
               }
               NewResult.setSucceeded(false);
            }
            if(pCollection->tearDownNeeded())
            {
               COLstring TearDownResultMessage;
               Succeeded = pMember->runTearDown(*pCollection, TearDownResultMessage, Flags);
               // if the TearDown fails we do not run the rest of the tests in the collection
               if(!Succeeded)
               {
                  NewResult.setMessage(TearDownResultMessage);
                  NewResult.setTestName(pCollection->tearDown()->testName());
                  NewResult.setSucceeded(false);
                  Result.setSucceeded(false);
                  return Succeeded;
               }
            }
         }
      }
      Succeeded = AllPassed;
   }
   else if (Test.testType().isDerivedFrom(TSTtypeTestable::instance()))
   {      
      Succeeded = pMember->runSingleTest(TestPath, Test,Result, Flags);
   }
   else
   {
      COL_ERROR_STREAM("Invalid type for run",COLerror::PreCondition);
   }
   Result.setSucceeded(Succeeded);
   return Succeeded;
}

bool TSTverifyResultSuccess(TSTtestResult& Result)
{
   if (!Result.countOfSubTestResult())
   {
      return Result.succeeded();
   }
   //recurse first, assume success unless sub test indicates at least one failure
   bool Succeeded = true;
   for (COLuint32 ResultIndex = 0; ResultIndex != Result.countOfSubTestResult(); ResultIndex++)
   {
      if (!TSTverifyResultSuccess(Result.subTestResult(ResultIndex)))
      {
         Succeeded = false;
      }
   }
   Result.setSucceeded(Succeeded);
   return Succeeded;
}

bool TSTtestRunner::runSpecifiedTests(TSTtestable& Test, TSTtestResult& Result, const TSTtestSpecification& Specification, const TSTflags& Flags)
{
   //resolve the path from the spec
   TSTtestable* pTest = &Test;
   TSTtestResult* pResult = &Result;
   pResult->setTestName(Test.testName());
   bool Succeeded = true;

   COLstring TestPath = "";
   
   for (COLuint32 PathIndex = 0; PathIndex < Specification.pathDepth(); PathIndex++)
   {
      COLstring TestName = Specification.pathItem(PathIndex);
      if (!pTest->testType().isDerivedFrom(TSTtypeTestCollection::instance()))
      {
         COL_ERROR_STREAM("Specification path is invalid. " << pTest->testName() << " is not a collection so " << TestName << " cannot be contained by it",COLerror::PreCondition);
      }

      if(PathIndex+1 < Specification.pathDepth())
      {
         TestPath = TSTaddPath(TestPath, TestName);
      }
      
      TSTtestable* pOldTest = pTest;
      //run test on collections are used to initialize their contents
      //see TSTtestFixture as an example
      ((TSTtestCollection*)pTest)->runTest(Flags);
      if( ((TSTtestCollection*)pTest)->setUpNeeded() )
      {
         try
         {
            ((TSTtestCollection*)pTest)->runSetUp(Flags);
         }
         catch(TSTexception& Error)
         {  // if the SetUp fails we do not run any of the tests in the collection
            COLstring Message;
            COLostream Stream(Message);
            Stream << Error;
            Succeeded = false;
            pResult = &pResult->push_back();
            pResult->setTestName(((TSTtestCollection*)pTest)->setUp()->testName());
            pResult->setMessage(Message);
            pResult->setSucceeded(false);
            return Succeeded;
         }
      }

      pTest = pMember->findSubTest((TSTtestCollection*)pTest,TestName);
      if (!pTest)
      {
         COL_ERROR_STREAM("Specification path is invalid. " << TestName << " not found in current test \"" << pOldTest->testName()<< "\"",COLerror::PreCondition);
      }
      pResult = &pResult->push_back();
      pResult->setTestName(pTest->testName());
   }

   //run test on collections are used to initialize their contents
   //see TSTtestFixture as an example
   ((TSTtestCollection*)pTest)->runTest(Flags);

   //run pTest with the options
   if (Specification.testOptions().runAll())
   {
      Succeeded = runAll(TestPath, *pTest,*pResult, Flags);
   }
   else if (pTest->testType().isDerivedFrom(TSTtypeTestCollection::instance()))
   {
      //verify
      COLuint32 SpecIndex = 0;
      for (SpecIndex = 0; SpecIndex < Specification.testOptions().countOfSubTest(); SpecIndex++)
      {
         TSTtestable* pSubTest = pMember->findSubTest((TSTtestCollection*)pTest,Specification.testOptions().subTest(SpecIndex));
         if (!pSubTest)
         {
            COL_ERROR_STREAM("Specification option is invalid. " << Specification.testOptions().subTest(SpecIndex) << " not found in current test " << pTest->testName(),COLerror::PreCondition);
         }            
      }

      //result requires another level
      pResult->setTestName(pTest->testName());

      bool AllSucceed = true;
      //run
      for (SpecIndex = 0; SpecIndex < Specification.testOptions().countOfSubTest(); SpecIndex++)
      {
         TSTtestResult& NewResult = pResult->push_back();
         TSTtestable* pSubTest = pMember->findSubTest((TSTtestCollection*)pTest,Specification.testOptions().subTest(SpecIndex));
         // if pSubTest is a single test must run SetUp for it, otherwise runAll will take care of it
         if(pSubTest->testType().isDerivedFrom(TSTtypeTestable::instance())
            && ((TSTtestCollection*)pTest)->setUpNeeded() )
         {
            try
            {
               ((TSTtestCollection*)pTest)->runSetUp(Flags);
            }
            catch(TSTexception& Error)
            {  // if the SetUp fails we do not run any of the tests in the collection
               COLstring Message;
               COLostream Stream(Message);
               Stream << Error;
               Succeeded = false;
               NewResult.setTestName(((TSTtestCollection*)pTest)->setUp()->testName());
               NewResult.setMessage(Message);
               NewResult.setSucceeded(false);
               return Succeeded;
            }
         }
         runAll(TSTaddPath(TestPath, pTest->testName()), *pSubTest,NewResult, Flags);
         if (!NewResult.succeeded() && AllSucceed)
         {
            AllSucceed = false;
         }
      }
      Succeeded = AllSucceed;
      pResult->setSucceeded(Succeeded);
   }
   else
   {
      COL_ERROR_STREAM("Test range specified but Test " <<pTest->testName() << "is not a collection, and runAll is false.",COLerror::PreCondition);
   }

   //we have to verify the result in case we used a spec test
   TSTverifyResultSuccess(Result);
   return Succeeded;
}

bool TSTtestRunner::runSpecifiedTestsInString(TSTtestable& Test, TSTtestResult& Result, const COLstring& SpecString, const TSTflags& Flags)
{
   TSTtestSpecificationParser Parser;
   TSTtestSpecification Spec;
   Parser.createSpecification(SpecString,Spec);
   return runSpecifiedTests(Test,Result,Spec, Flags);
}
