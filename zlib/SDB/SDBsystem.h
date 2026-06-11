#ifndef __SDB_SYSTEM_H__
#define __SDB_SYSTEM_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBsystem
//
// Description:
//
// Iguana logging and queueing system
//
// Author: Eliot Muir
// Date:   Monday 27 February 2023 - 01:02PM
// ---------------------------------------------------------------------------

#include <CFG/CFGmap.h>

#include <COL/COLminimumInclude.h>
#include <COL/COLhashmap.h>
#include <COL/COLlist.h>
#include <COL/COLclosure.h>
#include <COL/COLauto.h>

#include <SDB/SDBdequeue.h>
#include <SDB/SDBjournalThread.h>
#include <SDB/SDBlogThread.h>
#include <SDB/SDBmessageId.h>
#include <SDB/SDBpurge.h>
#include <SDB/SDBstats.h>
#include <SDB/SDBcomponent.h>

#include <memory>

class COLstring;
class SCKloop;
class SDBmessage;

#define SDB_SYNC_WRITE_ENV_NAME "IFW_SYNC_WRITE"


class SDBsystem{
public:
   SDBsystem(SCKloop* pLoop, CFGmap& Configs, const COLstring& EncryptionKey = "");
   ~SDBsystem();
   SCKloop* pLoop;
   CFGmap& ConfigMap;  // a reference to component config map
   COLhashmap<COLstring, SDBcomponent> ComponentMap;
   COLhashmap<COLstring, SDBid> RepositionQueue;  // set if queue repositioning has been requested for a component
   COLhashmap<COLstring, bool>         ClearQueue;       // set if clear queue has been requested for a component
   SDBstats StatsMap;
   COLstring JournalBuffer;
   int CacheCleanupTimer = 0;
   int LogPurgerTimer = 0;
   COLarray<SDBpurgeRule> LogPurgeRules;
   // this callback is for interacting with notifications
   COLauto<COLclosure1<SDBmessage*>> pCheckNotifications;
   COLthreadQueue<SDBjournalTask> JournalTaskQueue;
   COLthreadQueue<SDBlogTask> LogTaskQueue;
   int IdleLogHandleTimer = 0;
   COLthreadQueue<SDBjournalTask> JournalTaskQueue2;
   COLthreadQueue<SDBlogTask> LogTaskQueue2;
   int IdleLogHandleTimer2 = 0;
   COLstring LogEncryptionKey;
};

void SDBregisterComponent   (SDBsystem* pSystem, const COLstring& ComponentId);

void SDBsetMessageQueued    (SDBsystem* pSystem, const COLstring&  QueueId, COLclosure1<SDBid>* pCallback = NULL);
SDBid SDBlogMessage         (SDBsystem* pSystem, std::shared_ptr<SDBmessage> pMessage);

void SDBcreateDequeue       (SDBsystem* pSystem, const COLstring&  QueueId, const COLstring& ConsumerId, const COLstring& EncryptionKey, COLauto<COLclosure2<int, SDBmessage*>> pCallback);
void SDBdeleteDequeue       (SDBsystem* pSystem, const COLstring&  QueueId, const COLstring& ConsumerId);
void SDBdequeueMessage      (SDBsystem* pSystem, const COLstring&  QueueId, const COLstring& ConsumerId);

void SDBsetPositionCommitted(SDBsystem* pSystem, const COLstring&  QueueId, const COLstring& ConsumerId, COLclosure0* pCallback = NULL);
void SDBsetQueueNotEmpty    (SDBsystem* pSystem, const COLstring&  QueueId, const COLstring& ConsumerId, COLclosure0* pCallback = NULL);
void SDBcommitPosition      (SDBsystem* pSystem,           SDBid  Position, const COLstring& ConsumerId);

SDBmessageId SDBcurrentPosition(SDBsystem* pSystem, const COLstring&  QueueId, const COLstring& ConsumerId);

void SDBcleanUpQueueCache(SDBsystem* pSystem, SDBid TargetId);

COLarray<SDBmessageId> SDBqueuePosition(SDBsystem* pSystem, const COLstring& ComponentId);


#endif // end of defensive include
