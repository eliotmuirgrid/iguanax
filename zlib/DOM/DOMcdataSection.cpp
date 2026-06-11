//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DOMcdataSection
//
// Description:
//
// CDATA sections are used to escape blocks of text containing characters
// that would otherwise be regarded as markup. The only delimiter that
// is recognized in a CDATA section is the "]]>" string that ends the
// CDATA section. CDATA sections can not be nested. The primary purpose
// is for including material such as XML fragments, without needing
// to escape all the delimiters.
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

#include <DOM/DOMcdataSection.h>

const COLstring DOMcdataSection::DOM_CDATA_SECTION_NODE_NAME = "#cdata-section";

DOMcdataSection::DOMcdataSection
(
   DOMnode* pParentNode, 
   DOMdocument* pDocument,
   const char* pData,
   COLuint32 DataLength
) 
: DOMcharacterData(pParentNode, pDocument, pData, DataLength)
{
}

DOMcdataSection::DOMcdataSection
(
   DOMnode* pParentNode, 
   DOMdocument* pDocument,
   const COLstring& Data
)
: DOMcharacterData(pParentNode, pDocument, Data)
{
}

const char* DOMcdataSection::nodeName(COLuint32* pLength) const
{
   *pLength = DOM_CDATA_SECTION_NODE_NAME.size();
   return DOM_CDATA_SECTION_NODE_NAME.c_str();
}

COLstring DOMcdataSection::nodeName() const
{
   return DOM_CDATA_SECTION_NODE_NAME;
}

DOMnode::DOMnodeType DOMcdataSection::nodeType() const
{
   return DOMnode::CDataSectionNode;
}

void DOMcdataSection::streamNode(COLostream& OutputStream) const{
   OutputStream << COL_T("<![CDATA[") << nodeValue() << COL_T("]]>");
}
