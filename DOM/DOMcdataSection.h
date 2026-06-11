#ifndef __DOM_CDATA_SECTION_H__
#define __DOM_CDATA_SECTION_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2004 iNTERFACEWARE Inc.  All Rights Reserved
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

#include <DOM/DOMcharacterData.h>

class COLstring;

class DOMcdataSection : public DOMcharacterData
{
public:

   static const COLstring DOM_CDATA_SECTION_NODE_NAME;

   /**
    * Constructor.
    *
    * Specify the data for the CdataSection node using the
    * pData and DataLength values.  This is the efficient
    * way to do things because no memory allocation or
    * copying is done. HOWEVER, you have to be careful,
    * because it is assumed that the memory allocated for
    * the data remains allocated (until this node goes
    * away).  Generally, the pData pointer will point
    * to memory allocated in the Document's dataBuffer()
    * element (this ensures it stays around for the life
    * of the document's nodes).  
    *
    * This efficient constructor is used by DOMsink.
    */
   DOMcdataSection
   (
      DOMnode* pParentNode, 
      DOMdocument* pDocument,
      const char* pData,
      COLuint32 DataLength
   );

   /**
    * Constructor.
    *
    * Specify the data for the CDataSection node using the
    * COLstring values.  This is less efficient than
    * the other constructor since memory allocation and
    * copying is done.
    */
   DOMcdataSection
   (
      DOMnode* pParentNode, 
      DOMdocument* pDocument,
      const COLstring& Data
   );

   /**
    * The name of this node which will always be"#cdata-section".
    *
    * This method returns a pointer to a non-null-terminated
    * string and its length.
    */   
   virtual const char* nodeName(COLuint32* pLength) const;

   /**
    * The name of this node which will always be"#cdata-section".
    */   
   virtual COLstring nodeName() const;

   /**
    * Returns the value DOMnodeType::CDataSectionNode
    */      
   virtual DOMnodeType nodeType() const;

   //Streams the XML representation of this node.
   virtual void streamNode(COLostream& OutputStream) const;

private:

   DOMcdataSection(); // not allowed
   DOMcdataSection(const DOMcdataSection& Orig); // not allowed - probably should be
   DOMcdataSection& operator=(const DOMcdataSection& Orig); // not allowed - probably should be
};

#endif // end of defensive include
