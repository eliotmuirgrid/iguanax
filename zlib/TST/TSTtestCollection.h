#ifndef __TST_TEST_COLLECTION_H__
#define __TST_TEST_COLLECTION_H__
//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TSTtestCollection
//
// Description:
//
// A collection of testable instances. The runTest on this class itself does nothing,
// since its just a container class. By having it implement the testable interface,
// its contents can be individual tests, or more collections.
//
// Author: Nasron Cheong
// Date:   Feb 2004 
//
//---------------------------------------------------------------------------

#include "TSTtestable.h"

class TSTtestSetUp;
class TSTtestTearDown;
class TSTtestCollectionPrivate;

class TSTtestCollection : public TSTtestable
{
public:
   TSTtestCollection(const COLstring& TestName);
   virtual ~TSTtestCollection();

   virtual TSTtype& testType() const;

   //takes ownership
   void addTest(TSTtestable* pNewTest);

   //gets a specific test in the collection
   COLuint32 countOfTest() const;
   TSTtestable* test(COLuint32 TestIndex) const;
   TSTtestable* setUp() const;
   TSTtestable* tearDown() const;

   void removeTest(COLuint32 TestIndex);

   void clear();

   //The global collection instance.
   static TSTtestCollection& globalCollection();

   //does nothing
   virtual void runTest(const TSTflags& Flags);

   virtual void runSetUp(const TSTflags& Flags);
   virtual void runTearDown(const TSTflags& Flags);

   void addSetUp(TSTtestable* pNewSetUp);
   void addTearDown(TSTtestable* pNewTearDown);

   void setSetUpNeeded(bool SetUpNeeded);
   void setTearDownNeeded(bool TearDownNeeded);
   bool setUpNeeded();
   bool tearDownNeeded();

   virtual TSTtestable* clone() const;

protected:
   void copyTo(TSTtestCollection& Target) const;

private:
   TSTtestCollectionPrivate* pMember;
   TSTtestCollection(const TSTtestCollection& Orig);
   TSTtestCollection& operator=(const TSTtestCollection& Orig);
};

#endif   // end of defensive include

