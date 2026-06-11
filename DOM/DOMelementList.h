#ifndef __DOM_ELEMENT_LIST_H__
#define __DOM_ELEMENT_LIST_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2004 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DOMelementList
//
// Description:
//
// Collection of DOMelements that can be passed safely by value.
//
// Author: Rob Moyse
// Date:   November 10, 2004
//
//
//---------------------------------------------------------------------------

class DOMelement;
class DOMelementListPrivate;

class DOMelementList
{
public:

   DOMelementList();
   virtual ~DOMelementList();

   void add(DOMelement* Element);
   DOMelement* element(COLuint32 Index) const;

   COLuint32 countOfElement() const;

   DOMelement* operator [](COLindex Index);

   DOMelementList(const DOMelementList& Orig);
   DOMelementList& operator=(const DOMelementList& Orig);

private:

   DOMelementListPrivate* pMember;

};

#endif // end of defensive include
