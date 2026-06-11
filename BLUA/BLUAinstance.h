#ifndef __BLUA_INSTANCE_H__
#define __BLUA_INSTANCE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: BLUAinstance
//
// Description:
//
// Author: Vismay Shah
// Date:   Friday May 14 2021
//---------------------------------------------------------------------------
#include "BLUAtask.h"

#include <nghttp2/nghttp2.h>

#include <COL/COLweb.h>
#include <COL/COLhashmap.h>
#include <COL/COLmap.h>
#include <COL/COLthreadQueue.h>
#include <SDB/SDBmessageId.h>
#include <SDB/SDBid.h>

struct CFGmap;
struct CFGconfig;
class  SCKlistener;
class  SCKloop;
class  SCKconnection;
class  SCKSsslContext;

struct HTTP2streamData {
   int32_t ID { 0 };
   COLstring Method;
   COLstring Scheme;
   COLstring Authority;
   COLstring Path;
   COLmap<COLstring, COLstring> Headers;
   COLstring Body;
};

struct HTTP2sessionData {
   int ConnectionId { 0 };
   nghttp2_session* pSession { nullptr };
   SCKconnection* pConnection { nullptr };
   COLthreadQueue<BLUAtask>* pTaskQueue { nullptr };
   COLarray<HTTP2streamData*> Streams;
};

class BLUAinstance2 {
public:
   BLUAinstance2(CFGconfig& Config, SCKloop* pLoop);
   ~BLUAinstance2();

   SCKloop* m_pLoop { NULL };
   CFGconfig& m_Config;
   COLthreadQueue<BLUAtask> m_JobQueue;
   COLthreadQueue<COLclosure1<COLvar>*> m_CallbackQueue;    // for BLUAcall
   COLmap<BLUAtaskId, COLauto<COLclosure3<lua_State*&, const COLvar&, int*>> > m_TaskMap;
   COLvar m_Fields; // a snapshot of the custom field values when the component gets started
   bool m_ForceKill { false };
   bool m_IsStopping { false };
   int m_IdleTimer { 0 };
   // stop on error
   bool m_StopOnError { true };
   bool m_ResubmissionMode { false };
   // dequeue
   int m_DequeueListIndex = 0;
   COLarray<COLstring> m_DequeueList;
   SDBid m_DequeuePosition;
   int m_pDequeueRetryTimer { 0 };
   int m_DequeueRetryDelay { -1 };  // for retry dequeue
   // enqueue
   SDBid m_PendingMessage;
   // long sleep
   int m_SleepTimer { 0 };
   // async socket
   int m_ConnId { 0 };
   // component.spawnClone
   int  m_CloneCount = 0;
   COLauto<COLerror> m_pError;
   SCKlistener*    m_pListener      { NULL };
   SCKSsslContext* m_pServerContext { NULL };
   SCKSsslContext* m_pClientContext { NULL };
   COLhashmap<int, SCKconnection*> m_Connections;    // TODO: we can use map right?
   COLhashmap<int, HTTP2sessionData> m_Http2Connections;
   COLclosure0* m_pShutdown { NULL };
   COLmap<int, bool> m_TimerMap;    // TODO: we can use map right?
   bool m_Dequeueing { false };        // for delaying shutdown
};

void BLUAinstanceStopOnError(BLUAinstance2* pInstance, bool Value);
int BLUAsetLight(lua_State* L);
bool BLUAgetCalledFunction(const COLstring& FunctionName, lua_State* L, int* pResult);

void BLUAinstanceRegister(lua_State* L, BLUAinstance2* pInstance);
BLUAinstance2* BLUAinstanceGet(lua_State* L);

// For use in teardown
void BLUAinstanceClearScriptTimers(BLUAinstance2* pInstance);

#endif // end of defensive include
