#ifndef __LTUN_CONNECTION_ID_H__
#define __LTUN_CONNECTION_ID_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LTUNconnectionId
//
// Description:
//
// Code to grab a COLuint64 from SCKloop, increment its internal counter and
// set the integer as _pConnectionId in the lua state.
//
// Author: Vismay Shah 
// Date:   Monday 10 July 2023 - 11:17AM
// ---------------------------------------------------------------------------
struct lua_State;
class  SCKloop;

void LTUNsetConnectionId(lua_State* L, SCKloop* pLoop);

#endif // end of defensive include
