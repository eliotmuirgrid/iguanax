#ifndef __DOM_NODE_H__
#define __DOM_NODE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DOMnode
//
// Description:
//
// The DOMnode is the primary base datatype for the entire Document 
// Object Model. It represents a single node in the document tree. 
// While all objects subclass DOMnode interface expose methods for 
// dealing with children, not all objects subclassing the DOMnode 
// interface may have children. For example, Text nodes may not 
// have children.
//
// The methods to access nodeName, nodeValue and attributes are 
// included as a mechanism to get at node information without
// casting down to the specific derived interface. In cases where
// there is no obvious mapping of these attributes for a specific 
// nodeType (e.g., nodeValue for an Element or attributes for a
// Comment), this returns null. Note that the specialized classes
// may contain additional and more convenient mechanisms to get
// and set the relevant information. 
//
// Direct Subclasses: DOMattribute, DOMchracterData, DOMdocument, 
//                    DOMelement
// 
// Based on Document Object Model (Core) Level 1
// http://www.w3.org/TR/1998/REC-DOM-Level-1-19981001/level-one-core.html
//
// Author: Rob Moyse
// Date:   November 8, 2004
//
//
//---------------------------------------------------------------------------

template<class _Value>
class COLarray;

class COLstring;
class COLostream;
#include <COL/COLrefCounted.h>

class DOMdocument;
class DOMelement;
class DOMelementList;
class DOMattribute;
class DOMnodePrivate;

class DOMnode : public COLrefCounted
{
public:

   // types returned from nodeType(), useful
   // since in most cases we're dealing with subclasses
   // as DOMnodes.
   enum DOMnodeType
   {
      AttributeNode,    // The node is a DOMattribute
      CDataSectionNode, // The node is a DOMcDataSection
      CommentNode,      // The node is a DOMcomment
      DocumentNode,     // The node is a DOMdocument
      ElementNode,      // The node is a DOMelement
      TextNode          // The node is a DOMtext
   };
   
   DOMnode(DOMnode* pParentNode, DOMdocument* pDocument);
   virtual ~DOMnode();

   //The parent of this node.  May be null.
   virtual DOMnode* parentNode() const;

   //The node immediately following this node. If there is no such node, this returns null. 
   virtual DOMnode* nextSibling() const;

   //The node immediately previous to this node. If there is no such node, this returns null. 
   virtual DOMnode* previousSibling() const;

   // Adds the node newChild to the end of the list of children of this node.
   // NOTE: ownership of the new child is assumed by the node receiving the new child.
   virtual DOMnode* appendChild(DOMnode* pNewChild);

   // Adds the node newChild to the list of children of this node directly before the reference child.
   //  NOTE: ownership of the new child is assumed by the node receiving the new child.
   virtual DOMnode* insertBefore(DOMnode* pReferenceChild,DOMnode* pNewChild);

   //Removes the specified child.
   virtual void removeChild(DOMnode* pChild);

   // Removes the specified old child and replaces it with the provided new child.
   //  /NOTE: ownership of the new child is assumed by the node receiving the new child.
   virtual DOMnode* replaceChild(DOMnode* pNewChild, DOMnode* pOldChild);

   // Returns the child node immediately after the given reference node. If there is no such node, this returns null. 
   virtual DOMnode* childAfter(const DOMnode* pReferenceChild) const;

   //Returns the child node immediately before the given reference node. If there is no such node, this returns null. 
   virtual DOMnode* childBefore(const DOMnode* pReferenceChild) const;

   // Returns the child node at the given index.
   //  If the provided index is out of bounds, an exception is thrown.
   virtual DOMnode* childNode(COLuint32 ChildIndex) const;

   // Returns the count of child nodes.
   virtual COLuint32 countOfChild() const;

   // Returns whether the node has children (true) or not.
   virtual bool hasChildren() const;

   // Returns the attribute at the given index if this node is an Element, otherwise NULL is returned.
   virtual DOMattribute* attribute(COLuint32 ChildIndex) const { return NULL; };

   // Returns the count of attributes if this node is an Element, otherwise 0 is returned.
   virtual COLuint32 countOfAttribute() const;

   // Returns whether this node has attributes associated with it or not (will always be false in the case of a non-Element node).
    virtual bool hasAttributes() const;

   // The Document (root) object associated with this node
   virtual DOMdocument* ownerDocument();

