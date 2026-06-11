//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DOMdocument
//
// Description:
//
// The Document object represents the entire HTML or XML document. 
// Conceptually, it is the root of the document tree, and provides 
// the primary access to the document's data.
//
// Text nodes, comments, etc. cannot exist outside
// the context of a Document.
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

#include <EXP/expat.h>

#include <DOM/DOMelement.h>
#include <DOM/DOMdocument.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

class DOMdocumentPrivate
{
public:
   COLstring DataBuffer;
};

const COLstring DOMdocument::DOM_DOCUMENT_NODE_NAME = "#document";

DOMdocument::DOMdocument(DOMnode* pParentNode)
: DOMnode(pParentNode, this)
{
   COL_METHOD(DOMdocument::DOMdocument);
   pMember = new DOMdocumentPrivate;
}

DOMdocument::~DOMdocument()
{
   delete pMember;
}

DOMelement* DOMdocument::documentElement()
{
   COL_METHOD(DOMdocument::documentElement);
   for (COLuint32 ChildIndex = 0; ChildIndex < countOfChild(); ChildIndex++)
   {
      if (childNode(ChildIndex)->nodeType() == DOMnode::ElementNode)
      {
         return (DOMelement*)childNode(ChildIndex);
      }
   }

   return NULL;
}

COLstring& DOMdocument::dataBuffer()
{
   COL_METHOD(DOMdocument::dataBuffer);
   return pMember->DataBuffer;
}

const char* DOMdocument::nodeName(COLuint32* pLength) const
{
   COL_METHOD(DOMdocument::nodeName);
   *pLength = DOM_DOCUMENT_NODE_NAME.size();
   return DOM_DOCUMENT_NODE_NAME.c_str();
}

COLstring DOMdocument::nodeName() const
{
   return DOM_DOCUMENT_NODE_NAME;
}

DOMnode::DOMnodeType DOMdocument::nodeType() const
{
   return DOMnode::DocumentNode;
}

void DOMdocument::streamNode(COLostream& OutputStream) const{
   COL_METHOD(DOMdocument::streamNode);
   for (COLuint32 i = 0; i < countOfChild(); i++){
      childNode(i)->streamNode(OutputStream);
      OutputStream << newline;
   }
}

