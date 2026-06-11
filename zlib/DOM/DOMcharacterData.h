#ifndef __DOM_CHARACTER_DATA_H__
#define __DOM_CHARACTER_DATA_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2004 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DOMcharacterData
//
// Description:
//
// The CharacterData interface extends Node with a set of
// methods for accessing character data in the DOM. For clarity 
// this set is defined here rather than on each object that
// uses these attributes and methods. No DOM objects 
// correspond directly to CharacterData, though Text and others
// do inherit the interface from it. All offsets in this interface
// start from 0. 
//
// Direct subclasses: DOMtext, DOMcomment, DOMcdataSection.
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

class DOMcharacterDataPrivate;

class DOMcharacterData : public DOMnode
{
public:

   // see subclasses Text, Comment and CdataSection
   // for details
   DOMcharacterData
   (
      DOMnode* pParentNode, 
      DOMdocument* pDocument,
      const char* pData,
      COLuint32 DataLength
   );

   // see subclasses Text, Comment and CdataSection
   // for details
   DOMcharacterData
   (
      DOMnode* pParentNode, 
      DOMdocument* pDocument,
      const COLstring& Data
   );

   virtual ~DOMcharacterData();

   /**
    * The character data.
    *
    * This method returns a pointer to a non-null-terminated
    * string and its length.
    */
   virtual const char* nodeValue(COLuint32* pLength) const;

   /**
    * The character data.
    *
    * This method creates and returns a COLstring representation
    * of the data.
    */
   virtual COLstring nodeValue() const;

   /**
    * Set the chracter data to the pointer passed in.
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
    * Set the chracter data to the string passed in.
    *
    * This method takes a COLstring.  As such, it does memory
    * allocation and copying.  The more efficient approach is
    * to use the other setNodeValue() method (but you have to
    * be more careful with that one.)
    */
   virtual void setNodeValue(const COLstring& Value);

   virtual COLstring substringData(COLuint32 Offset, COLuint32 Count);

   /**   
    * this method only adjusts the length value associated with
    * the character data.  This is useful only in the mode where
    * you're setting the data using pointers to an external buffer.
    */
   virtual void appendLength(COLuint32 ExtraLength);

   virtual void appendData(const COLstring& Data);
   virtual void insertData(COLuint32 Offset, const COLstring& Data);
   virtual void deleteData(COLuint32 Offset, COLuint32 Count);
   virtual void replaceData(COLuint32 Offset, COLuint32 Count, const COLstring& Data);
   
protected:

   /**
    * Ensures that the COLstring associated with this node is
    * populated.  Likely because some operation that COLstring
    * provides is going to be performed.
    */ 
   void ensureDataStringPopulated();

   /**
    * Updates the data pointer and length value based on the
    * COLstring data member.
    */ 
   void updateFromDataString();

private:
   DOMcharacterDataPrivate* pMember;

   DOMcharacterData(); // not allowed
   DOMcharacterData(const DOMcharacterData& Orig); // not allowed - probably should be
   DOMcharacterData& operator=(const DOMcharacterData& Orig); // not allowed - probably should be
};

#endif // end of defensive include
