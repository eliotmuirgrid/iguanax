//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DOMelementList
//
// Description:
//
// Author: Rob Moyse
// Date:   November 10, 2004
//
//
//---------------------------------------------------------------------------
#include "DOMpreCompiled.h"
#pragma hdrstop

#include <COL/COLerror.h>
#include <COL/COLarray.h>

#include <DOM/DOMelement.h>
#include <DOM/DOMelementList.h>

class DOMelementListPrivate
{
public:

   COLarray< DOMelement* > Elements;
};

DOMelementList::DOMelementList(const DOMelementList& Orig)
{
   pMember = new DOMelementListPrivate;
   for (COLuint32 ElementIndex = 0; ElementIndex < Orig.countOfElement(); ElementIndex++)
   {
      pMember->Elements.push_back(Orig.element(ElementIndex));
   }
}

DOMelementList& DOMelementList::operator=(const DOMelementList& Orig)
{
   if (this != &Orig)
   {
      pMember = new DOMelementListPrivate;
      for (COLuint32 ElementIndex = 0; ElementIndex < Orig.countOfElement(); ElementIndex++)
      {
         pMember->Elements.push_back(Orig.element(ElementIndex));
      }
   }
   return *this;
}

DOMelementList::DOMelementList()
{
   pMember = new DOMelementListPrivate;
}

DOMelementList::~DOMelementList()
{
   delete pMember;
}

void DOMelementList::add(DOMelement* Element)
{
   pMember->Elements.push_back(Element);
}

DOMelement* DOMelementList::element(COLuint32 ElementIndex) const
{
   if (ElementIndex < pMember->Elements.size())
   {
      return pMember->Elements[ElementIndex];
   }
   else
   {
      COL_ERROR_STREAM(COL_T("DOMelementList index [") << ElementIndex << COL_T("] out of bounds."),
                        COLerror::PreCondition);      
   }

}

DOMelement* DOMelementList::operator [](COLindex ElementIndex)
{
   return element(ElementIndex);
}

COLuint32 DOMelementList::countOfElement() const
{
   return pMember->Elements.size();
}

