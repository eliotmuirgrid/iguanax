//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TSMattributeList
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Monday, February 12th, 2007 @ 12:30:47 PM
//
//---------------------------------------------------------------------------
#include "TSMprecomp.h"
#pragma hdrstop

#include "TSMattributeList.h"

TSMattributeList::TSMattributeList()
: CountOfAttribute(0)
{
}

TSMattributeList::~TSMattributeList()
{
}

void TSMattributeList::addAttribute(const TSMattribute& NewAttribute)
{
   if (AttributeArray.size() == CountOfAttribute)
   {
      AttributeArray.push_back(NewAttribute);
   }
   else
   {
      AttributeArray[CountOfAttribute] = NewAttribute;
   }
   CountOfAttribute++;
}

TSMattribute& TSMattributeList::addAttribute()
{
   if (AttributeArray.size() == CountOfAttribute)
   {
      AttributeArray.push_back();
   }
   CountOfAttribute++;
   return AttributeArray[CountOfAttribute-1];
}

TSMattribute& TSMattributeList::attribute(COLuint32 AttributeIndex)
{
   COLPRECONDITION(AttributeIndex < CountOfAttribute);
   return AttributeArray[AttributeIndex];
}

const TSMattribute& TSMattributeList::attribute(COLuint32 AttributeIndex) const
{
   COLPRECONDITION(AttributeIndex < CountOfAttribute);
   return AttributeArray[AttributeIndex];
}

