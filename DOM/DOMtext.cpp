//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DOMtext
//
// Description:
//
// The Text interface represents the textual content (termed character 
// data in XML) of an Element. If there is no markup inside an
// element's content, the text is contained in a single Text object
// that is the only child of the element. If there is markup, it
// is parsed into a list of elements and Text nodes that form
// the list of children of the element.
//
// Subclass of DOMcharacterData
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

#include <DOM/DOMtext.h>

const COLstring DOMtext::DOM_TEXT_NODE_NAME = "#text";

DOMtext::DOMtext
(
   DOMnode* pParentNode, 
   DOMdocument* pDocument,
   const char* pData,
   COLuint32 DataLength
) 
: DOMcharacterData(pParentNode, pDocument, pData, DataLength)
{
}

DOMtext::DOMtext
(
   DOMnode* pParentNode, 
   DOMdocument* pDocument,
   const COLstring& Data
)
: DOMcharacterData(pParentNode, pDocument, Data)
{
}

const char* DOMtext::nodeName(COLuint32* pLength) const
{
   *pLength = DOM_TEXT_NODE_NAME.size();
   return DOM_TEXT_NODE_NAME.c_str();
}

COLstring DOMtext::nodeName() const
{
   return DOM_TEXT_NODE_NAME;
}

DOMnode::DOMnodeType DOMtext::nodeType() const
{
   return DOMnode::TextNode;
}

void DOMtext::streamNode(COLostream& OutputStream) const{
   OutputStream << nodeValue();
}
