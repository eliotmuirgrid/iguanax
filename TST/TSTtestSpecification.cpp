//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TSTtestSpecification
//
// Description:
//
// TSTtestSpecification is used along with TSTtestSpecificationParser 
// to convert an XSLT style string into a set of tests to be run. This allows 
// TSTtestRunner to run a subset of tests in a test collection instead of all tests.
//
// Author: Nasron Cheong
// Date:   March 2004 
//
//---------------------------------------------------------------------------

#include "TSTprecomp.h"
#pragma hdrstop

#include "TSTtestSpecification.h"
#include "TSTtestableOptions.h"
#include <COL/COLarray.h>

class TSTtestSpecificationPrivate
{
public:
   COLarray<COLstring> Path;
   TSTtestableOptions Options;
};


TSTtestSpecification::TSTtestSpecification()
{
   pMember = new TSTtestSpecificationPrivate;
}

TSTtestSpecification::~TSTtestSpecification()
{
   delete pMember;
}

void TSTtestSpecification::pushBackPath(const COLstring& TestableName)
{
   pMember->Path.push_back(TestableName);
}

COLuint32 TSTtestSpecification::pathDepth() const
{
   return pMember->Path.size();
}

const COLstring& TSTtestSpecification::pathItem(COLuint32 Depth) const
{
   return pMember->Path[Depth];
}

TSTtestableOptions& TSTtestSpecification::testOptions()
{
   return pMember->Options;
}

const TSTtestableOptions& TSTtestSpecification::testOptions() const
{
   return pMember->Options;
}

bool TSTtestSpecification::operator==(TSTtestSpecification& Rhs)
{
   //compare vector
   if (pathDepth() != Rhs.pathDepth())
   {
      return false;
   }
   COLindex PathIndex = 0;
   for (; PathIndex < pathDepth(); PathIndex++)
   {
      if (pathItem(PathIndex) != Rhs.pathItem(PathIndex))
      {
         return false;
      }
   }
   //compare options
   if (testOptions().countOfSubTest() != Rhs.testOptions().countOfSubTest())
   {
      return false;
   }
   if (testOptions().runAll() != Rhs.testOptions().runAll())
   {
      return false;
   }
   for (PathIndex = 0; PathIndex < testOptions().countOfSubTest(); PathIndex++)
   {
      if (testOptions().subTest(PathIndex) != Rhs.testOptions().subTest(PathIndex))
      {
         return false;
      }
   }
   return true; //equal
}

void TSTtestSpecification::clear()
{
   pMember->Options.clear();
   pMember->Path.clear();
}

