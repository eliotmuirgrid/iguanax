//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DOMelement
//
// Description:
//
// By far the vast majority of objects (apart from text) that authors 
// encounter when traversing a document are Element nodes. Assume 
// the following XML document: 
//
// <elementExample id="demo">
//    <subelement1/>
//    <subelement2><subsubelement/></subelement2>
// </elementExample>  
//
// When represented using DOM, the top node is an Element node for 
// "elementExample", which contains two child Element nodes, one for 
// "subelement1" and one for "subelement2". "subelement1" contains
// no child nodes.
//
// Elements may have attributes associated with them; since the 
// Element object inherits from Node, the generic Node object 
// attribute methods may be used to retrieve attributes
// for an element. There are methods on the Element interface to 
// retrieve either an Attribute object by name or an attribute 
// value by name. 
//
// Based on Document Object Model (Core) Level 1
// http://www.w3.org/TR/1998/REC-DOM-Level-1-19981001/level-one-core.html
// Author: Rob Moyse
// Date:   November 8, 2004
//
//
//---------------------------------------------------------------------------
#include "DOMpreCompiled.h"
#pragma hdrstop

#include <COL/COLarray.h>
#include <COL/COLref.h>

#include <DOM/DOMattribute.h>
#include <DOM/DOMelement.h>

class DOMelementPrivate
{
public:
   DOMelementPrivate() {}
   virtual ~DOMelementPrivate() {}

   const char* pTagName;
   COLuint32 TagNameLength;
   COLstring TagNameData;

   COLarray< COLref<DOMattribute> > Attributes;
};

DOMelement::DOMelement
(
   DOMnode* pParentNode, 
   DOMdocument* pDocument,
   const char* pTagName,
   COLuint32 TagNameLength
) 
: DOMnode(pParentNode, pDocument)
{
   pMember = new DOMelementPrivate();

   pMember->pTagName = pTagName;
   pMember->TagNameLength = TagNameLength;
}

DOMelement::DOMelement
(
   DOMnode* pParentNode, 
   DOMdocument* pDocument,
   const COLstring& TagName
)
: DOMnode(pParentNode, pDocument)
{
   pMember = new DOMelementPrivate();

   pMember->TagNameData = TagName;
   pMember->pTagName = pMember->TagNameData.c_str();
   pMember->TagNameLength = TagName.size();
}

DOMelement::~DOMelement()
{
   delete pMember;
}


DOMattribute* DOMelement::attribute(COLuint32 AttributeIndex) const
{
   if (AttributeIndex < pMember->Attributes.size())
   {
      return pMember->Attributes[AttributeIndex].get();
   }
   return NULL;
}

COLstring DOMelement::attribute(const COLstring& Name) const
{
   COLuint32 AttributeIndex = attributeIndex(Name);
   if (AttributeIndex < pMember->Attributes.size())
   {
      return pMember->Attributes[AttributeIndex]->nodeValue();
   }
   return "";
}

DOMattribute* DOMelement::attributeNode(const COLstring& Name) const
{
   COLuint32 AttributeIndex = attributeIndex(Name);
   if (AttributeIndex < pMember->Attributes.size())
   {
      return pMember->Attributes[AttributeIndex].get();
   }
   return NULL;
}

COLuint32 DOMelement::countOfAttribute() const
{
   return pMember->Attributes.size();
}

bool DOMelement::hasAttributes() const
{
   if (pMember->Attributes.size() > 0)
   {
      return true;
   }
   return false;
}

void DOMelement::removeAttribute(const COLstring& Name)
{
   COLuint32 AttributeIndex = attributeIndex(Name);
   if (AttributeIndex < pMember->Attributes.size())
   {
      pMember->Attributes.remove(AttributeIndex);
   }
}

void DOMelement::setAttribute(const char* pName, COLuint32 NameLength, const char* pValue, COLuint32 ValueLength)
{
   COLref<DOMattribute>& pNewAttribute = pMember->Attributes.push_back
      (
         new DOMattribute(ownerDocument(), pName, NameLength)
      );
   pNewAttribute->setNodeValue(pValue, ValueLength);
}

void DOMelement::setAttribute(const COLstring& Name, const COLstring& Value)
{
   COLref<DOMattribute>& pNewAttribute = pMember->Attributes.push_back
      (
         new DOMattribute(ownerDocument(), Name)
      );
   pNewAttribute->setNodeValue(Value);
}

void DOMelement::insertAttribute(COLuint32 AttributeIndex, const COLstring& Name, const COLstring& Value)
{
   pMember->Attributes.insert(new DOMattribute(ownerDocument(), Name), AttributeIndex);
   pMember->Attributes[AttributeIndex]->setNodeValue(Value);
}

DOMattribute* DOMelement::setAttributeNode(DOMattribute* pAttribute)
{
   return pMember->Attributes.push_back(pAttribute).get();
}

COLuint32 DOMelement::attributeIndex(const COLstring& Name) const
{
   COLuint32 AttributeIndex = 0;
   for (; AttributeIndex < pMember->Attributes.size(); AttributeIndex++)
   {
      if (pMember->Attributes[AttributeIndex]->nodeName() == Name)
      {
         return AttributeIndex;
      }
   }
   return AttributeIndex;
}

void DOMelement::setOwnerDocument(DOMdocument* pOwnerDocument)
{
   DOMnode::setOwnerDocument(pOwnerDocument);

   // Also set attributes' owner documents.
   COLuint32 CountOfAttribute = pMember->Attributes.size();
   for (COLuint32 AttributeIndex = 0; AttributeIndex < CountOfAttribute; AttributeIndex++)
   {
      pMember->Attributes[AttributeIndex]->setOwnerDocument(pOwnerDocument);
   }
}

const char* DOMelement::nodeName(COLuint32* pLength) const
{
   *pLength = pMember->TagNameLength;
   return pMember->pTagName;
}

COLstring DOMelement::nodeName() const
{
   return COLstring(pMember->pTagName, pMember->TagNameLength);
}

DOMnode::DOMnodeType DOMelement::nodeType() const
{
   return DOMnode::ElementNode;
}

void DOMelement::streamNode(COLostream& OutputStream) const{
   OutputStream << '<' << nodeName();
   for (COLuint32 AttributeIndex = 0; AttributeIndex < countOfAttribute(); AttributeIndex++){
      OutputStream << ' '; 
      attribute(AttributeIndex)->streamNode(OutputStream);
   }
   OutputStream << '>';
   for (COLuint32 ChildIndex = 0; ChildIndex < countOfChild(); ChildIndex++){
      childNode(ChildIndex)->streamNode(OutputStream);
   }
   OutputStream << COL_T("</") << nodeName() << '>';   
}
