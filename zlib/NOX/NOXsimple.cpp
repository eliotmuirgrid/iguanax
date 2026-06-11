//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NOXsimple
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Monday, September 19th, 2011 @ 11:44:46 AM
//
//---------------------------------------------------------------------------
#include "NOXprecomp.h"
#pragma hdrstop

#include "NOXsimple.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

NOXsimple::NOXsimple(DOMnode* pDomNode, NOXcontext* pContext)
 : m_pDomNode(pDomNode),
   m_pContext(pContext),
   m_NameId(-1)
{
   m_Value = m_pDomNode->nodeValue();
}

int NOXsimple::nodeType() const{
   switch (m_pDomNode->nodeType())
   {
   case DOMnode::AttributeNode:    return NOX_ATTRIBUTE;
   case DOMnode::CDataSectionNode: return NOX_CDATA;
   case DOMnode::TextNode:         return NOX_TEXT;
   default:
      COL_ERROR_STREAM("Unexpected XML node type", COLerror::PreCondition);
   };
}

const COLstring& NOXsimple::nodeTypeName() const{
   switch(m_pDomNode->nodeType()){
   case DOMnode::AttributeNode:    return NOXattributeName;
   case DOMnode::CDataSectionNode: return NOXcdataName;
   case DOMnode::TextNode:         return NOXtextName;
   default:
      COL_ERROR_STREAM("Unexpected XML node type", COLerror::PreCondition);
   }
}

const COLstring& NOXsimple::name() const{
   if (m_NameId == -1){
      COLstring Name;
      switch (m_pDomNode->nodeType()){
      case DOMnode::CDataSectionNode:
         Name = "CDATA";
         m_NameId = m_pContext->idFromValue(Name);
         break;
      case DOMnode::TextNode:
         Name = "Text";
         break;
      default:
         Name = m_pDomNode->nodeName();
         break;
      };
      m_NameId = m_pContext->idFromValue(Name);
   }
   return m_pContext->valueFromId(m_NameId);
}

bool NOXsimple::isNamed() const{
   return m_pDomNode->nodeType() == DOMnode::AttributeNode;
}

bool NOXsimple::setTreeValue(const COLstring& TreeValue){
   setValue(TreeValue);
   return true;
}

void NOXsimple::setValue(const COLstring& Value){
   m_Value = Value;
   m_pDomNode->setNodeValue(Value);
}
