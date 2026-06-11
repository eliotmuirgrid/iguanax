#ifndef __TRN_LUA_DEBUG_H__
#define __TRN_LUA_DEBUG_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNluaDebug
//
// Description:
//
// COLostream operator for lua_Debug struct
//
// Author: Eliot Muir 
// Date:   Wednesday 07 June 2023 - 07:30AM
// ---------------------------------------------------------------------------

class COLostream;
struct lua_Debug;
class COLstring;

COLostream& operator<<(COLostream& Stream, const lua_Debug& Debug);

COLstring TRNluaDebugName(lua_Debug* pD);
COLstring TRNluaDebugSource(lua_Debug* pD);

#endif // end of defensive include
