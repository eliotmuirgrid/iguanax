#ifndef __DOM_COMMENT_H__
#define __DOM_COMMENT_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2004 iNTERFACEWARE Inc.  All Rights Reserved
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

#include <DOM/DOMcharacterData.h>

class COLstring;

class DOMcomment : public DOMcharacterData
{
public:

   static const COLstring DOM_COMMENT_NODE_NAME;


   /**
    * Constructor.
    *
    * Specify the data for the Comment node using the
    * pData and DataLength values.  This is the efficient
    * way to do things because no memory allocation or
    * copying is done. HOWEVER, you have to be careful,
    * because it is assumed that the memory allocated for
    * the data remains allocated (until this node goes
    * away).  Generally, the pData pointer will point
    * to memory allocated in the Documents dataBuffer()
    * element (this ensures it stays around for the life
    * of the document's nodes).  
    *
    * This efficient constructor is used by DOMsink.
    */
   DOMcomment
   (
      DOMnode* pParentNode, 
      DOMdocument* pDocument,
      const char* pData,
      COLuint32 DataLength
   );

   /**
    * Constructor.
    *
    * Specify the data for the Comment node using the
    * COLstring values.  This is less efficient than
    * the other constructor since memory allocation and
    * copying is done.
    */
   DOMcomment
   (
      DOMnode* pParentNode, 
      DOMdocument* pDocument,
      const COLstring& Data
   );

   /**
    * The name of this node which will always be"#comment".
    *
    * This method returns a pointer to a non-null-terminated
    * string and its length.
    */   
   virtual const char* nodeName(COLuint32* pLength) const;

   /**
    * The name of this node which will always be"#comment".
    */   
   virtual COLstring nodeName() const;

   /**
    * Returns the value DOMnodeType::CommentNode
    */   
   virtual DOMnodeType nodeType() const;

   // Streams the XML representation of this node.
   virtual void streamNode(COLostream& OutputStream) const;
private:

   DOMcomment(); // not allowed
   DOMcomment(const DOMcomment& Orig); // not allowed - probably should be
   DOMcomment& operator=(const DOMcomment& Orig); // not allowed - probably should be
};

#endif // end of defensive include
