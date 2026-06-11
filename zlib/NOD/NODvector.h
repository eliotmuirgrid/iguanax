#ifndef __NOD_VECTOR_H__
#define __NOD_VECTOR_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NODvector
//
// Description:
//
// NODvector class.  A helper class to make it easier to create a Vector
// subclass of NODplace.  You don't need to use this - you can extend from
// NODplace directly.
//
// Author: Kevin Senn
// Date:   Tuesday, September 21st, 2010 @ 04:32:42 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

#include "NODplaceComposite.h"

class NODvector : public NODplaceComposite
{
public:
   NODvector();
   virtual ~NODvector();

   virtual NODgrammarType grammarType() const { return NOD_VECTOR; }

   bool favorIndexByName() const { return false; }
   void setValue(const COLstring& Value) { COL_ERROR_STREAM_PLAIN("Cannot assign a string value to a node with children.", COLerror::PreCondition); }
   const COLstring& value() const { COLASSERT(isSimple()); return NOD_EMPTY_SIMPLE_VALUE; }

   // Simple default implementation - "null" means the vector has no items.
   virtual bool isNull() const;

private:
   NODvector(const NODvector& Orig); // not allowed
   NODvector& operator=(const NODvector& Orig); // not allowed
};

#endif // end of defensive include
