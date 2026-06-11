//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TSTtestCollection
//
// Description:
//
// Implementation
//
// Author: Nasron Cheong
// Date:   Feb 2004 
//
//---------------------------------------------------------------------------

#include "TSTprecomp.h"
#pragma hdrstop

#include "TSTtestCollection.h"
#include "TSTtestSetUpFunction.h"
#include "TSTtestTearDownFunction.h"
#include "TSTtestSetUpMethod.h"
#include "TSTtestTearDownMethod.h"
#include "TSTflags.h"
#include <COL/COLarray.h>
#include <COL/COLauto.h>

typedef COLarray< COLauto<TSTtestable> > TSubTests;

class TSTtestCollectionPrivate
{
public:
   TSTtestCollectionPrivate()
   {
      SetUpNeeded = 0;
      TearDownNeeded = 0;
      SetUp = 0;
      TearDown = 0;
   }
   virtual ~TSTtestCollectionPrivate()
   {
      delete SetUp;
      delete TearDown;
   }
   TSubTests SubTests;
   bool SetUpNeeded;
   bool TearDownNeeded;
   TSTtestable* SetUp;
   TSTtestable* TearDown;
};

TSTtestCollection::TSTtestCollection(const COLstring& Name)
   : TSTtestable(Name)
{
   pMember = new TSTtestCollectionPrivate;
   pMember->SetUpNeeded = false;
   pMember->TearDownNeeded = false;
}

TSTtestCollection::~TSTtestCollection()
{
   delete pMember;
}

TSTtype& TSTtestCollection::testType() const
{
   return TSTtypeTestCollection::instance();
}

void TSTtestCollection::addTest(TSTtestable* pNewTest)
{
   pMember->SubTests.push_back(pNewTest);
}

void TSTtestCollection::addSetUp(TSTtestable* pNewSetUp)
{
   pMember->SetUp = pNewSetUp;
   pMember->SetUpNeeded=true;
}

void TSTtestCollection::addTearDown(TSTtestable* pNewTearDown)
{
   pMember->TearDown = pNewTearDown;
   pMember->TearDownNeeded=true;
}

COLuint32 TSTtestCollection::countOfTest() const
{
   return pMember->SubTests.size();
}

TSTtestable* TSTtestCollection::test(COLuint32 TestIndex) const
{
   return pMember->SubTests[TestIndex];
}


TSTtestable* TSTtestCollection::setUp() const
{
   return pMember->SetUp;
}

TSTtestable* TSTtestCollection::tearDown() const
{
   return pMember->TearDown;
}

void TSTtestCollection::clear()
{
   pMember->SubTests.clear();
}

TSTtestCollection& TSTtestCollection::globalCollection()
{
   static TSTtestCollection Instance("All Tests");
   return Instance;
}

void TSTtestCollection::runTest(const TSTflags& Flags)
{
   //do nothing
}

void TSTtestCollection::runSetUp(const TSTflags& Flags)
{
   pMember->SetUp->runTest(Flags);
}

void TSTtestCollection::runTearDown(const TSTflags& Flags)
{
   pMember->TearDown->runTest(Flags);
}

void TSTtestCollection::setSetUpNeeded(bool SetUpNeeded)
{
   pMember->SetUpNeeded = SetUpNeeded;
}

bool TSTtestCollection::setUpNeeded()
{
   return pMember->SetUpNeeded;
}

void TSTtestCollection::setTearDownNeeded(bool TearDownNeeded)
{
   pMember->TearDownNeeded = TearDownNeeded;
}

bool TSTtestCollection::tearDownNeeded()
{
   return pMember->TearDownNeeded;
}

TSTtestable* TSTtestCollection::clone() const
{
   COLauto<TSTtestCollection> pNewCollection = new TSTtestCollection(testName());
   copyTo(*pNewCollection);
   pNewCollection.setIsOwner(false);
   return pNewCollection;
}

void TSTtestCollection::copyTo(TSTtestCollection& Target) const
{
   Target.pMember->SubTests.clear();
   for (TSubTests::const_iterator Iter = pMember->SubTests.begin(); Iter != pMember->SubTests.end(); ++Iter)
   {
      COLauto<TSTtestable> pNewTest = (*Iter)->clone();
      Target.addTest(pNewTest);
      pNewTest.setIsOwner(false);
   }
}

void TSTtestCollection::removeTest(COLuint32 TestIndex)
{
   pMember->SubTests.remove(TestIndex);
}