   // Sets the document (root) object and parent node associated with this node.
   // Will set the owner document for every ancestor of this node as well.
   // This should only be used when moving DOM nodes from one tree to another.
   virtual void setOwnerDocument(DOMdocument* pOwnerDocument);

   // Sets the parent of this node.
   // This should only be used when moving DOM nodes from one tree to another.
   virtual void setParent(DOMnode* pParent);

   // The value of this node.  Depends on the node type:
   //  Attribute:    value of attribute
   //  CDATASection: content of the CDATASection
   //  Comment:      content of the Comment
   //  Text:         content of the Text node
   // All other node types will return NULL for their value.
   //
   // This method returns a pointer to a non-null-terminated string and its length.
   virtual const char* nodeValue(COLuint32* pLength) const { return NULL; };

   // The value of this node.  Depends on the node type:
   //  Attribute:    value of attribute
   //  CDATASection: content of the CDATASection
   //  Comment:      content of the Comment
   //  Text:         content of the Text node
   // All other node types will return NULL for their value.
   //
   // This method creates and returns a COLstring representation of the data.
   virtual COLstring nodeValue() const;

   /**
    * Set the value of this node.  Depends on the node type:
    *
    * Attribute:    value of attribute
    * CDATASection: content of the CDATASection
    * Comment:      content of the Comment
    * Text:         content of the Text node
    *
    * Setting values for all other node types will do nothing.
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
   virtual void setNodeValue(const char* pValue, COLuint32 Length) {};

   /**
    * Set the value of this node.  Depends on the node type:
    *
    * Attribute:    value of attribute
    * CDATASection: content of the CDATASection
    * Comment:      content of the Comment
    * Text:         content of the Text node
    *
    * Setting values for all other node types will do nothing.
    *
    * This method takes a COLstring.  As such, it does memory
    * allocation and copying.  The more efficient approach is
    * to use the other setNodeValue() method (but you have to
    * be more careful with that one.)
    */
   virtual void setNodeValue(const COLstring& Value) {};

   /**
    * The name of this node.  Depends on the node type:
    *
    * Element:      the tag name
    * Attribute:    name of attribute
    * CDATASection: "#cdata-section"
    * Comment:      "#comment"
    * Document:     "#document"
    * Text:         "#text"
    *
    * This method returns a pointer to a non-null-terminated
    * string and its length.
    */   
   virtual const char* nodeName(COLuint32* pLength) const = 0;

   /**
    * The name of this node.  Depends on the node type:
    *
    * Element:      the tag name
    * Attribute:    name of attribute
    * CDATASection: "#cdata-section"
    * Comment:      "#comment"
    * Document:     "#document"
    * Text:         "#text"
    *
    * This method creates and returns a COLstring representation
    * of the name.
    */   
   virtual COLstring nodeName() const = 0;

   /**
    * Indicates the type of the node by returning a value
    * from the enumeration DOMnodeType defined above.
    */
   virtual DOMnodeType nodeType() const = 0;

    
   // Streams the XML representation of this node and all
   // of its children to the provided OutputStream.  Invoking
   // this method on the Document (root) object of a DOM
   // tree will result in generation of the entire XML
   // representation of that document.
   virtual void streamNode(COLostream& OutputStream) const = 0;

   /**
    * Returns a DOMelementList containing all the Elements
    * with a given tag name in the order in which they
    * would be encountered in a preorder traversal of
    * the tree starting at the current node.
    */
   virtual DOMelementList elementsByTagName(const COLstring& TagName);

   /**
    * Populates a DOMelementList containing all the Elements
    * with a given tag name in the order in which they
    * would be encountered in a preorder traversal of
    * the tree starting at the current node.
    *
    * More efficient to use this version of the method since
    * the DOMelementList need not be copied.
    */
   virtual void elementsByTagName(const COLstring& TagName, DOMelementList& ElementList);

protected:

   // determine the index of a child given it as a reference
   COLuint32 childIndex(const DOMnode* pReferenceChild) const;

private:
   DOMnodePrivate* pMember;

   DOMnode(); // not allowed
   DOMnode(const DOMnode& Orig); // not allowed - probably should be
   DOMnode& operator=(const DOMnode& Orig); // not allowed - probably should be
};

// allows you to stream the node.
COLostream& operator<<(COLostream& Stream, const DOMnode& Node);

#endif // end of defensive include
