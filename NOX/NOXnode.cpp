//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NOXnode
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Tuesday, June 22nd, 2010 @ 03:05:08 PM
//
//---------------------------------------------------------------------------
#include "NOXprecomp.h"
#pragma hdrstop

#include "NOXnode.h"
#include "NOXparse.h"
#include "NOXutils.h"
#include "NOXsimple.h"

#include <NOD/NODoperation.h>

#include <DOM/DOMattribute.h>
#include <DOM/DOMdocument.h>
#include <DOM/DOMsink.h>
#include <DOM/DOMelement.h>
#include <DOM/DOMcdataSection.h>
#include <DOM/DOMtext.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

NOXnode::NOXnode(DOMnode* pDomNode, NOXcontext* pContext)
 : m_pDomNode(pDomNode),
   m_pContext(pContext),
   m_NameId(-1)
{
   resetSize();
}

NOXnode::~NOXnode(){
   for (COLlist<DOMdocument*>::iterator i = m_pParsedDocumentz.begin(); i != m_pParsedDocumentz.end(); ++i) {
      if (*i) delete *i;
   }
}

int NOXnode::nodeType() const
{
   if (m_pDomNode->nodeType() == DOMnode::DocumentNode){
      return NOX_DOCUMENT;
   } else {
      return NOX_ELEMENT;
   }
}

const COLstring& NOXnode::nodeTypeName() const
{
   if (m_pDomNode->nodeType() == DOMnode::DocumentNode){
      return NOXdocName;
   } else {
      return NOXelementName;
   }
}

const COLstring& NOXnode::name() const{
   if (m_NameId == -1){
      COLstring Name;
      if (m_pDomNode->nodeType() == DOMnode::DocumentNode){
         Name = "Document";
      } else {
         Name = m_pDomNode->nodeName();
      }
      m_NameId = m_pContext->idFromValue(Name);
   }
   return m_pContext->valueFromId(m_NameId);
}

bool NOXnode::isNamed() const{
   return m_pDomNode->nodeType() == DOMnode::ElementNode;
}

static void NOXclearChildren(DOMnode* pDomNode)
{
   while (pDomNode->countOfChild())
   {
      pDomNode->removeChild(pDomNode->childNode(0));
   }
}

bool NOXnode::setTreeValue(const COLstring& TreeValue)
{
   bool IsElement = m_pDomNode->nodeType() == DOMnode::ElementNode;
   m_pParsedDocumentz.push_back(new DOMdocument());
   DOMdocument* pThisParsedDoc = m_pParsedDocumentz.back();
   // We need to make sure the buffer is big enough to store "<elementName>"
   // and "</elementName>", if we have to to wrap the tree value.
   int ExtraBufferSpace = IsElement ? (name().size()*2 + 5) : 0;
   DOMsink Sink(*pThisParsedDoc, TreeValue.size() + ExtraBufferSpace);
   
   COLstring WrappedXml;
   try{
      if (IsElement){
         COLostream WrappedXmlStream(WrappedXml);
         WrappedXmlStream << '<' << name() << '>'
                          << TreeValue
                          << COL_T("</") << name() << '>';
         Sink.write(WrappedXml.c_str(), WrappedXml.size());
      } else {
         Sink.write(TreeValue.c_str(), TreeValue.size());
      }
   } catch (const XMLexpatParserError& Error){
      NOXrethrowExceptionWithSource(Error, IsElement ? WrappedXml : TreeValue);
   }
   
   NOXcleanDomTree(pThisParsedDoc);
   DOMnode* pRootNode = pThisParsedDoc->childNode(0);
   pRootNode->setOwnerDocument(m_pDomNode->ownerDocument());

   NOXclearChildren(m_pDomNode.get());
   if (IsElement){
      COLuint32 CountOfChild = pRootNode->countOfChild();
      for (COLuint32 ChildIndex = 0; ChildIndex < CountOfChild; ChildIndex++)
      {
         DOMnode* pChildNode = pRootNode->childNode(ChildIndex);
         pChildNode->setParent(m_pDomNode.get());
         m_pDomNode->appendChild(pChildNode);
      }
   } else {
      pRootNode->setParent(m_pDomNode.get());
      m_pDomNode->appendChild(pRootNode);
   }

   resetSize();

   return true;
}

