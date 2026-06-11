//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DOMnode
//
// Description:
//
// The DOMnode is the primary base datatype for the entire Document 
// Object Model. It represents a single node in the document tree. 
// While all objects subclass DOMnode interface expose methods for 
// dealing with children, not all objects subclassing the DOMnode 
// interface may have children. For example, Text nodes may not 
// have children.
//
// The methods to access nodeName, nodeValue and attributes are 
// included as a mechanism to get at node information without
// casting down to the specific derived interface. In cases where
// there is no obvious mapping of these attributes for a specific 
// nodeType (e.g., nodeValue for an Element or attributes for a
// Comment), this returns null. Note that the specialized classes
// may contain additional and more convenient mechanisms to get
// and set the relevant information. 
//
// Based on Document Object Model (Core) Level 1
// http://www.w3.org/TR/1998/REC-DOM-Level-1-19981001/level-one-core.html
//
// Author: Rob Moyse
// Date:   November 8, 2004
//
//
//---------------------------------------------------------------------------

#include "DOMpreCompiled.h"
#pragma hdrstop

#include <COL/COLarray.h>
#include <COL/COLref.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#include "DOMelementList.h"
#include "DOMnode.h"

class DOMnodePrivate
{
public:
   DOMnodePrivate() {}
   virtual ~DOMnodePrivate() {}

   DOMnode* pParent;
   DOMdocument* pDocument;

   COLarray< COLref<DOMnode> > Children;
};

DOMnode::DOMnode(DOMnode* pParentNode, DOMdocument* pDocument)
{
   pMember = new DOMnodePrivate;
   pMember->pParent = pParentNode;
   pMember->pDocument = pDocument;
}

DOMnode::~DOMnode()
{
   delete pMember;
}

DOMnode* DOMnode::parentNode() const
{
   return pMember->pParent;
}

DOMnode* DOMnode::nextSibling() const
{
   if (pMember->pParent)
   {
      return pMember->pParent->childAfter(this);
   }

   return NULL;
}
DOMnode* DOMnode::previousSibling() const
{
   if (pMember->pParent)
   {
      return pMember->pParent->childBefore(this);
   }

   return NULL;
}

DOMnode* DOMnode::appendChild(DOMnode* pNewChild) {
   COL_METHOD(DOMnode::appendChild);
   COL_VAR(pNewChild);
   return pMember->Children.push_back(pNewChild).get();
}

DOMnode* DOMnode::insertBefore(DOMnode* pReferenceChild,DOMnode* pNewChild)
{
   COL_METHOD(DOMnode::insertBefore);
   COL_VAR2(pNewChild, pReferenceChild);
   COLuint32 ChildIndex = childIndex(pReferenceChild);
   if (ChildIndex < pMember->Children.size())
   {
      pMember->Children.insert(pNewChild, ChildIndex);
      return pNewChild;
   }

   return pMember->Children.push_back(pNewChild).get();
}

void DOMnode::removeChild(DOMnode* pChild) {
   COL_METHOD(DOMnode::removeChild);
   COL_VAR(pChild);
   COLuint32 ChildIndex = childIndex(pChild);
   if (ChildIndex < pMember->Children.size()) {
      pMember->Children.remove(ChildIndex);
   }
}

DOMnode* DOMnode::replaceChild(DOMnode* pNewChild, DOMnode* pOldChild)
{
   COLuint32 ChildIndex = childIndex(pOldChild);
   if (ChildIndex < pMember->Children.size())
   {
      pMember->Children.remove(ChildIndex);
      pMember->Children.insert(pNewChild, ChildIndex);
      return pNewChild;
   }

   return pMember->Children.push_back(pNewChild).get();
}

DOMnode* DOMnode::childAfter(const DOMnode* pReferenceChild) const
{
   COLuint32 ChildIndex = childIndex(pReferenceChild);
   if (ChildIndex < pMember->Children.size()-1)
   {
      return pMember->Children[ChildIndex+1].get();
   }

   return NULL;
}

DOMnode* DOMnode::childBefore(const DOMnode* pReferenceChild) const
{
   COLuint32 ChildIndex = childIndex(pReferenceChild);
   if ( (ChildIndex < pMember->Children.size()) && (ChildIndex > 0) )
   {
      return pMember->Children[ChildIndex-1].get();
   }

   return NULL;
}

DOMnode* DOMnode::childNode(COLuint32 ChildIndex) const
{

   if ( ChildIndex >= pMember->Children.size() )
   {
      COL_ERROR_STREAM(COL_T("DOMnode child index [") << ChildIndex << COL_T("] out of bounds for node ") << nodeName() << '.',
                        COLerror::PreCondition);      

   }
   return pMember->Children[ChildIndex].get();
}

COLuint32 DOMnode::countOfChild() const
{
   return pMember->Children.size();
}

bool DOMnode::hasChildren() const
{
   if (pMember->Children.size() > 0)
   {
      return true;
   }

   return false;
}


COLuint32 DOMnode::childIndex(const DOMnode* pReferenceChild) const
{
   COLuint32 ChildIndex = 0;
   for (; ChildIndex < pMember->Children.size(); ChildIndex++)
   {
      if ( pMember->Children[ChildIndex].get() == pReferenceChild )
      {
         return ChildIndex;
      }
   }
   return ChildIndex;
}

COLuint32 DOMnode::countOfAttribute() const
{
   return 0;
}

bool DOMnode::hasAttributes() const
{
   return false;
}

DOMdocument* DOMnode::ownerDocument()
{
   COL_METHOD(DOMnode::ownerDocument);
   COL_VAR(pMember->pDocument);
   return pMember->pDocument;
}

void DOMnode::setOwnerDocument(DOMdocument* pOwnerDocument)
{
   pMember->pDocument = pOwnerDocument;
   
   // Recursively set all children's owner documents.
   COLuint32 CountOfChild = pMember->Children.size();
   for (COLuint32 ChildIndex = 0; ChildIndex < CountOfChild; ChildIndex++)
   {
      pMember->Children[ChildIndex]->setOwnerDocument(pOwnerDocument);
   }
}

void DOMnode::setParent(DOMnode* pParent)
{
   pMember->pParent = pParent;
}

COLstring DOMnode::nodeValue() const
{
   return "";
}

DOMelementList DOMnode::elementsByTagName(const COLstring& TagName)
{
   DOMelementList ElementList;
   elementsByTagName(TagName, ElementList);
   return ElementList;
}

void DOMnode::elementsByTagName(const COLstring& TagName, DOMelementList& ElementList)
{

   if ((nodeName() == TagName) && (nodeType() == DOMnode::ElementNode))
   {
      ElementList.add((DOMelement*)this);
   }

   for (COLuint32 ChildIndex = 0; ChildIndex < countOfChild(); ChildIndex++)
   {
      childNode(ChildIndex)->elementsByTagName(TagName, ElementList);
   }

}

COLostream& operator<<(COLostream& Stream, const DOMnode& Node){
   Node.streamNode(Stream);
   return Stream;
}

