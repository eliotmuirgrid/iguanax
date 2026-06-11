//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TSTtestableOptions
//
// Description:
//
// Implementation
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

class TSTtestableOptionsPrivate
{
public:
   TSTtestableOptionsPrivate() : RunAll(true){};
   virtual ~TSTtestableOptionsPrivate(){};
   COLarray<COLstring> SubTests;
   bool RunAll;
};


TSTtestableOptions::TSTtestableOptions()
{
   pMember = new TSTtestableOptionsPrivate;  
}

TSTtestableOptions::~TSTtestableOptions()
{
   delete pMember;
}

void TSTtestableOptions::clear()
{
   pMember->SubTests.clear();
   pMember->RunAll = true;
}

void TSTtestableOptions::setRunAll(bool Enable)
{
   pMember->RunAll = Enable;
}

bool TSTtestableOptions::runAll() const
{
   return pMember->RunAll;
}

void TSTtestableOptions::addSubTest(const COLstring& SubTest)
{
   pMember->SubTests.push_back(SubTest);
}

COLuint32 const TSTtestableOptions::countOfSubTest() const
{
   return pMember->SubTests.size();
}

const COLstring& TSTtestableOptions::subTest(COLuint32 TestIndex) const
{
   return pMember->SubTests[TestIndex];
}