void NOXnode::clear()
{
   for (int i = size()-1; i >= 0; i--)
   {
      NODplaceComposite::remove(i);
   }
}

void NOXnode::nullify(int i)
{
   NODplaceComposite::remove(i);
}

void NOXnode::nullify(const COLstring& Name)
{
   remove(Name);
}

NODplace& NOXnode::add(const char* Arg1, const char* Arg2){
   int IndexInserted = size();
   if (Arg1 == NULL){
      COL_ERROR_STREAM_PLAIN("type, name/data", NOD_BAD_ARGUMENTS);
   }
   else if (m_pDomNode->nodeType() != DOMnode::ElementNode){
      COL_ERROR_STREAM_PLAIN("Only element nodes can have children added to them.", COLerror::PreCondition);
   }
   else if (Arg1 == NOXelementName){
      if (Arg2 == NULL){
         COL_ERROR_STREAM_PLAIN("type, name", NOD_BAD_ARGUMENTS);
      }
      DOMnode* pElementNode = new DOMelement(m_pDomNode.get(), m_pDomNode->ownerDocument(), Arg2);
      m_pDomNode->appendChild(pElementNode);
   }
   else if (Arg1 == NOXattributeName){
      if (Arg2 == NULL){
         COL_ERROR_STREAM_PLAIN("type, name", NOD_BAD_ARGUMENTS);
      }
      IndexInserted = m_pDomNode->countOfAttribute();
      DOMelement* pThisElement = (DOMelement*)m_pDomNode.get();
      pThisElement->setAttribute(Arg2, COLstring());
   }
   else if (Arg1 == NOXcdataName){
      if (Arg2 == NULL){
         COL_ERROR_STREAM_PLAIN("type, data", NOD_BAD_ARGUMENTS);
      }
      DOMnode* pCdataNode = new DOMcdataSection(m_pDomNode.get(), m_pDomNode->ownerDocument(), Arg2);
      m_pDomNode->appendChild(pCdataNode);
   }
   else if (Arg1 == NOXtextName){
      if (Arg2 == NULL){
         COL_ERROR_STREAM_PLAIN("type, text", NOD_BAD_ARGUMENTS);
      }
      DOMnode* pTextNode = new DOMtext(m_pDomNode.get(), m_pDomNode->ownerDocument(), Arg2);
      m_pDomNode->appendChild(pTextNode);
   }
   else{
      COL_ERROR_STREAM_PLAIN("Unrecognized node type '" << Arg1
         << "'.  Supported types are 'element', 'attribute', 'cdata', and 'text'.", COLerror::PreCondition);
   }

   // If we get here, it means an element was successfully inserted at IndexInserted.
   insertChildCache(IndexInserted);
   return child(IndexInserted);
}

static void NOXinsertChildNode(DOMnode* pNode, int i, DOMnode* pChild){
   i -= pNode->countOfAttribute();
   COLPRECONDITION(i <= pNode->countOfChild());

   if (i == pNode->countOfChild()){
      pNode->appendChild(pChild);
   }
   else{
      DOMnode* pInsertBefore = pNode->childNode(i);
      pNode->insertBefore(pInsertBefore, pChild);
   }
}

