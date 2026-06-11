#ifndef __LHL_7API_H__
#define __LHL_7API_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LHL7api
//
// Description:
//
// Lua API for hl7.parseMessage, hl7.newMessage etc.
//
// Author: Eliot Muir 
// Date:   Friday 07 October 2022 - 08:30AM
// ---------------------------------------------------------------------------

struct lua_State;

void LHL7api(lua_State* L);

#endif // end of defensive include
