#ifndef __LCOMSETFIELD_H__
#define __LCOMSETFIELD_H__
///  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: LCOMsetField.h
//
//  Description: Set the field of a component programmatically
//  Lua binding for component.setField
//
//  Author: Matthew Sobkowski
//  Date:   23/01/24 11:50 AM
//  ---------------------------------------------------------------------------
struct lua_State;

int LCOMsetField(lua_State* L);

#endif // end of defensive include
