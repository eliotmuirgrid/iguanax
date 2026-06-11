//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TSTtestResult
//
// Description:
//
// Description: To avoid having multiple result classes per TSTtestable type,
// this class is used. It shows the names and messages of TSTtestables, but 
// not the TSTtype of the test (which is usually not of much interest anyway).
//
// Author: Nasron Cheong
// Date:   Feb 2004 
//
//---------------------------------------------------------------------------

#include "TSTprecomp.h"
#pragma hdrstop

#include "TSTtestResult.h"
#include <COL/COLarray.h>
#include <COL/COLauto.h>

class TSTtestResultPrivate
{
public:
   TSTtestResultPrivate() : Succeeded(false){};
   virtual ~TSTtestResultPrivate(){};
   typedef COLarray< COLauto<TSTtestResult> > TSubResults;

   void copyFrom(const TSTtestResultPrivate& Orig)
   {
      SubResults.clear();
      for (TSubResults::const_iterator Iter = Orig.SubResults.begin();
           Iter != Orig.SubResults.end(); ++Iter)
      {
         SubResults.push_back( new TSTtestResult(*Iter) );
      }
      Succeeded = Orig.Succeeded;
      Message = Orig.Message;
      TestName = Orig.TestName;
   }

   TSubResults SubResults;
   bool Succeeded;
   COLstring TestName;
   COLstring Message;
};

TSTtestResult::TSTtestResult(const TSTtestResult& Orig)
{
   pMember = new TSTtestResultPrivate;
   pMember->copyFrom(*Orig.pMember);
}

TSTtestResult& TSTtestResult::operator=(const TSTtestResult& Orig)
{
   if (&Orig != this)
   {
      pMember->copyFrom(*Orig.pMember);
   }
   return *this;
}

TSTtestResult::TSTtestResult()
{
   pMember = new TSTtestResultPrivate;
}

TSTtestResult::~TSTtestResult()
{
   delete pMember;
}


bool TSTtestResult::succeeded() const
{
   return pMember->Succeeded;
}

void TSTtestResult::setSucceeded(bool Succeeded)
{
   pMember->Succeeded = Succeeded;
}

const COLstring& TSTtestResult::testName() const
{
   return pMember->TestName;
}

void TSTtestResult::setTestName(const COLstring& Name)
{
   pMember->TestName = Name;
}

const COLstring& TSTtestResult::message() const
{
   return pMember->Message;
}

void TSTtestResult::setMessage(const COLstring& Message)
{
   pMember->Message = Message;
}

COLuint32 TSTtestResult::countOfSubTestResult() const
{
   return pMember->SubResults.size();
}

TSTtestResult& TSTtestResult::subTestResult(COLuint32 TestIndex) const
{
   return pMember->SubResults[TestIndex];
}

TSTtestResult& TSTtestResult::push_back()
{
   pMember->SubResults.push_back(new TSTtestResult);
   return pMember->SubResults.back();
}

void TSTtestResult::clear()
{
   pMember->SubResults.clear();
}

COLuint32 TSTtestResult::countOfFailure() const
{
   COLuint32 CountOfFailure = 0;
   if (countOfSubTestResult() > 0)
   {
      for (COLuint32 ResultIndex = 0; ResultIndex < countOfSubTestResult(); ResultIndex++)
      {
         CountOfFailure += subTestResult(ResultIndex).countOfFailure();         
      }
   }
   else
   {
      if (!succeeded()) 
      {
         CountOfFailure = 1;
      }
   }
   return CountOfFailure;
}

static TSTtestResult* findSubTestResult(const COLstring& Name, TSTtestResult& Source)
{
   for (COLuint32 ResultIndex = 0; ResultIndex != Source.countOfSubTestResult(); ResultIndex++)
   {
      TSTtestResult& SubResult = Source.subTestResult(ResultIndex);
      if (SubResult.testName() == Name)
      {
         return &SubResult;
      }
   }
   return 0;
}

void TSTmergeResults(const TSTtestResult& Source, TSTtestResult& Target)
{
   if (Source.countOfSubTestResult() == 0 ||
       Target.testName().is_null())
   {
      Target = Source;
   }
   else
   {
      for (COLuint32 ResultIndex = 0; ResultIndex != Source.countOfSubTestResult(); ResultIndex++)
      {
         const TSTtestResult& SubResult = Source.subTestResult(ResultIndex);
         TSTtestResult* pResultInTarget = findSubTestResult(SubResult.testName(),Target);
         if (!pResultInTarget)
         {
            pResultInTarget = &Target.push_back();
            *pResultInTarget = SubResult;
         }
         else
         {
            TSTmergeResults(SubResult, *pResultInTarget);
         }
      }
   }
}
