#ifndef __NOD_MEMOIZER_H__
#define __NOD_MEMOIZER_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NODmemoizer
//
// Description:
//
// NODmemoizer class.  Maps integers to string values.  Not used in the NOD
// library itself, but is a useful tool for implementing libraries that will
// reuse the same string values many times.
//
// Author: Kevin Senn
// Date:   Monday, September 19th, 2011 @ 10:57:39 AM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>
#include <COL/COLrefCounted.h>
#include <COL/COLhashmap.h>

class NODmemoizer : public COLrefCounted
{
public:
   NODmemoizer(): m_NextId(0) {}
   virtual ~NODmemoizer() {}

   // Will store the value and create a new key, if necessary.
   COLint64 idFromValue(const COLstring& Value);

   const COLstring& valueFromId(const COLint64 Id) const;

   // Should only be used when reconstructing a serialized tree.
   void add(const COLint64 Id, const COLstring& Value);

   // Functions for generic string value iteration.
   inline COLhashmapPlace first() const { return m_Lookup.first(); }
   inline COLhashmapPlace next(COLhashmapPlace Place) const { return m_Lookup.next(Place); }
   inline COLint64 id(COLhashmapPlace Place) const { return m_Lookup[Place]; }
   inline const COLstring& value(COLhashmapPlace Place) const { return m_Lookup.key(Place); }

private:
   COLint64 m_NextId;

   COLhashmap<COLstring, COLint64>        m_Lookup;
   COLhashmap<COLint64, const COLstring*> m_ReverseLookup;

   NODmemoizer(const NODmemoizer& Orig); // not allowed
   NODmemoizer& operator=(const NODmemoizer& Orig); // not allowed
};

#endif // end of defensive include
