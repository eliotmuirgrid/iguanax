#ifndef __TST_TESTABLE_H__ 
#define __TST_TESTABLE_H__
//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TSTtestable
//
// Description:
//
// The base interface that represents something that can be run as a test. 
//
// Author: Nasron Cheong
// Date:   Feb 2004 
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

class COLstring;
class TSTresult;
class TSTtype;
class TSTtestablePrivate;
class TSTflags;

class TSTtestable
{
public:
   TSTtestable(const COLstring& TestName);
   virtual ~TSTtestable();

   virtual TSTtype& testType() const;
   const COLstring& testName() const;

   //do nothing if succeeded, a TSTexception should be thrown if failed
   virtual void runTest(const TSTflags& Flags) = 0;

   // returns a deep copy of itself dynamically...must take ownership
   // this throws by default
   virtual TSTtestable* clone() const;

private:
   TSTtestablePrivate* pMember;
   TSTtestable(const TSTtestable& Orig); 
   TSTtestable& operator=(const TSTtestable& Orig); 
};

#endif // end of defensive include



