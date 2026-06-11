//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TSTtestable
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

#include "TSTtestable.h"

class TSTtestablePrivate
{
public:
   TSTtestablePrivate(){};
   virtual ~TSTtestablePrivate(){};
   COLstring TestName;
};

TSTtestable::TSTtestable(const COLstring& iTestName)
{
   pMember = new TSTtestablePrivate;
   pMember->TestName = iTestName;
}

TSTtestable::~TSTtestable()
{
   delete pMember;
}

const COLstring& TSTtestable::testName() const
{
   return pMember->TestName;
}

TSTtype& TSTtestable::testType() const
{
   return TSTtypeTestable::instance();
}

TSTtestable* TSTtestable::clone() const
{
   COL_ERROR_STREAM("clone() of class derived from TSTtestable SHOULD be implemented.",COLerror::PreCondition);
}
