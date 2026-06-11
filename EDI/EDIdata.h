#ifndef __EDI_DATA_H__
#define __EDI_DATA_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: EDIdata
//
// Description:
//
// Data - instance of a EDI data node - i.e. an HL7 message, segment etc.
// It should be possible for this to be node at any level in the EDI structure.
//
// Author: Eliot Muir 
// Date:   Wednesday 05 October 2022 - 07:58PM
// ---------------------------------------------------------------------------

#include <COL/COLarray.h>
#include <COL/COLrefCounted.h>
#include <COL/COLref.h>

class EDInode;
class EDIcollection;
class EDIchild;

enum EDIdataType{ 
   EDIN_MESSAGE,
   EDIN_SEGMENT_GROUP_ARRAY,
   EDIN_SEGMENT_GROUP,
   EDIN_SEGMENT_ARRAY,
   EDIN_SEGMENT,
   EDIN_FIELD_ARRAY,
   EDIN_FIELD, 
   EDIN_SUBFIELD,
   EDIN_SUBSUBFIELD
};

bool EDIisRepeat(EDIdataType Type);

class EDIdata : public COLrefCounted {
public:
   EDIdata(const EDIcollection& Collection, const COLstring& MessageName);
   EDIdata(const EDIcollection& Collection, const EDInode* pParentType , int ChildIndex, EDIdataType Type);
   ~EDIdata();

   void flatwire(COLstring* pOut) const;
   void flatwireHtml(COLstring* pOut) const;
   void toVar(COLvar* pOut) const; // Sets up a COLvar which can be serialized to JSON
   // TODO add grammar info then we have [] overloaded with names.
   EDIdata& operator[](const COLstring& Name);


   // TODO const versions
   EDIdata& operator[](int i);
   const EDIdata& operator[](int i) const;
   EDIdata& operator=(const COLstring& Name);
   bool operator==(const EDIdata& Other) const; // TODO - delete if not needed

   bool isRepeat() const;
   bool isNull() const;
   bool isLeaf() const;
   const COLstring& name() const;

   int childIndex(const COLstring& Name) const;

   int size() const;
   int grammarSize() const;

   void addRepeat(int i);
   void addRepeatField(int i);
   void addChild (int i);
   void addRootChild(int i);

   bool isRequired() const;

   void setValue(const COLstring& Data, COLarray<COLstring>* pHere=nullptr);
   const COLstring& value() const;

   const EDIcollection& Collection;

   const EDInode* pParentType;
   int ChildIndex;
   EDIdataType Type;

   // TODO - optimize later
   COLstring           Data;       // If we are a leaf
   COLarray< COLref<EDIdata> > Children;   // If we are a branch
};

COLostream& operator<<(COLostream& Stream, const EDIdata& Node);

// Question - do we need a pointer to the EDIcollecton object or not?
// One school of thought is to just do functional code which can be given
// the EDIcollection at the top.

EDIdataType EDImapChildType(EDIdataType ParentType, const EDIchild& Child);
const char* EDIdataTypeAsString(EDIdataType Type);

#endif // end of defensive include
