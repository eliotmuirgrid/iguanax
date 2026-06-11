#ifndef __LX12_MESSAGE_H__
#define __LX12_MESSAGE_H__
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: LX12message.h
//
//  Description:
//
//  Implementation
//
//  Author: Matthew Sobkowski
//  Date:   3/21/23 2:18 PM
//  ---------------------------------------------------------------------------
struct lua_State;
class COLstring;
class EDIcollection;

int LX12newMessage(lua_State* L);

#endif
