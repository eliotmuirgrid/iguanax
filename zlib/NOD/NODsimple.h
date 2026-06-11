#ifndef __NOD_SIMPLE_H__
#define __NOD_SIMPLE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NODsimple
//
// Description:
//
// NODsimple class.  A helper class to make it easier to create a Simple
// subclass of NODplace.  You don't need to use this - you can extend from
// NODplace directly.
//
// Author: Kevin Senn
// Date:   Tuesday, September 21st, 2010 @ 04:16:45 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

#include "NODplace.h"
#include "NODoperation.h"

class NODsimple : public NODplace
{
public:
   NODsimple();
   virtual ~NODsimple();

   NODgrammarType grammarType() const { return NOD_SIMPLE; }

   bool isHomogeneous() const { COLASSERT(isVector()); return false; }
   bool favorIndexByName() const { COLASSERT(isComplex()); return false; }

   const COLref<NODplace> createDummyItem() const { COLASSERT(!isSimple()); return NULL; }

   virtual void clear();

   void remove(int i) { COL_ERROR_STREAM_PLAIN("This is a leaf node without children.", NOD_IS_LEAF); }
   NODplace& add(const char* Arg1, const char* Arg2) { COL_ERROR_STREAM_PLAIN("This is a leaf node to which children cannot be added.", NOD_IS_LEAF); }
   NODplace& insert(int i, const char* Arg1, const char* Arg2) { COL_ERROR_STREAM_PLAIN("This is a leaf node to which children cannot be added.", NOD_IS_LEAF); }

   NODplace& child(int i) { COL_ERROR_STREAM_PLAIN("This is a leaf node without children.", NOD_IS_LEAF); }
   const NODplace& child(int i) const { COL_ERROR_STREAM_PLAIN("This is a leaf node without children.", NOD_IS_LEAF); }

   int childIndex(const COLstring& Name) const { COL_ERROR_STREAM_PLAIN("This is a leaf node without children.", NOD_IS_LEAF); }
   NODplace& child(const COLstring& Name) { COL_ERROR_STREAM_PLAIN("This is a leaf node without children.", NOD_IS_LEAF); }
   const NODplace& child(const COLstring& Name) const { COL_ERROR_STREAM_PLAIN("This is a leaf node without children.", NOD_IS_LEAF); }

   void remove(const COLstring& Name) { COL_ERROR_STREAM_PLAIN("This is a leaf node without children.", NOD_IS_LEAF); }

   virtual void nullify(int i) { COL_ERROR_STREAM_PLAIN("This is a leaf node without children.", NOD_IS_LEAF); }
   virtual void nullify(const COLstring& Name) { COL_ERROR_STREAM_PLAIN("This is a leaf node without children.", NOD_IS_LEAF); }

private:
   NODsimple(const NODsimple& Orig); // not allowed
   NODsimple& operator=(const NODsimple& Orig); // not allowed
};

#endif // end of defensive include