NODplace& NOXnode::insert(int i, const char* Arg1, const char* Arg2){
   if (Arg1 == NULL){
      COL_ERROR_STREAM_PLAIN("type, name/data", NOD_BAD_ARGUMENTS);
   }
   else if (m_pDomNode->nodeType() != DOMnode::ElementNode){
      COL_ERROR_STREAM_PLAIN("Only element nodes can have children added to them.", COLerror::PreCondition);
   }
   else if (Arg1 == NOXelementName){
      if (Arg2 == NULL){
         COL_ERROR_STREAM_PLAIN("type, name", NOD_BAD_ARGUMENTS);
      }
      else if (i < 0 || i < m_pDomNode->countOfAttribute() || i > size()){
         COL_ERROR_STREAM_PLAIN("Index " << i << " is out of bounds.", NOD_INDEX_OUT_OF_BOUNDS);
      }
      DOMnode* pElementNode = new DOMelement(m_pDomNode.get(), m_pDomNode->ownerDocument(), Arg2);
      NOXinsertChildNode(m_pDomNode.get(), i, pElementNode);
   }
   else if (Arg1 == NOXattributeName){
      if (Arg2 == NULL){
         COL_ERROR_STREAM_PLAIN("type, name", NOD_BAD_ARGUMENTS);
      }
      else if (i < 0 || i > m_pDomNode->countOfAttribute()){
         COL_ERROR_STREAM_PLAIN("Index " << i << " is out of bounds.", NOD_INDEX_OUT_OF_BOUNDS);
      }
      DOMelement* pThisElement = (DOMelement*)m_pDomNode.get();
      pThisElement->insertAttribute(i, Arg2, COLstring());
   }
   else if (Arg1 == NOXcdataName){
      if (Arg2 == NULL){
         COL_ERROR_STREAM_PLAIN("type, data", NOD_BAD_ARGUMENTS);
      }
      else if (i < 0 || i < m_pDomNode->countOfAttribute() || i > size()){
         COL_ERROR_STREAM_PLAIN("Index " << i << " is out of bounds.", NOD_INDEX_OUT_OF_BOUNDS);
      }
      DOMnode* pCdataNode = new DOMcdataSection(m_pDomNode.get(), m_pDomNode->ownerDocument(), Arg2);
      NOXinsertChildNode(m_pDomNode.get(), i, pCdataNode);
   }
   else if (Arg1 == NOXtextName){
      if (Arg2 == NULL){
         COL_ERROR_STREAM_PLAIN("type, text", NOD_BAD_ARGUMENTS);
      }
      else if (i < 0 || i < m_pDomNode->countOfAttribute() || i > size()){
         COL_ERROR_STREAM_PLAIN("Index " << i << " is out of bounds.", NOD_INDEX_OUT_OF_BOUNDS);
      }
      DOMnode* pTextNode = new DOMtext(m_pDomNode.get(), m_pDomNode->ownerDocument(), Arg2);
      NOXinsertChildNode(m_pDomNode.get(), i, pTextNode);
   } else {
      COL_ERROR_STREAM_PLAIN("Unrecognized node type '" << Arg1
         << "'.  Supported types are 'element', 'attribute', 'cdata', and 'text'.", COLerror::PreCondition);
   }

   // If we get here, it means an element was successfully inserted at index i.
   insertChildCache(i);
   return child(i);
}


NODplace* NOXnode::initChild(int i) const{
   if (i < m_pDomNode->countOfAttribute()){
      return new NOXsimple(m_pDomNode->attribute(i), m_pContext.get());
   } else {
      DOMnode* pDomNode = m_pDomNode->childNode(i - m_pDomNode->countOfAttribute());
      if (pDomNode->nodeType() == DOMnode::ElementNode){
         return new NOXnode(pDomNode, m_pContext.get());
      } else {
         return new NOXsimple(pDomNode, m_pContext.get());
      }
   }
}

bool NOXnode::onRemoveChild(int i){
   COLuint32 CountOfAttribute = m_pDomNode->countOfAttribute();
   COLPOSTCONDITION(i >= 0 && i < CountOfAttribute + m_pDomNode->countOfChild());

   if (i < CountOfAttribute){
      const COLstring& RemovedName = NODplaceComposite::child(i).name();
      ((DOMelement*)m_pDomNode.get())->removeAttribute(RemovedName);
   } else {
      DOMnode* pChildToRemove = m_pDomNode->childNode(i - CountOfAttribute);
      m_pDomNode->removeChild(pChildToRemove);
   }
   return true;
}

void NOXnode::resetSize(){
   setSize(m_pDomNode->countOfAttribute() + m_pDomNode->countOfChild());
}
