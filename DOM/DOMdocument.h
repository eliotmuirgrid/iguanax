#ifndef __DOM_DOCUMENT_H__
#define __DOM_DOCUMENT_H__
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

#include <DOM/DOMnode.h>

class COLstring;

class DOMelement;
class DOMdocumentPrivate;

class DOMdocument : public DOMnode
{
public:

   static const COLstring DOM_DOCUMENT_NODE_NAME;

   DOMdocument(DOMnode* pParentNode = NULL);
   virtual ~DOMdocument();

   /**
    * This is a convenience method that allows direct access to 
    * the child node that is the root element of the document. 
    * For HTML documents, this is the element with the tagName 
    * "HTML". 
    */
   DOMelement* documentElement();

   /**
    * Returns the single buffer that can be used to store all
    * element tag names and attribute names, values, etc. in one
    * place so that allocation is efficient.  Since the buffer
    * resides on the document object (which is the root of the
    * DOM tree), then it's guaranteed to be around for the life
    * of its member nodes.
    *
    * Used by DOMsink to populate DOM trees.
    */
   COLstring& dataBuffer();

   /**
    * The name of this node which will always be"#document".
    *
    * This method returns a pointer to a non-null-terminated
    * string and its length.
    */   
   virtual const char* nodeName(COLuint32* pLength) const;

   /**
    * The name of this node which will always be"#document".
    *
    * This method returns a COLstring.
    */   
   virtual COLstring nodeName() const;   

   /**
    * Returns the value DOMnodeType::DocumentNode
    */   
   virtual DOMnodeType nodeType() const;

   //Streams the XML representation of the entire DOM document.
   virtual void streamNode(COLostream& OutputStream) const;

private:
   DOMdocumentPrivate* pMember;

   DOMdocument(const DOMdocument& Orig); // not allowed - probably should be
   DOMdocument& operator=(const DOMdocument& Orig); // not allowed - probably should be
};

#endif // end of defensive include
