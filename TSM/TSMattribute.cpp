//---------------------------------------------------------------------------
// Copyright (C) 1997-2007 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TSMattribute
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Monday, February 12th, 2007 @ 12:30:45 PM
//
//---------------------------------------------------------------------------
#include "TSMprecomp.h"
#pragma hdrstop

#include "TSMattribute.h"

TSMattribute::TSMattribute()
{
}

TSMattribute::~TSMattribute()
{
}

const TSMlabel& TSMattribute::name() const
{
   return Name;
}

TSMlabel& TSMattribute::name()
{
   return Name;
}

const TSMvariant& TSMattribute::value() const
{
   return Value;
}

TSMvariant& TSMattribute::value()
{
   return Value;
}

COLostream& operator<<(COLostream& Stream, const TSMattribute& Attr){
   Stream << COL_T("Attribute: ") << Attr.name() << COL_T(" = ") << Attr.value();   
   return Stream;
}
