//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DOMutils
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Friday, September 25th, 2009 @ 03:57:23 PM
//
//---------------------------------------------------------------------------
#include "DOMpreCompiled.h"
#pragma hdrstop

#include "DOMutils.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

#include "DOMnode.h"
#include "DOMattribute.h"
#include "DOMelementList.h"
#include "DOMelement.h"

COLstring DOMattributeValue(DOMnode& Node, const COLstring& Name){
   for (int i = 0; i < Node.countOfAttribute(); i++){
      if (Node.attribute(i)->nodeName() == Name){
         return Node.attribute(i)->nodeValue(); 
      }
   }
   return COLstring();
}

void DOMimmediateElementListByTagName(DOMnode& Node, const COLstring& FullNameSpace, const COLstring& Name, DOMelementList* List){
   for (int i = 0; i < Node.countOfChild(); i++){
      DOMnode* pChildNode = Node.childNode(i);
      if (DOMisNodeNamedElement(pChildNode, FullNameSpace, Name)){
         List->add(dynamic_cast<DOMelement*>(pChildNode));
      }
   }   
}

bool DOMisNodeNamedElement(DOMnode* pNode, const COLstring& FullNameSpace, const COLstring& Name){
   COL_FUNCTION(DOMisNodeNamedElement);
   COL_VAR(pNode->nodeName());

   if (pNode->nodeType() != DOMnode::ElementNode){
      COL_TRC("Not element node.");
      return false;
   }
   COLstring ShortNameSpace, UName;
   if (pNode->nodeName().split(ShortNameSpace, UName, ":")){
      if (UName != Name){
         return false;
      }
   }

   if (FullNameSpace != DOMnameSpace(pNode)){
      return false;
   }
   return true;
}

COLstring DOMextractElementUName(DOMnode* pNode, const COLstring& FullNameSpace)
{
   if (pNode->nodeType() != DOMnode::ElementNode)
   {
      return COLstring();
   }
   // TODO - could optimize here since we split the string twice
   if (FullNameSpace != DOMnameSpace(pNode))
   {
      return COLstring();
   }
   
   COLstring ShortNameSpace, UName;
   pNode->nodeName().split(ShortNameSpace, UName, ":");
   return UName;
}


COLstring DOMfindParentAttributeValueWithName(DOMnode* pNode, const COLstring& AttributeName){
   DOMnode* pCurrentNode = pNode;
   while (pCurrentNode != NULL){
      for (int i=0; i < pCurrentNode->countOfAttribute(); i++){
         if (pCurrentNode->attribute(i)->nodeName() == AttributeName){
            return pCurrentNode->attribute(i)->nodeValue();
         }
      }
      pCurrentNode = pCurrentNode->parentNode();
   }
   return COLstring();
}


COLstring DOMnameSpace(DOMnode* pNode){
   COLstring NodeName = pNode->nodeName();
   COLstring ShortNameSpace, Name;
   if (NodeName.split(ShortNameSpace, Name, ":")){
      // we have an element with a qualified name space
      return DOMfindParentAttributeValueWithName(pNode, "xmlns:" + ShortNameSpace);
   } else {
      return DOMfindParentAttributeValueWithName(pNode, "xmlns");
   }
   return ShortNameSpace;
}

DOMnode* DOMfindChildWithName(DOMnode* pNode, const COLstring& Name){
   for (int i = 0; i < pNode->countOfChild(); i++){
      if (pNode->childNode(i)->nodeName() == Name){
         return pNode->childNode(i);
      }
   }
   return NULL;
}

DOMnode* DOMfindChildWithFullName(DOMnode* pNode, const COLstring& FullNameSpace, const COLstring& Name){
   // TODO - this routine is *really* inefficient.  But emphasis at this point is to "GET IT WORKING!" - Eliot
   for (int i = 0; i < pNode->countOfChild(); i++){
      DOMnode* pChildNode = pNode->childNode(i);
      if (DOMisNodeNamedElement(pChildNode, FullNameSpace, Name)){
         return pChildNode;
      }
   }
   return NULL;
}

DOMnode* DOMfindLastElementChild(DOMnode* pParent){
   COL_FUNCTION(DOMfindFirstElementChild);
   COLuint32 CountOfChild = pParent->countOfChild();
   for (int i=CountOfChild - 1; i >= 0; i--){
      if (pParent->childNode(i)->nodeType() == DOMnode::ElementNode){
         return pParent->childNode(i);
      }
   }
   return NULL;
}

DOMnode* DOMfindFirstElementChild(DOMnode* pParent){
   COL_FUNCTION(DOMfindFirstElementChild);
   COLuint32 CountOfChild = pParent->countOfChild();
   for (int i=0; i < CountOfChild; i++){
      if (pParent->childNode(i)->nodeType() == DOMnode::ElementNode){
         return pParent->childNode(i);
      }
   }
   return NULL;
}

COLstring DOMunbalancedTagWarning(const DOMdocument& Document, const DOMsink& Sink){
   int CountOfUnbalancedNodes = Sink.countOfStartElement() - Sink.countOfEndElement();
   COL_VAR(CountOfUnbalancedNodes);
   DOMnode* pNode = Document.childNode(0);
   COLstring Address;
   for (int i=0; i < CountOfUnbalancedNodes; i++){
      if (pNode){
         Address += pNode->nodeName() + " ";
         pNode = DOMfindLastElementChild(pNode);
      }
   }
   COL_VAR(Address);
   COLstring Error;
   COLostream ErrorStream(Error);

   if (CountOfUnbalancedNodes == 1){
      ErrorStream << Address << "tag was not closed.";
   } else {
      ErrorStream << CountOfUnbalancedNodes << " tags were not closed.  Tags affected: " << Address;
   }
   return Error;
}
