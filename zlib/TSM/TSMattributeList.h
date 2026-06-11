#ifndef __TSM_ATTRIBUTE_LIST_H__
#define __TSM_ATTRIBUTE_LIST_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TSMattributeList
//
// Description:
//
// TXMattributeList class.  This represents a list of attributes basically
// like a vector.  The main idea is that we avoid malloc and free calls by 
// not destroying the vector when we clear it - we merely set the number of
// items in the collection to zero.  So in theory this should play part of
// making a fast XML and or binary parser.
//
// This was originally TXM/TXMattributeList
//
// Author: Eliot Muir
// Date:   Monday, February 12th, 2007 @ 12:30:47 PM
//
//---------------------------------------------------------------------------

#include <COL/COLarray.h>

#include "TSMattribute.h"

class TSMattributeList 
{
public:
   TSMattributeList();
   // This is non virtual because we are assuming no classes will inherit from this class
   ~TSMattributeList();

   COLuint32 countOfAttribute() const;

   TSMattribute& attribute(COLuint32 AttributeIndex);
   const TSMattribute& attribute(COLuint32 AttributeIndex) const;

   const TSMattribute* begin() const;
   const TSMattribute* end() const;

   void addAttribute(const TSMattribute& NewAttribute);
   TSMattribute& addAttribute();

   void clear();
private:
   COLuint32 CountOfAttribute;
   COLarray<TSMattribute> AttributeArray;

   TSMattributeList(const TSMattributeList& Orig); // not allowed
   TSMattributeList& operator=(const TSMattributeList& Orig); // not allowed
};

inline COLuint32 TSMattributeList::countOfAttribute() const
{
   return CountOfAttribute;
}

inline const TSMattribute* TSMattributeList::begin() const
{
   return AttributeArray.begin();
}

inline const TSMattribute* TSMattributeList::end() const
{
   return begin() + CountOfAttribute;
}

inline void TSMattributeList::clear()
{
   CountOfAttribute = 0;
}

#endif // end of defensive include

