#ifndef __NOB_COMPLEX_LUA_NODE_H__
#define __NOB_COMPLEX_LUA_NODE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NOBcomplexLuaNode
//
// Description:
//
// Thin extension of NOBcomposite, which contains extra information for
// display purposes.
//
// Author: Kevin Senn
// Date:   Monday, November 14th, 2011 @ 09:41:04 AM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

#include "NOBcomposite.h"

class NOBmemoizer;

class NOBcomplexLuaNode : public NOBcomposite
{
public:
   NOBcomplexLuaNode(NOBmemoizer* pMemoizer, const COLstring& ShortLabel, const COLstring& LongLabel)
      : NOBcomposite(pMemoizer), m_ShortLabel(ShortLabel), m_LongLabel(LongLabel) {}
   // This is non virtual because we are assuming no classes will inherit from this class
   ~NOBcomplexLuaNode() {}

   const COLstring& shortLabel() const { return m_ShortLabel; }
   const COLstring& longLabel()  const { return m_LongLabel;  }

private:
   const COLstring m_ShortLabel;
   const COLstring m_LongLabel;

   NOBcomplexLuaNode(const NOBcomplexLuaNode& Orig); // not allowed
   NOBcomplexLuaNode& operator=(const NOBcomplexLuaNode& Orig); // not allowed
};

// Simple wrapper around dynamic_cast to make code cleaner and easier to understand.
bool NOBisComplexLuaNode(const NODplace& Place);

#endif // end of defensive include
