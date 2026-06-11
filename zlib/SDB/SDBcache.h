#ifndef __SDB_CACHE_H__
#define __SDB_CACHE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBcache
//
// Description:
//
// Functions for cacheing data messages in memory.
//
// Author: John Qi
// Date:   Thursday 09 March 2023 - 03:49PM
// ---------------------------------------------------------------------------
#include <memory>

#include <COL/COLlist.h>
#include <SDB/SDBmessage.h>

// Maximum queue cache size. Make it env var configurable?
#define SDB_MAX_CACHE_SIZE 16777216  // 16 MiB
// Queue consumers that have been idling or too slow have to dequeue from log file
// IMPORTANT: make sure SDB_SLOW_CONSUMER_LIMIT is larger than SDB_DEFAULT_MAX_LOG_FLUSH_INTERVAL
// or dequeue could skip messages. IX-4310
#define SDB_SLOW_CONSUMER_LIMIT 30  // seconds

class COLstring;
class SDBcomponent;
class SDBmessageId;
class SDBsystem;

// returns the total size of messages at the beginning of pMessageList that have the same date time
int SDBcalculateLogSize(const COLlist<std::shared_ptr<SDBmessage>>* pMessageList);

// adds comitted messages to the in-memory cache
void SDBcacheEnqueuedMessages(SDBcomponent& Queue, COLlist<std::shared_ptr<SDBmessage>>* pComittedMessages);

// removes logged messages from the in-memory cache
void SDBevictLoggedMessages(SDBcomponent& Queue, const SDBid& LastLoggedId);

// starts the cache cleanup cycle
void SDBcleanUpCache(SDBsystem* pSysytem);
// stops the cache cleanup cycle
void SDBstopCacheCleanupCycle(SDBsystem* pSysytem);

// helper function for removing messages from cache
void SDBremoveMessagesFromList(COLlist<std::shared_ptr<SDBmessage>>* pList, const SDBid& UpTo);

// unit test
SDBid SDBdetermineEvictee(COLlist<std::shared_ptr<SDBmessage>>* pCache, const SDBid& Limit);
SDBid SDBdetermineCleanupee(SDBcomponent& Queue);
void SDBevictMessages(SDBcomponent& Queue, const SDBid& Upto);

#endif // end of defensive include
