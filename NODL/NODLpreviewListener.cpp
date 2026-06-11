//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NODLpreviewListener
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Monday, February 14th, 2011 @ 04:19:00 PM
//---------------------------------------------------------------------------
#include "NODLpreviewListener.h"

#include <COL/COLlog.h>
#include <LUA/LUAutils.h>
#include <NODL/NODLconstants.h>
COL_LOG_MODULE;

void NODLpreview(lua_State* L, const NODplace& TreeOutput, NODLpreviewListener::ePreviewType PreviewType){
   LUApushString(L, NODL_PREVIEW_LISTENER);
   lua_gettable(L, LUA_REGISTRYINDEX);
   if (!lua_isnil(L, -1)){
      NODLpreviewListener* pListener = (NODLpreviewListener*)lua_touserdata(L, -1);
      COLASSERT(pListener);
      pListener->onPreviewOutput(TreeOutput, PreviewType);
   }
   lua_pop(L, 1);
}

void NODLpreview(lua_State* L, const COLstring& StringOutput, NODLpreviewListener::ePreviewType PreviewType, const COLstring& ExtraArg){
   LUApushString(L, NODL_PREVIEW_LISTENER);
   lua_gettable(L, LUA_REGISTRYINDEX);
   if (!lua_isnil(L, -1)){
      NODLpreviewListener* pListener = (NODLpreviewListener*)lua_touserdata(L, -1);
      COLASSERT(pListener);
      pListener->onPreviewOutput(StringOutput, PreviewType, ExtraArg);
   }
   lua_pop(L, 1);
}

