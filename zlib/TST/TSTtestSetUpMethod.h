#ifndef __TST_TEST_SET_UP_METHOD_H__
#define __TST_TEST_SET_UP_METHOD_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TSTtestSetUpMethod
//
// Description:
//
// TSTtestSetUpMethod class.
//
// Author: Joanna Karlic
// Date:   Tuesday, May 6th, 2008 @ 12:00:37 PM
//
//---------------------------------------------------------------------------

#include <TST/TSTtestable.h>
#include <TST/TSTtype.h>

template <class _T>
class TSTtestSetUpMethod : public TSTtestable
{
public:
   typedef void (_T::*fpTestSetUpMethodDef)();

   typedef void (_T::*fpTestSetUpMethodDefWithFlags)(const TSTflags& );

   TSTtestSetUpMethod(const COLstring& TestName,
                 fpTestSetUpMethodDef ipSetUpMethodPointer)
      : TSTtestable(TestName), 
        pSetUpMethodPointer(ipSetUpMethodPointer),
        pSetUpMethodPointerWithFlags(NULL),
        pInstance(NULL)
   {
   }

   virtual ~TSTtestSetUpMethod()
   {
   };

   void setInstance(_T* ipInstance)
   {
      pInstance = ipInstance;
   }

   virtual TSTtype& testType() const
   {
      return TSTtypeTestSetUpMethod< _T >::instance();
   }

   virtual void runTest(const TSTflags& Flags)
   {
      runSetUp(Flags);
   }

   virtual void runSetUp(const TSTflags& Flags)
   {
      COLPRECONDITION(pInstance != NULL);
      if (pSetUpMethodPointer)
      {
         (*pInstance.*pSetUpMethodPointer)();
      }
   }


   virtual TSTtestable* clone() const
   {
      TSTtestSetUpMethod<_T>* pNewSetUpMethod = new TSTtestSetUpMethod<_T>(testName(),pSetUpMethodPointer);
      pNewSetUpMethod->pSetUpMethodPointerWithFlags = pSetUpMethodPointerWithFlags;
      pNewSetUpMethod->pInstance = pInstance;
      return pNewSetUpMethod;
   }

private:
   _T* pInstance; //does not own
   fpTestSetUpMethodDef          pSetUpMethodPointer;
   fpTestSetUpMethodDefWithFlags pSetUpMethodPointerWithFlags;
};

#endif   // end of defensive include
