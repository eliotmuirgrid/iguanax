#ifndef __BLUA_QUEUE_H__
#define __BLUA_QUEUE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: BLUAqueue
//
// Description:
//    BLUAinstance queue functions
//
// Author: Vismay Shah
// Date:   Friday May 14 2021
//---------------------------------------------------------------------------
#include <COL/COLauto.h>
#include <SDB/SDBcontent.h>

class  BLUAinstance2;
class  BLUAtask;
class  COLstring;
class  SCKloop;
struct lua_State;

class BLUAenqueueData {
public:
   COLstring    Data;
   COLstring    Tags;
   COLvar       Meta;
   SDBmessageId RelatedId;
};

// IMPORTANT: Must be called from Lua worker thread.
// Returns messages id in string format: <component_id>-<timestamp>-<message_id>
COLstring BLUAinstanceEnqueue(BLUAinstance2* pInstance, COLauto<BLUAenqueueData> pData, COLstring* pError);


BLUAtask BLUAdequeuedTask(const COLstring& Message, const COLvar& Meta = COLvar());
void BLUAhandleDequeue(lua_State*& L, const COLvar& Payload, int* pResult);
void BLUAinstanceDequeued(BLUAinstance2* pInstance);
void BLUAinstanceDequeueMessage(BLUAinstance2* pInstance);
void BLUAinstanceSetUpDequeue(BLUAinstance2* pInstance);
void BLUAinstanceTearDownDequeue(BLUAinstance2* pInstance);
void BLUAinstanceStopDequeue(BLUAinstance2* pInstance);


int BLUAretryDequeue(lua_State* L);
void BLUAinstanceRetryDequeueSetup(BLUAinstance2* pInstance);

#endif // end of defensive include
