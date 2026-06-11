#ifndef __BLUA_IDEL_CONNECTION_H__
#define __BLUA_IDEL_CONNECTION_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: BLUAidleConnection
//
// Description:
//    BLUAinstance lua remove idle connection task
//
// Author: John Q
// Date:   Wednesday Oct 5 2022
//---------------------------------------------------------------------------

class  COLvar;
struct BLUAinstance2;
struct lua_State;

void BLUAhandleRemoveIdleConnection(lua_State*& L, const COLvar& Payload, int* pResult);

void BLUAstartIdleConnectionPurgeTimer(BLUAinstance2* pInstance);
void BLUAcancelIdleConnectionPurgeTimer(BLUAinstance2* pInstance);

#endif // end of defensive include
