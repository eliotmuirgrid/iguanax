#ifndef __NOD_COMPLEX_H__
#define __NOD_COMPLEX_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NODcomplex
//
// Description:
//
// NODcomplex class.  A helper class to make it easier to create a Complex
// subclass of NODplace.  You don't need to use this - you can extend from
// NODplace directly.
//
// Author: Kevin Senn
// Date:   Tuesday, September 21st, 2010 @ 04:47:06 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

#include "NODplaceComposite.h"

class NODcomplex : public NODplaceComposite
{
public:
   NODcomplex();
   virtual ~NODcomplex();

   virtual NODgrammarType grammarType() const { return NOD_COMPLEX; }

   bool isHomogeneous() const { COLASSERT(isVector()); return false; }
   void setValue(const COLstring& Value) { COL_ERROR_STREAM_PLAIN("Cannot assign a string value to a node with children.", COLerror::PreCondition); }
   const COLstring& value() const { COLASSERT(isSimple()); return NOD_EMPTY_SIMPLE_VALUE; }

private:
   NODcomplex(const NODcomplex& Orig); // not allowed
   NODcomplex& operator=(const NODcomplex& Orig); // not allowed
};

#endif // end of defensive include
