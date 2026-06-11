#ifndef __LHL7_PARSE_H__
#define __LHL7_PARSE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LHL7parse
//
// Description:
//
// Lua routine to parse an HL7 message into a HL7 node tree.
//
// Author: Eliot Muir 
// Date:   Wednesday 26 October 2022 - 02:02PM
// ---------------------------------------------------------------------------

struct lua_State;

int LHL7parseMessage(lua_State* L);

#endif // end of defensive include
