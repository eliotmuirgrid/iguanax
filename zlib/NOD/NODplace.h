#ifndef __NOD_PLACE_H__
#define __NOD_PLACE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NODplace
//
// Description:
//
// NODplace - represents a node and its grammar.
//
// Internally, there is a vector of reference counted pointers to child
// NODplaces.  Pointers can be NULL, but if they are it just means that
// the data has not been initialized yet.  For example, calling child(0)
// several times will only actually allocate a new NODplace object once,
// and return the stored object every time.
//
// Externally, we can just call child(int) or child(Name) without worrying
// about whether or not the data is initialized - NODplace will take care
// of that for us.  However, NODplaces returned can be in one of two
// possible states - null or non-null.  Use the isNull() function to
// check this.
//
// Author: Kevin Senn
// Date:   Tuesday, September 21st, 2010 @ 11:58:54 AM
//
//---------------------------------------------------------------------------

#include "NODgrammarType.h"

#include <COL/COLref.h>
#include <COL/COLarray.h>

class NODplace : public COLrefCounted
{
public:

   NODplace() {}
   virtual ~NODplace() {}

   //////////////////////////////////////////////////////////////////////////
   // Meta info                                                            //
   //////////////////////////////////////////////////////////////////////////

   virtual NODgrammarType grammarType() const = 0;

   virtual int protocolType() const = 0;
   virtual int nodeType() const = 0;
   virtual const COLstring& protocolName() const = 0;
   virtual const COLstring& nodeTypeName() const = 0;

   virtual const COLstring& name() const = 0;

   virtual bool isReadOnly() const = 0;

   // Some nodes have no name, and even though they may implement the name()
   // function for display purposes, they should not be able to be indexed by name.
   virtual bool isNamed() const { return true; }

   // Only valid for Vector nodes.
   // Returns true if all items in the vector share the same grammar.
   virtual bool isHomogeneous() const = 0;

   // Only valid for Complex nodes.
   virtual bool favorIndexByName() const = 0;

   // Only valid for Complex nodes.  Doesn't actually change any of the
   // NODplace's behavior, but can be used externally to decide how
   // to handle NOD_NO_SUCH_MEMBER errors.
   virtual bool gracefulNoSuchMember() const { return false; }

   // Helpers
   bool isSimple() const;
   bool isVector() const;
   bool isComplex() const;

   // Helper function - we can create a dummy item iff:
   //  1. the node is a vector,
   //  2. the node is null,
   //  3. the node is homogeneous
   bool canCreateDummyItem() const;


   //////////////////////////////////////////////////////////////////////////
   // Data info - valid for all types.                                     //
   //////////////////////////////////////////////////////////////////////////

   // Set this node's contents based given a string.  Will depend on implementation.
   // Returns true if the operation is implemented for this NODplace, false otherwise.
   virtual bool setTreeValue(const COLstring& TreeValue);

   // Is this node null?
   virtual bool isNull() const = 0;
   // Explicitly set the node as null or non-null.
   // A lot of implementations won't need this.
   virtual void setIsNull(bool IsNull) {}


   //////////////////////////////////////////////////////////////////////////
   // For Simple nodes.                                                    //
   //////////////////////////////////////////////////////////////////////////

   virtual void setValue(const COLstring& Value) = 0;
   virtual const COLstring& value() const = 0;
   // This does not change the behaviour of any of the node's functions.
   // Instead, it tells us if we should escape this node's value when generating
   // a textual representation of this node.  Default is true.
   virtual bool shouldEscapeValue() const { return true; }


   //////////////////////////////////////////////////////////////////////////
   // For (Homogeneous) Vector nodes.                                      //
   //////////////////////////////////////////////////////////////////////////

   // Creates a null item for an item in the vector.  Can be used to dig into
   // the grammar for a Vector node with no items.
   virtual const COLref<NODplace> createDummyItem() const = 0;


   //////////////////////////////////////////////////////////////////////////
   // For Vector or Complex nodes.                                         //
   //////////////////////////////////////////////////////////////////////////

   // For Complex nodes, the number of members.  For Vector nodes, the number
   // of items in the vector.
   virtual int size() const { return 0; }

   // For Complex nodes, nullify all children.  For Vector nodes, remove all children.
   virtual void clear() = 0;

   // Remove an item from a Vector, or a member from a Complex type.
   virtual void remove(int i) = 0;

   // Add a child node.  What exactly this method does, and what the parameters
   // represent, depends on the implementation of newChild().  Either Arg2 or both
   // the args may be NULL.
   virtual NODplace& add(const char* Arg1, const char* Arg2) = 0;

   // Insert a child node at the specified index.  What exactly this method does,
   // and what the parameters represent, depends on the implementation of newChild().
   // Either Arg2 or both the args may be NULL.
   virtual NODplace& insert(int i, const char* Arg1, const char* Arg2) = 0;

   // Integer-based indexing.
   virtual NODplace& child(int i) = 0;
   virtual const NODplace& child(int i) const = 0;


   //////////////////////////////////////////////////////////////////////////
   // For Complex nodes.                                                   //
   //////////////////////////////////////////////////////////////////////////

   // Default implementation which is probably nearly as good as any overridden implementation.
   // Returns the index of the first child with the specified name.
   virtual int childIndex(const COLstring& Name) const = 0;

   // Name-based indexing.
   virtual NODplace& child(const COLstring& Name) = 0;
   virtual const NODplace& child(const COLstring& Name) const = 0;

   // Remove a child by name.
   virtual void remove(const COLstring& Name) = 0;

   // Nullify a child (by integer index or name).
   virtual void nullify(int i) = 0;
   virtual void nullify(const COLstring& Name) = 0;

   // BRO code - return the grammar for this node.
   virtual void grammar(COLvar* pGrammar) const {}
protected:

   static const COLstring NOD_EMPTY_SIMPLE_VALUE;

private:
   NODplace(const NODplace& Orig); // not allowed
   NODplace& operator=(const NODplace& Orig); // not allowed
};

COLostream& operator<<(COLostream& Stream, const NODplace& Place);

COLstring NODmemberList(const NODplace& Place);

void NODthrowNoSuchMember(const NODplace& Place, const COLstring& MemberName);

#endif // end of defensive include
