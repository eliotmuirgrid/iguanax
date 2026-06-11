#ifndef __TST_TEST_FIXTURE_H__
#define __TST_TEST_FIXTURE_H__
//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TSTtestFixture
//
// Description:
//
// A special type of test collection that is templated. The runTest command initializes
// any TSTtestMethod members to use a single instance that this class owns. This allows
// testMethods inside a testFixture to have consistent state between each other.
//
// Author: Nasron Cheong
// Date:   Feb 2004 
//
//---------------------------------------------------------------------------

#include <TST/TSTtestCollection.h>
#include <TST/TSTtestMethod.h>
#include <TST/TSTtestSetUpMethod.h>
#include <TST/TSTtestTearDownMethod.h>
#include <COL/COLauto.h>
//#include <TST/TSTtype.h>

template <class _T> 
class TSTtestFixture : public TSTtestCollection
{
public:
   
   typedef _T* (*fpClassForFixtureCreateDef)();
   typedef _T* (*fpClassForFixtureCreateDefWithFlags)(const TSTflags& Flags);
   
   
   //does not construct the instance, uses the instance pointer from
   //the parent fixture
   TSTtestFixture(const COLstring& TestName, TSTtestFixture<_T>* ipParentFixture)
            : TSTtestCollection(TestName),
              pInstance(NULL),
              pGivenInstance(NULL),
              pCreateFunc(NULL),
              pCreateFuncWithFlags(NULL),
              pParentFixture(ipParentFixture)
   {

   }

   
   TSTtestFixture(const COLstring& TestName, fpClassForFixtureCreateDef ipCreateFunc)
            : TSTtestCollection(TestName),
              pInstance(NULL), 
              pGivenInstance(NULL),
              pCreateFunc(ipCreateFunc),
              pCreateFuncWithFlags(NULL),
              pParentFixture(NULL)
   {

   }

   TSTtestFixture(const COLstring& TestName, fpClassForFixtureCreateDefWithFlags ipCreateFuncWithFlags)
            : TSTtestCollection(TestName),
              pInstance(NULL),
              pGivenInstance(NULL),
              pCreateFunc(NULL),
              pCreateFuncWithFlags(ipCreateFuncWithFlags),
              pParentFixture(NULL)
   {

   }

   TSTtestFixture(const COLstring& TestName, _T* ipGivenInstance)
            : TSTtestCollection(TestName),
              pInstance(NULL),
              pGivenInstance(ipGivenInstance),
              pCreateFunc(NULL),
              pCreateFuncWithFlags(NULL),
              pParentFixture(NULL)
   {

   }
   

   virtual ~TSTtestFixture()
   {
      if (pCreateFunc || pCreateFuncWithFlags) //delete only if owner.
      {
         delete pInstance;
      }
      if(pGivenInstance)
      {
         delete pGivenInstance;
      }
   }

   virtual TSTtype& testType() const
   {
      return TSTtypeTestFixture::instance();
   }

   void addTestMethod(TSTtestMethod<_T>* pNewMethod)
   {
      TSTtestCollection::addTest(pNewMethod);
   }

   //the runTest function is used to set up the instance that will be used 
   //by any underlying TSTtestMethods
   virtual void runTest(const TSTflags& Flags)
   {
      //construct if needed
      if(pGivenInstance)
      {
         // if we have a given instance, we just use it
         pInstance = pGivenInstance;
      }
      else if (pCreateFunc)
      {
         delete pInstance;
         pInstance = (*pCreateFunc)();
      }
      else if (pCreateFuncWithFlags)
      {
         delete pInstance;
         pInstance = (*pCreateFuncWithFlags)(Flags);
      }
      else //obtain instance from parent
      {
         COLPRECONDITION(pParentFixture != NULL)
         pInstance = pParentFixture->pInstance;
         COLPRECONDITION(pInstance != NULL);
      }
      for (COLuint32 TestIndex = 0; TestIndex < countOfTest(); TestIndex++)
      {
         TSTtestable* pTest = test(TestIndex);
         if (&pTest->testType() == &TSTtypeTestMethod<_T>::instance())
         {
            TSTtestMethod<_T>* pTestMethod = (TSTtestMethod<_T>*)pTest;
            pTestMethod->setInstance(pInstance);
         }
      }
   }

   virtual void runSetUp(const TSTflags& Flags)
   {
      COLPRECONDITION(pInstance); // should have been initialized in runTest()
      TSTtestable* pSetUp = setUp();
      if (&pSetUp->testType() == &TSTtypeTestSetUpMethod<_T>::instance())      
      {
         TSTtestSetUpMethod<_T>* pSetUpMethod = (TSTtestSetUpMethod<_T>*)pSetUp;
         pSetUpMethod->setInstance(pInstance);
      }
      pSetUp->runTest(Flags);
   }

   virtual void runTearDown(const TSTflags& Flags)
   {
      COLPRECONDITION(pInstance); // should have been initialized in runTest()
      TSTtestable* pTearDown = tearDown();
      if (&pTearDown->testType() == &TSTtypeTestTearDownMethod<_T>::instance())      
      {
         TSTtestTearDownMethod<_T>* pTearDownMethod = (TSTtestTearDownMethod<_T>*)pTearDown;
         pTearDownMethod->setInstance(pInstance);
      }
      pTearDown->runTest(Flags);
   }
   
   virtual TSTtestable* clone() const
   {
      COLauto< TSTtestFixture<_T> > pNewFixture = new TSTtestFixture<_T>(testName(),pParentFixture);
      //a bit cludgy here
      pNewFixture->pInstance = pInstance;
      pNewFixture->pCreateFunc = pCreateFunc;
      pNewFixture->pCreateFuncWithFlags = pCreateFuncWithFlags;
      TSTtestCollection::copyTo(*pNewFixture);
      pNewFixture.setIsOwner(false);
      return pNewFixture;
   }

private:
   TSTtestFixture<_T>* pParentFixture;
   fpClassForFixtureCreateDef pCreateFunc;
   fpClassForFixtureCreateDefWithFlags pCreateFuncWithFlags;

   _T* pGivenInstance;
   _T* pInstance;
   TSTtestFixture(const TSTtestFixture& Orig);
   TSTtestFixture& operator=(const TSTtestFixture& Orig);
};

#endif // end of defensive include

