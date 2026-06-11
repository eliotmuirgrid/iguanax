//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NOXparse
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Wednesday, June 23rd, 2010 @ 09:32:20 AM
//
//---------------------------------------------------------------------------
#include "NOXprecomp.h"
#pragma hdrstop

#include "NOXparse.h"
#include "NOXnode.h"
#include "NOXutils.h"

#include "NOD/NODplace.h"

#include <FIL/FILfile.h>

#include <DOM/DOMdocument.h>
#include <DOM/DOMsink.h>
#include <DOM/DOMutils.h>
#include <DOM/DOMtext.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

// Remove comments and white space from the DOM tree.
void NOXcleanDomTree(DOMnode* pNode) {
   COL_FUNCTION(NOXcleanDomTree);
   COLuint32 CountOfChild = pNode->countOfChild();
   for (COLuint32 ChildIndex = 0; ChildIndex < pNode->countOfChild(); ChildIndex++) {
      DOMnode* pChildNode = pNode->childNode(ChildIndex);
      if (pChildNode->nodeType() == DOMnode::CommentNode ||
         (pChildNode->nodeType() == DOMnode::TextNode && pChildNode->nodeValue().isWhitespace())) {
         pNode->removeChild(pChildNode);
         ChildIndex--;
      } else if (pChildNode->nodeType() == DOMnode::ElementNode) {
         NOXcleanDomTree(pChildNode);
      }
   }
}

NODplace* NOXparseString(const COLstring& XmlString) {
   COL_FUNCTION(NOXparseString);
   COL_VAR(XmlString);
   
   DOMdocument* pDocument = new DOMdocument();
   DOMsink Sink(*pDocument, XmlString.size());
   
   try {
      Sink.write(XmlString.c_str(), XmlString.size());
      if (Sink.isUnbalanced()){
         throw COLerror(DOMunbalancedTagWarning(*pDocument, Sink));
      }
   } catch (const XMLexpatParserError& Error) {
      NOXrethrowExceptionWithSource(Error, XmlString);
   }
   
   NOXcleanDomTree(pDocument);
   COLref<NOXcontext> pContext = new NOXcontext(pDocument);

   return new NOXnode(pDocument, pContext.get());
}
