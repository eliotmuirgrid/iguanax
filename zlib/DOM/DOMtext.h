#ifndef __DOM_TEXT_H__
#define __DOM_TEXT_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2004 iNTERFACEWARE Inc.  All Rights Reserved
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

#include <DOM/DOMcharacterData.h>

class COLstring;

class DOMtext : public DOMcharacterData
{
public:

   static const COLstring DOM_TEXT_NODE_NAME;

   /**
    * Constructor.
    *
    * Specify the data for the Text node using the
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
   DOMtext
   (
      DOMnode* pParentNode, 
      DOMdocument* pDocument,
      const char* pData,
      COLuint32 DataLength
   );

   /**
    * Constructor.
    *
    * Specify the data for the Text node using the
    * COLstring values.  This is less efficient than
    * the other constructor since memory allocation and
    * copying is done.
    */
   DOMtext
   (
      DOMnode* pParentNode, 
      DOMdocument* pDocument,
      const COLstring& Data
   );

   /**
    * The name of this node which will always be"#text".
    *
    * This method returns a pointer to a non-null-terminated
    * string and its length.
    */   
   virtual const char* nodeName(COLuint32* pLength) const;

   /**
    * The name of this node which will always be"#text".
    */   
   virtual COLstring nodeName() const;

   /**
    * Returns the value DOMnodeType::TextNode
    */   
   virtual DOMnodeType nodeType() const;

   // Streams the XML representation of this node.
   virtual void streamNode(COLostream& OutputStream) const;

private:

   DOMtext(); // not allowed
   DOMtext(const DOMtext& Orig); // not allowed - probably should be
   DOMtext& operator=(const DOMtext& Orig); // not allowed - probably should be
};

#endif // end of defensive include
