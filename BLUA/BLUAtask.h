#ifndef __BLUA_TASK_H__
#define __BLUA_TASK_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: BLUAtask
//
// Description:
//    Tasks for BLUAinstance.
//
// Author: Vismay Shah
// Date:   Friday May 14 2021
//---------------------------------------------------------------------------
#include <COL/COLauto.h>
#include <COL/COLclosure.h>
#include <COL/COLvar.h>

#if defined(__linux__)
#include <cstdint>
#endif

class  BLUAinstance2;
struct lua_State;

// Switch to string for less maintenance.
enum BLUAtaskId {
   BLUA_RUN,
   BLUA_KILL,
   BLUA_RM_IDLE,
   BLUA_ON_CALL,
   BLUA_ON_DEQUEUE,
   BLUA_ON_TIMER,
   BLUA_ON_ACCEPT,
   BLUA_ON_CONNECT,
   BLUA_ON_DATA,
   BLUA_ON_WRITE,
   BLUA_ON_CLOSE,
   BLUA_HTTP_DATA
};

enum BLUAtaskResult {
   BLUA_ERROR_INTERNAL = INT32_MIN,
   BLUA_ERROR_COMPILE,
   BLUA_TERMINATE,
   BLUA_FORCE_KILLED,
   BLUA_UNKNOWN=-1
};

struct BLUAtask {
   BLUAtaskId Id;
   COLvar Payload;
};

void BLUAregisterTasks(BLUAinstance2* pInstance);
COLostream& operator<<(COLostream& Stream, BLUAtaskId TaskItem);

COLstring BLUAtaskResultAsString(int Result);


#endif
