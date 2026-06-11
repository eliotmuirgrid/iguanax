//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TSTtestFormatter
//
// Description:
//
// TSTtestFormatter: A class that prints the structure of a TSTtestable. It typechecks 
// and recurses through collections.
//
// Author: Nasron Cheong
// Date:   Feb 2004 
//
//---------------------------------------------------------------------------

#include "TSTprecomp.h"
#pragma hdrstop

#include "TSTtestFormatter.h"
#include "TSTtestResult.h"
#include "TSTtestable.h"
#include "TSTtestCollection.h"

class TSTtestFormatterPrivate
{
public:
   TSTtestFormatterPrivate() {}
   virtual ~TSTtestFormatterPrivate() {}

   void printOn(TSTtestable& Test, COLsink& Sink,const COLstring& Indent)
   {
      COLstring NextIndent = Indent;
      NextIndent += "   ";
      COLostream Writer(Sink);
      if (Test.testType().isDerivedFrom(TSTtypeTestCollection::instance()))
      {
         TSTtestCollection* pCollection = (TSTtestCollection*)&Test;
         Writer << Indent << '[' << pCollection->testName() << ']' << newline;
         for (COLuint32 TestIndex = 0; TestIndex < pCollection->countOfTest(); TestIndex++)
         {
            printOn(*pCollection->test(TestIndex),Sink,NextIndent);
         }
      }
      else if (Test.testType().isDerivedFrom(TSTtypeTestable::instance()))
      {
         Writer << Indent << '[' << Test.testName() << ":" << Test.testType().name() << ']' << newline;
      }
      else
      {
         COL_ERROR_STREAM("Invalid Type", COLerror::PreCondition);
      }
      Writer << flush;
   }

   void printFullTestNames(TSTtestable& Test, COLsink& Sink, const COLstring& Prefix = "", const TSTtestCollection* ParentTest = 0)
   {
      COLstring NextPrefix = Prefix;
      COLostream Writer(Sink);
      if (Test.testType().isDerivedFrom(TSTtypeTestCollection::instance()))
      {
         TSTtestCollection* pCollection = (TSTtestCollection*)&Test;
         COLstring Parentage = ParentTest ? Prefix + pCollection->testName() + '/' : "";
         for (COLuint32 TestIndex = 0; TestIndex < pCollection->countOfTest(); TestIndex++)
         {
            printFullTestNames(*pCollection->test(TestIndex), Sink, Parentage, pCollection);
         }
      }
      else if (Test.testType().isDerivedFrom(TSTtypeTestable::instance()))
      {
         Writer << '\"' << Prefix << '[' << Test.testName() << ']' << '\"' << newline;
      }
      else
      {
         COL_ERROR_STREAM("Invalid Type", COLerror::PreCondition);
      }
      Writer << flush;
   }
};

void TSTtestFormatter::printOn(TSTtestable& Test, COLsink& Sink)
{
   pMember->printOn(Test,Sink,"");
}

void TSTtestFormatter::printFull(TSTtestable& Test, COLsink& Sink)
{
   pMember->printFullTestNames(Test, Sink);
}

TSTtestFormatter::TSTtestFormatter()
{
   pMember = new TSTtestFormatterPrivate;
}

TSTtestFormatter::~TSTtestFormatter()
{
   delete pMember;
}
