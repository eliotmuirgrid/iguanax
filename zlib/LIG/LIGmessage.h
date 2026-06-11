#ifndef __LIG_MESSAGE_H__
#define __LIG_MESSAGE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LIGmessage
//
// Description:
//
// Api to get the current message Id
//
// Author: Vismay Shah 
// Date:   Tuesday 17 October 2023 - 04:47PM
// ---------------------------------------------------------------------------

struct lua_State;

int LIGmessageId(lua_State* L);

#endif // end of defensive include
