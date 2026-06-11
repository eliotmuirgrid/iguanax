//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DOMcomment
//
// Description:
//
// This represents the content of a comment, i.e., all the characters
// between the starting '<!--' and ending '-->'. Note that this is
// the definition of a comment in XML, and, in practice, HTML, although
// some HTML tools may implement the full SGML comment structure. 
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

#include <DOM/DOMcomment.h>

const COLstring DOMcomment::DOM_COMMENT_NODE_NAME = "#comment";

DOMcomment::DOMcomment
(
   DOMnode* pParentNode, 
   DOMdocument* pDocument,
   const char* pData,
   COLuint32 DataLength
) 
: DOMcharacterData(pParentNode, pDocument, pData, DataLength)
{
}

DOMcomment::DOMcomment
(
   DOMnode* pParentNode, 
   DOMdocument* pDocument,
   const COLstring& Data
)
: DOMcharacterData(pParentNode, pDocument, Data)
{
}

const char* DOMcomment::nodeName(COLuint32* pLength) const
{
   *pLength = DOM_COMMENT_NODE_NAME.size();
   return DOM_COMMENT_NODE_NAME.c_str();
}

COLstring DOMcomment::nodeName() const
{
   return DOM_COMMENT_NODE_NAME;
}

DOMnode::DOMnodeType DOMcomment::nodeType() const
{
   return DOMnode::CommentNode;
}

void DOMcomment::streamNode(COLostream& OutputStream) const{
   OutputStream << COL_T("<!--") << nodeValue() << COL_T("-->");
}
