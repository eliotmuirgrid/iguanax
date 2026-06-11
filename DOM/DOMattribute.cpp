//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DOMattribute
//
// Description:
//
// The Attribute object represents an attribute in an Element object. 
// Typically the allowable values for the attribute are defined in a 
// document type definition.
//
// Attribute objects inherit the Node object, but since they are not
// actually child nodes of the element they describe, the DOM does
// not consider them part of the document tree. Thus, the Node
// attributes parentNode, previousSibling, and nextSibling have a
// null value for Attr objects. The DOM takes the view that 
// attributes are properties of elements rather than having a
// separate identity from the elements they are associated with; 
// this should make it more efficient to implement such features
// as default attributes associated with all elements of a given
// type. Users and implementors of the DOM need to be aware that 
// Attribute nodes have some things in common with other objects
// inheriting the Node interface, but they also are quite distinct.
//
// Based on Document Object Model (Core) Level 1
// http://www.w3.org/TR/1998/REC-DOM-Level-1-19981001/level-one-core.html
//
// Author: Rob Moyse
// Date:   November 9, 2004
//
//
//---------------------------------------------------------------------------
#include "DOMpreCompiled.h"
#pragma hdrstop

#include <DOM/DOMattribute.h>

class DOMattributePrivate
{
public:
   DOMattributePrivate() {}
   virtual ~DOMattributePrivate() {}

   const char* pName;
   const char* pValue;

   COLuint32 NameLength;
   COLuint32 ValueLength;

   COLstring NameData;
   COLstring ValueData;
};

DOMattribute::DOMattribute
(
   DOMdocument* pDocument,
   const char* pName,
   COLuint32 NameLength
) 
: DOMnode(NULL, pDocument)
{
   pMember = new DOMattributePrivate();

   pMember->pName = pName;
   pMember->NameLength = NameLength;
}

DOMattribute::DOMattribute
(
   DOMdocument* pDocument,
   const COLstring& Name
)
: DOMnode(NULL, pDocument)
{
   pMember = new DOMattributePrivate();

   pMember->NameData = Name;
   pMember->pName = pMember->NameData.c_str();
   pMember->NameLength = Name.size();
}

DOMattribute::~DOMattribute()
{
   delete pMember;
}

const char* DOMattribute::nodeValue(COLuint32* pLength) const
{
   *pLength = pMember->ValueLength;
   return pMember->pValue;
}

COLstring DOMattribute::nodeValue() const
{
   return COLstring(pMember->pValue, pMember->ValueLength);
}

void DOMattribute::setNodeValue(const char* Value, COLuint32 Length)
{
   pMember->pValue = Value;
   pMember->ValueLength = Length;
}

void DOMattribute::setNodeValue(const COLstring& Value)
{
   pMember->ValueData = Value;
   pMember->pValue = pMember->ValueData.c_str();
   pMember->ValueLength = pMember->ValueData.size();
}

const char* DOMattribute::nodeName(COLuint32* pLength) const
{
   *pLength = pMember->NameLength;
   return pMember->pName;
}

COLstring DOMattribute::nodeName() const
{
   return COLstring(pMember->pName, pMember->NameLength);
}

DOMnode::DOMnodeType DOMattribute::nodeType() const
{
   return DOMnode::AttributeNode;
}

void DOMattribute::streamNode(COLostream& OutputStream) const{
   OutputStream << nodeName() << COL_T("=\"") << nodeValue() << '\"'; 
}
