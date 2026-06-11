#ifndef __LCOM_RUN_AS_H__
#define __LCOM_RUN_AS_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LCOMrunAs
//
// lua binding for component.runAsUser() (formerly component.runAs)
//
// Author: Matthew Sobkowski
// Date:   Monday 24 April 2023 - 3:59PM
// ---------------------------------------------------------------------------
struct lua_State;
class  COLstring;

int LCOMrunAsUser(lua_State* L);

// returns true if the RunAs user is set and the username is returned through pUser.
// returns false if the RunAs user is not set
bool LCOMgetRunAs(lua_State* L, COLstring* pUser);

#endif // end of defensive include
