#include "LEGprecomp.h"
#pragma hdrstop
//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LEGrefVectFunc
//
// Description:
//
// The allocation calculation functions for refvect, that determine the growth
// rate.
//
// Author: Nasron Cheong
// Date:   Jan 2004
//
//---------------------------------------------------------------------------

#include <LEG/LEGrefVect.h>

/**
 * Logarithmic growth
 * 
 * let f(x) be the size at time x.
 *
 * f(x) = ceil( (1 + 1/2) * f(x - 1) );
 *
 */
size_t LEGrefVectResizeFunc(size_t CurrentSize,size_t RequiredSize)
{
   COLPRECONDITION(RequiredSize > 0);
   COLuint32 NewCapacity = CurrentSize;
   if (!NewCapacity)
   {
      NewCapacity++;
   }
   COLuint32 Mod = 0;
   COLuint32 Numerator = 0;
   while (NewCapacity < RequiredSize)
   {
      Mod = NewCapacity % 2;
      NewCapacity += (NewCapacity+Mod)/2;
   }
   return NewCapacity;
}

/**
 * Exponential growth
 * 
 * let f(x) be the size at time x.
 *
 * f(x) = Factor*f(x-1); where Factor is >= 2
 *
size_t LEGrefVectResizeFunc(size_t CurrentSize,size_t RequiredSize)
{
   COLPRECONDITION(RequiredSize > 0);
   COLuint32 NewCapacity = CurrentSize;
   if (!NewCapacity)
   {
      NewCapacity++;
   }
   COLuint32 Factor = 2;
   while (NewCapacity < RequiredSize)
   {
      NewCapacity = NewCapacity * Factor;
   }
   return NewCapacity;
}
 */

/**
 * Linear Growth
 * 
 * let f(x) be the size at time x.
 *
 * f(x) = f(x-1) + Factor; where Factor >= 1
 *
size_t LEGrefVectResizeFunc(size_t CurrentSize,size_t RequiredSize)
{
   COLuint32 NewCapacity = CurrentSize;
   COLuint32 Factor = 10;
   while (NewCapacity < RequiredSize)
   {
      NewCapacity = NewCapacity + Factor;
   }
   return NewCapacity;
}
 */
