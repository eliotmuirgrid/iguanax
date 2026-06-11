#ifndef __LHL7_NODE_H__
#define __LHL7_NODE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LHL7node
//
// Description:
//
// Node tree representing the new HL7 implementation
//
// Author: Eliot Muir 
// Date:   Friday 07 October 2022 - 11:07AM
// ---------------------------------------------------------------------------

#include <NOD/NODplace.h>

class EDIdata;

class LHL7node : public NODplace{ 
public:
   LHL7node(EDIdata* pNode);
   virtual ~LHL7node();

   virtual NODgrammarType grammarType() const;

   virtual int protocolType() const;
   virtual int nodeType() const;
   virtual const COLstring& protocolName() const;
   virtual const COLstring& nodeTypeName() const;
   virtual const COLstring& name() const;
   virtual bool isReadOnly() const;
   virtual bool isNamed() const;

   // Only valid for Vector nodes.
   // Returns true if all items in the vector share the same grammar.
   virtual bool isHomogeneous() const;
   virtual bool favorIndexByName() const;

   //virtual bool gracefulNoSuchMember() const { return false; }
   virtual bool setTreeValue(const COLstring& TreeValue) { return false; }  // TODO - come back to it
   virtual bool isNull() const;
   //virtual void setIsNull(bool IsNull) {}
  
   virtual void setValue(const COLstring& Value);
   virtual const COLstring& value() const;
   virtual bool shouldEscapeValue() const;

   virtual const COLref<NODplace> createDummyItem() const;
   virtual int size() const;
   virtual void clear();

   virtual void remove(int i);
   virtual NODplace& add(const char* Arg1, const char* Arg2);
   virtual NODplace& insert(int i, const char* Arg1, const char* Arg2);

   virtual NODplace& child(int i);
   virtual const NODplace& child(int i) const;

   virtual int childIndex(const COLstring& Name) const;

   virtual NODplace& child(const COLstring& Name);
   virtual const NODplace& child(const COLstring& Name) const;

   // Remove a child by name.
   virtual void remove(const COLstring& Name);

   // Nullify a child (by integer index or name).
   virtual void nullify(int i);
   virtual void nullify(const COLstring& Name);

   virtual void grammar(COLvar* pGrammar) const;

   COLref<EDIdata> pNode; 
   COLarray< COLref<LHL7node> > m_Children;
};

#endif // end of defensive include
