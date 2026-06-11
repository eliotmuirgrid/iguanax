#ifndef __NOD_PLACE_COMPOSITE_H__
#define __NOD_PLACE_COMPOSITE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NODplaceComposite
//
// Description:
//
// NODplaceComposite class.  Represents a NODplace with children
// (ie, a vector or complex type).
//
// Author: Kevin Senn
// Date:   Wednesday, September 14th, 2011 @ 09:37:28 AM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

#include "NODplace.h"

class NODplaceComposite : public NODplace
{
public:
   NODplaceComposite();
   virtual ~NODplaceComposite();

   const COLref<NODplace> createDummyItem() const;

   int size() const;

   virtual void setValue(const COLstring& Value) { COL_ERROR_STREAM_PLAIN("Cannot assign a string value to a node with children.", COLerror::PreCondition); }
   virtual const COLstring& value() const { COL_ERROR_STREAM_PLAIN("This is not a leaf node, and therefore has no value.", COLerror::PreCondition); }

   virtual void clear();

   void remove(int i);
   virtual NODplace& add(const char* Arg1, const char* Arg2);
   virtual NODplace& insert(int i, const char* Arg1, const char* Arg2);

   NODplace& child(int i);
   const NODplace& child(int i) const;

   virtual int childIndex(const COLstring& Name) const;
   NODplace& child(const COLstring& Name);
   const NODplace& child(const COLstring& Name) const;

   void remove(const COLstring& Name);

   virtual void nullify(int i);
   virtual void nullify(const COLstring& Name);

protected:

   // Vector or Complex type.  IMPORTANT - call this in a subclass's
   // constructor if the initial size should be anything other than 0.
   // We can't call it in NODplace's constructor, because C++ doesn't
   // let us call virtual functions in constructors.
   // This can also be called if the node's children have changed
   // (through, for example, a call to setTreeValue()).  All cached
   // child nodes will be cleared.
   void setSize(int Size);

   // Insert a NULL slot for a child in the child cache.  This should
   // only be used if add() and/or insert() are overridden.
   void insertChildCache(int i);

   // Insert an initialized slot for a child in the child cache.
   // Will increase reference count of pChild.
   void insertChildCache(int i, NODplace* pChild);

   // Remove a slot for a child in the child cache.  This should only
   // be used if an object is not being removed via NODplace's standard
   // remove() functions.
   void removeChildCache(int i);

   // Vector or Complex type.  Called from add() or insert().
   // This creates a brand new child.  Implementations should return
   // a newly allocated NODplace.
   // Meaning of args depend on implementation.
   // An index of -1 indicates that a "dummy" item is being created for
   // a homogeneous vector (see createDummyItem() above).
   // For a non-homogeneous vector, expect this to be called with
   // Arg1 as a value returned by nodeTypeName().
   virtual NODplace* newChild(int i, const char* Arg1, const char* Arg2) = 0;

   // Vector or Complex type.  Called from the child() functions.
   // This initializes a child which is already "present" (ie, there
   // is a spot reserved for it in the vector of children).
   // Therefore, for vectors, it only applies if the vector starts
   // with some items in it.
   // Implementations should return a newly allocated NODplace.
   virtual NODplace* initChild(int i) const = 0;

   // Complex type.  Called from clear() or nullifyChild(), before the
   // child is actually nullified.
   virtual void onNullifyChild(int i) {}

   // Vector or Complex type.  Called from clear() or removeChild(), before
   // the child is actually removed.  Return true if the child should be
   // removed from the cache, false if the child should just be nullified
   // instead.
   virtual bool onRemoveChild(int i) { return true; }

private:
   // Cache of children.
   mutable COLarray< COLref<NODplace> > m_Children;

   void nullifyChildImpl(int i);
   void resizeHomogeneousVector(int NewSize);

   NODplaceComposite(const NODplaceComposite& Orig); // not allowed
   NODplaceComposite& operator=(const NODplaceComposite& Orig); // not allowed
};

#endif // end of defensive include
