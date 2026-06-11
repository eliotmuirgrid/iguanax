#ifndef __TST_TEST_METHOD_H__
#define __TST_TEST_METHOD_H__
//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TSTtestMethod
//
// Description: A testable class that uses a method of a class. It is templated
// and fits into a TSTtestFixture as an element.
//
// Author: Nasron Cheong
// Date:   Feb 2004 
//
//---------------------------------------------------------------------------

#include <TST/TSTtestable.h>
#include <TST/TSTtype.h>

template <class _T>
class TSTtestMethod : public TSTtestable
{
public:
   typedef void (_T::*fpTestMethodDef)();

   // Hopefully using constructor overloading here isn't going to screw us - Eliot
   typedef void (_T::*fpTestMethodDefWithFlags)(const TSTflags& );

   TSTtestMethod(const COLstring& TestName,
                 fpTestMethodDef ipMethodPointer)
      : TSTtestable(TestName), 
        pMethodPointer(ipMethodPointer),
        pMethodPointerWithFlags(NULL),
        pInstance(NULL)
   {
   }

   TSTtestMethod(const COLstring& TestName,
                 fpTestMethodDefWithFlags ipMethodPointerWithFlags)
      : TSTtestable(TestName), 
        pMethodPointer(NULL),
        pMethodPointerWithFlags(ipMethodPointerWithFlags),
        pInstance(NULL)
   {
   }

   virtual ~TSTtestMethod()
   {
   };

   void setInstance(_T* ipInstance)
   {
      pInstance = ipInstance;
   }

   virtual TSTtype& testType() const
   {
      return TSTtypeTestMethod< _T >::instance();
   }

   virtual void runTest(const TSTflags& Flags)
   {
      COLPRECONDITION(pInstance != NULL);
      if (pMethodPointer)
      {
         (*pInstance.*pMethodPointer)();
      }
      else
      {
         COLPRECONDITION(pMethodPointerWithFlags != NULL);
         (*pInstance.*pMethodPointerWithFlags)(Flags);      
      }   
   }

   virtual TSTtestable* clone() const
   {
      TSTtestMethod<_T>* pNewMethod = new TSTtestMethod<_T>(testName(),pMethodPointer);
      pNewMethod->pMethodPointerWithFlags = pMethodPointerWithFlags;
      pNewMethod->pInstance = pInstance;
      return pNewMethod;
   }

private:
   _T* pInstance; //does not own
   fpTestMethodDef          pMethodPointer;
   fpTestMethodDefWithFlags pMethodPointerWithFlags;
};

#endif   // end of defensive include
