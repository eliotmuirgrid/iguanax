#ifndef __DOM_ELEMENT_H__
#define __DOM_ELEMENT_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DOMelement
//
// Description:
//
// By far the vast majority of objects (apart from text) that authors 
// encounter when traversing a document are Element nodes. Assume 
// the following XML document: 
//
// <elementExample id="demo">
//    <subelement1/>
//    <subelement2><subsubelement/></subelement2>
// </elementExample>  
//
// When represented using DOM, the top node is an Element node for 
// "elementExample", which contains two child Element nodes, one for 
// "subelement1" and one for "subelement2". "subelement1" contains
// no child nodes.
//
// Elements may have attributes associated with them; since the 
// Element object inherits from Node, the generic Node object 
// attribute methods may be used to retrieve attributes
// for an element. There are methods on the Element interface to 
// retrieve either an Attribute object by name or an attribute 
// value by name. 
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

class DOMelementPrivate;

class DOMelement : public DOMnode
{
public:

   /**
    * Constructor.
    *
    * Specify the tag name for the Element node using the
    * pTagName and TagNameLength values.  This is the efficient
    * way to do things because no memory allocation or
    * copying is done. HOWEVER, you have to be careful,
    * because it is assumed that the memory allocated for
    * the tag name remains allocated (until this node goes
    * away).  Generally, the pTagName pointer will point
    * to memory allocated in the Document's dataBuffer()
    * element (this ensures it stays around for the life
    * of the document's nodes).  
    *
    * This efficient constructor is used by DOMsink.
    */
   DOMelement
   (
      DOMnode* pParentNode, 
      DOMdocument* pDocument,
      const char* pTagName,
      COLuint32 TagNameLength
   );

   /**
    * Constructor.
    *
    * Specify the name for the Element node using the
    * COLstring values.  This is less efficient than
    * the other constructor since memory allocation and
    * copying is done.
    */
   DOMelement
   (
      DOMnode* pParentNode, 
      DOMdocument* pDocument,
      const COLstring& TagName
   );

   virtual ~DOMelement();

   /** 
    * Returns the attribute at the given index if there is one
    * else NULL is returned.
    */
   virtual DOMattribute* attribute(COLuint32 AttributeIndex) const;

   /** 
    * Returns the value of the attribute with the given Name.
    */
   virtual COLstring attribute(const COLstring& Name) const;

   /** 
    * Returns the DOMattribute node object of the attribute 
    * with the given Name.
    */
   virtual DOMattribute* attributeNode(const COLstring& Name) const;

   /** 
    * Returns the count of attributes.
    */
   virtual COLuint32 countOfAttribute() const;

   /** 
    * Returns whether this element has attributes associated with it
    * or not.
    */
   virtual bool hasAttributes() const;
 
   /** 
    * Removes the attribute of the given name from this element's
    * collection of attributes.
    */
   virtual void removeAttribute(const COLstring& Name);

   /**
    * Creates a new attribute of the given name and value and
    * associates it with this element.
    *
    * NOTE: this is the efficient (but less safe) approach.  It
    * doesn't do any copying or allocation and just assumes that
    * the memory allocated to the strings will stay allocated
    * for the lifetime of the attribute.  Usually to ensure this,
    * the memory buffer in Document's dataBuffer() is used.
    *
    * This is used by DOMsink's implementaiton.
    */
   virtual void setAttribute(const char* pName, COLuint32 NameLength, const char* pValue, COLuint32 ValueLength);

   /**
    * Creates a new attribute of the given name and value and
    * associates it with this element.
    *
    * NOTE: this is the less efficient implementation since it
    *       does allocation and copying.
    */
   virtual void setAttribute(const COLstring& Name, const COLstring& Value);

   /**
    * Similar to the above, but it uses a specific attribute index.
    */
   virtual void insertAttribute(COLuint32 AttributeIndex, const COLstring& Name, const COLstring& Value);

   /**
    * Sets the given attribute as an attribute on this element.
    *
    * NOTE: This element will take ownership of the attribute 
    *       object (is responsible for its deallocation).
    */
   virtual DOMattribute* setAttributeNode(DOMattribute* pAttribute);

   /**
    * Sets the document (root) object and parent node associated with this node.
    * Will set the owner document for every ancestor of this node, as well as
    * every attribute.
    */
   virtual void setOwnerDocument(DOMdocument* pOwnerDocument);

   /**
    * The tag name of this element.
    *
    * This method returns a pointer to a non-null-terminated
    * string and its length.
    */      
   virtual const char* nodeName(COLuint32* pLength) const;

   /**
    * The tag name of this element.
    *
    * This method creates and returns a COLstring representation
    * of the name.
    */   
   virtual COLstring nodeName() const;

   /**
    * Indicates the type of the node by returning a value
    * from the enumeration DOMnodeType, ElementNode
    */
   virtual DOMnodeType nodeType() const;
   
   // Streams the XML representation of this node and all of its children.
   virtual void streamNode(COLostream& OutputStream) const;

protected:
   virtual COLuint32 attributeIndex(const COLstring& Name) const;

private:
   DOMelementPrivate* pMember;

   DOMelement(); // not allowed
   DOMelement(const DOMelement& Orig); // not allowed - probably should be
   DOMelement& operator=(const DOMelement& Orig); // not allowed - probably should be
};

#endif // end of defensive include
