//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NOBcomplexLuaNode
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Monday, November 14th, 2011 @ 09:41:04 AM
//
//---------------------------------------------------------------------------
#include "NOBprecomp.h"
#pragma hdrstop

#include "NOBcomplexLuaNode.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

bool NOBisComplexLuaNode(const NODplace& Place){
   return dynamic_cast<const NOBcomplexLuaNode*>(&Place) ? true : false;
}
