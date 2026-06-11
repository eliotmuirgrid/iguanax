#ifndef __LNET_HTTP_H__
#define __LNET_HTTP_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2014 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LNEThttp
//
// Description:
//
// LNEThttp class.
//
// Authors: Nasron Cheong, Akshay Ganeshen
// Date:    Fri  7 Nov 2014 15:25:40 EST
//
//---------------------------------------------------------------------------

struct lua_State;
class COLstring;
class COLvar;

void LNETaddHttpModule(lua_State* L);

// exposing helpers for LNETrespond
bool LNETgetHeader(const COLvar& Headers, const COLstring& Key, COLstring* pValue=nullptr);
void LNETsetDefaultHeader(COLvar& Headers, const COLstring& Key, const COLstring& Value);
void LNETprocessHeaders(COLvar* pVar);
void LNETsplitHeader(const COLstring& Header, COLstring* pKey, COLstring* pValue);

#endif // end of defensive include
