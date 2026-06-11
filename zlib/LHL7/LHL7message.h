#ifndef __LHL7_MESSAGE_H__
#define __LHL7_MESSAGE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LHL7message
//
// Description:
//
// Lua routine to create an empty HL7 message from a EDI grammar.
//
// Author: Eliot Muir 
// Date:   Friday 07 October 2022 - 08:32AM
// ---------------------------------------------------------------------------

struct lua_State;
class COLstring;
class EDIcollection;

int LHL7newMessage(lua_State* L);

#endif // end of defensive include
