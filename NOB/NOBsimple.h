#ifndef __NOB_SIMPLE_H__
#define __NOB_SIMPLE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NOBsimple
//
// Description:
//
// NOBsimple class.  A simple NODplace with a non-empty value.
//
// Author: Kevin Senn
// Date:   Thursday, September 15th, 2011 @ 09:40:42 AM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

#include "NOBsimpleEmpty.h"
#include "NOBmemoizer.h"

class NOBsimple : public NOBsimpleEmpty
{
public:
   NOBsimple(NOBmemoizer* pMemoizer) : NOBsimpleEmpty(pMemoizer) {}

   const COLstring& value() const { return m_Value; }

   void setCachedValue(const COLstring& Value) { m_Value = Value; }

private:
   COLstring m_Value;

   NOBsimple(const NOBsimple& Orig); // not allowed
   NOBsimple& operator=(const NOBsimple& Orig); // not allowed
};

#endif // end of defensive include
