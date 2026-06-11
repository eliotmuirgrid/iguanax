#ifndef __TST_TEST_TEAR_DOWN_METHOD_H__
#define __TST_TEST_TEAR_DOWN_METHOD_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TSTtestTearDownMethod
//
// Description:
//
// TSTtestTearDownMethod class.
//
// Author: Joanna Karlic
// Date:   Tuesday, May 6th, 2008 @ 12:00:48 PM
//
//---------------------------------------------------------------------------

#include <TST/TSTtestable.h>
#include <TST/TSTtype.h>

template <class _T>
class TSTtestTearDownMethod : public TSTtestable
{
public:
   typedef void (_T::*fpTestTearDownMethodDef)();

   typedef void (_T::*fpTestTearDownMethodDefWithFlags)(const TSTflags& );

   TSTtestTearDownMethod(const COLstring& TestName,
                 fpTestTearDownMethodDef ipTearDownMethodPointer)
      : TSTtestable(TestName), 
        pTearDownMethodPointer(ipTearDownMethodPointer),
        pTearDownMethodPointerWithFlags(NULL),
        pInstance(NULL)
   {
   }

   virtual ~TSTtestTearDownMethod()
   {
   };

   void setInstance(_T* ipInstance)
   {
      pInstance = ipInstance;
   }

   virtual TSTtype& testType() const
   {
      return TSTtypeTestTearDownMethod< _T >::instance();
   }

   virtual void runTest(const TSTflags& Flags)
   {
      runTearDown(Flags);
   }

   virtual void runTearDown(const TSTflags& Flags)
   {
      COLPRECONDITION(pInstance != NULL);
      if (pTearDownMethodPointer)
      {
         (*pInstance.*pTearDownMethodPointer)();
      }
   }

   virtual TSTtestable* clone() const
   {
      TSTtestTearDownMethod<_T>* pNewTearDownMethod = new TSTtestTearDownMethod<_T>(testName(),pTearDownMethodPointer);
      pNewTearDownMethod->pTearDownMethodPointerWithFlags = pTearDownMethodPointerWithFlags;
      pNewTearDownMethod->pInstance = pInstance;
      return pNewTearDownMethod;
   }

private:
   _T* pInstance; //does not own
   fpTestTearDownMethodDef          pTearDownMethodPointer;
   fpTestTearDownMethodDefWithFlags pTearDownMethodPointerWithFlags;
};

#endif   // end of defensive include
