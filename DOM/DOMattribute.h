#ifndef __DOM_ATTRIBUTE_H__
#define __DOM_ATTRIBUTE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2004 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DOMattribute
//
// Description:
//
// The Attribute object represents an attribute in an Element object. 
// Typically the allowable values for the attribute are defined in a 
// document type definition.
//
// Attribute objects inherit the Node object, but since they are not
// actually child nodes of the element they describe, the DOM does
// not consider them part of the document tree. Thus, the Node
// attributes parentNode, previousSibling, and nextSibling have a
// null value for Attr objects. The DOM takes the view that 
// attributes are properties of elements rather than having a
// separate identity from the elements they are associated with; 
// this should make it more efficient to implement such features
// as default attributes associated with all elements of a given
// type. Users and implementors of the DOM need to be aware that 
// Attribute nodes have some things in common with other objects
// inheriting the Node interface, but they also are quite distinct.
//
// Based on Document Object Model (Core) Level 1
// http://www.w3.org/TR/1998/REC-DOM-Level-1-19981001/level-one-core.html
//
// Author: Rob Moyse
// Date:   November 9, 2004
//
//
//---------------------------------------------------------------------------

#include <DOM/DOMnode.h>

class COLstring;

class DOMattributePrivate;

class DOMattribute : public DOMnode
{
public:

   /**
    * Constructor.
    *
    * Specify the name for the Attribute node using the
    * pName and NameLength values.  This is the efficient
    * way to do things because no memory allocation or
    * copying is done. HOWEVER, you have to be careful,
    * because it is assumed that the memory allocated for
    * the name remains allocated (until this node goes
    * away).  Generally, the pName pointer will point
    * to memory allocated in the Document's dataBuffer()
    * element (this ensures it stays around for the life
    * of the document's nodes).  
    *
    * This efficient constructor is used by DOMsink.
    */
   DOMattribute
   (
      DOMdocument* pDocument,
      const char* pName,
      COLuint32 NameLength
   );

   /**
    * Constructor.
    *
    * Specify the name for the Attribute node using the
    * COLstring values.  This is less efficient than
    * the other constructor since memory allocation and
    * copying is done.
    */
   DOMattribute
   (
      DOMdocument* pDocument,
      const COLstring& Name
   );

   virtual ~DOMattribute();

   /**
    * The value of this attribute.
    *
    * This method returns a pointer to a non-null-terminated
    * string and its length.
    */
   virtual const char* nodeValue(COLuint32* pLength) const;

   /**
    * The value of this attribute.
    *
    * This method creates and returns a COLstring representation
    * of the data.
    */
   virtual COLstring nodeValue() const;

   /**
    * Set the value of this attribute.
    *
    * This method is to be used CAREFULLY.  The Value passed
    * in and length is used directly (no memory allocation or
    * copying of characters is performed).  This means that 
    * setting the node's value is very efficient - but it also
    * means that the memory must remain allocated.  Usually 
    * the pointer passed in will point to a location in the
    * dataBuffer() associated with the Document object (which
    * is the root object for a DOM tree).
    *
    * Note: DOMsink uses this approach to efficiently populate
    *       the DOM tree.
    */
   virtual void setNodeValue(const char* Value, COLuint32 Length);

   /**
    * Set the value of this attribute.
    *
    * This method takes a COLstring.  As such, it does memory
    * allocation and copying.  The more efficient approach is
    * to use the other setNodeValue() method (but you have to
    * be more careful with that one.)
    */
   virtual void setNodeValue(const COLstring& Value);

   /**
    * The name of this attribute.
    *
    * This method returns a pointer to a non-null-terminated
    * string and its length.
    */      
   virtual const char* nodeName(COLuint32* pLength) const;

   /**
    * The name of this attribute.
    *
    * This method creates and returns a COLstring representation
    * of the name.
    */   
   virtual COLstring nodeName() const;

   // Indicates the type of the node by returning a value
   // from the enumeration DOMnodeType, AttributeNode.
   virtual DOMnodeType nodeType() const;
   // Streams the XML representation of this attribute. ex. name="value"
   virtual void streamNode(COLostream& OutputStream) const;

private:
   DOMattributePrivate* pMember;

   DOMattribute(); // not allowed
   DOMattribute(const DOMattribute& Orig); // not allowed - probably should be
   DOMattribute& operator=(const DOMattribute& Orig); // not allowed - probably should be
};

#endif // end of defensive include
