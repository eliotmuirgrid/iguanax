#ifndef __LNET_TCP_H__
#define __LNET_TCP_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LNETtcp
//
// Description:
//
// LNETtcp class.
//
// Author: Eric Mulvaney
// Date:   Wednesday, April 6th, 2011 @ 10:41:50 AM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

struct lua_State;
extern const char* LNET_TCP_METATABLE;

// Add the "tcp" module table to some other table.
void LNETaddTcpModule(lua_State*);

#endif // end of defensive include
