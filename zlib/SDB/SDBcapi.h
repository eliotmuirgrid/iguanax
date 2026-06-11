#ifndef __SDB_CAPI_H__
#define __SDB_CAPI_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBcapi
//
// Description:
//
// A Simplified Logging and Queueing interface that takes care of the
// SDBsystem pointer for you.
//
// Author: John Qi
// Date:   Monday 24 April 2023 - 01:17PM
// ---------------------------------------------------------------------------

#include <COL/COLauto.h>
#include <COL/COLclosure.h>
#include <COL/COLarray.h>
#include <SDB/SDBcontent.h>
#include <SDB/SDBdequeue.h>
#include <SDB/SDBmessageId.h>
#include <SDB/SDBpurge.h>
#include <SDB/SDBserviceLog.h>

class COLstring;
class SCKloop;
class SDBmessage;
class SDBsystem;

// NOTE: SDBsystem depends on CFGmap (component configs). Make sure it is loaded first!
void SDBIstartLogQueue(COLclosure0* pDone, SDBsystem* pSystem);
void SDBIstopLogQueue (COLclosure0* pDone, SDBsystem* pSystem);

bool SDBIlogQueueIsRunning();

// Convenience function for logging component messages.
// pData will be replaced by an empty string afterwards.
// Messages larger than SDBmaxMessageSizeMB() will be rejected.
SDBid SDBIlogComponent(COLstring* pData, const COLstring& ComponentId, const COLstring& Tags="", const SDBmessageId& RefId = SDBmessageId(), COLvar* pMeta = nullptr, COLstring* pError = nullptr);
SDBid SDBIlogComponent(COLstring  Data,  const COLstring& ComponentId, const COLstring& Tags="", const SDBmessageId& RefId = SDBmessageId(), COLvar  Meta = COLvar(), COLstring* pError = nullptr);

// Convenience function for logging Iguana Service messages.
// Thread-safe
SDBid SDBIlogService(const COLstring& Data, const COLstring& Tags = "");

// Create a queue for the specified new component
void SDBIcreateQueue(const COLstring& QueueId);

// Create a consumer for the specified queue if one doesn't exist already
void SDBIcreateQueueConsumer(const COLstring& QueueId, const COLstring& ConsumerId);

// Delete the specified consumer from the specified queue if exists
// IMPORTANT: make sure ConsumerId component is stopped before calling this function
void SDBIdeleteQueueConsumer(const COLstring& QueueId, const COLstring& ConsumerId);

// Sets the onMessageEnqueued callback function for the specified component queue.
void SDBIsetMessageEnqueued(const COLstring& QueueId, COLclosure1<SDBid>* pCallback);

// Pushes a message on to the specified queue. Message Id is returned through the callback function on success.
// pData will be replaced by an empty string afterwards.
SDBid SDBIenqueueMessage(COLstring* pData, const COLstring& QueueId, const COLstring& Tags = "", const SDBmessageId& RelatedId=SDBmessageId(), COLvar* pMeta = nullptr, COLstring* pError = nullptr);

// Adds a new queue reader to the system.
void SDBIcreateDequeue(const COLstring& QueueId, const COLstring& ConsumerId, COLauto<COLclosure2<int, SDBmessage*>> pCallback);

// Removes a queue reader from the system.
void SDBIdeleteDequeue(const COLstring& QueueId, const COLstring& ConsumerId);

// Fetches the next N messages from the specified queue. Message and Id are returned through the callback function on success.
// Messages are returned through the callback function. In the event of End-Of-Queue, SDB_END_OF_LOG and NULL are returned.
// Please don't spam this API to check if there is anything to dequeue. Use SDBIsetQueueNotEmpty().
void SDBIdequeueMessage(const COLstring& QueueId, const COLstring& ConsumerId);

// Sets the onPositionCommitted callback function for the specified queue.
void SDBIsetPositionCommitted(const COLstring& QueueId, const COLstring& ConsumerId, COLclosure0* pCallback = NULL);

// Commits the current dequeue position
void SDBIcommitPosition(const SDBid& Position, const COLstring& ConsumerId);

// Returns the current dequeue position of ConsumerId for QueueId
SDBmessageId SDBIcurrentPosition(const COLstring& QueueId, const COLstring& ConsumerId);

// Registers to receive Queue-Not-Empty notifications. When a new messages is committed to disk for the specified queue, the
// registered callback is executed and subsequently removed. You need to register again if you wish to receive another notification.
void SDBIsetQueueNotEmpty(const COLstring& QueueId, const COLstring& ConsumerId, COLclosure0* pCallback = NULL);

// Returns the number of messages in the queue.
int SDBIgetQueueCount(const COLstring& ConsumerId);
// Returns the positions of the queues for the component.
COLarray<SDBmessageId> SDBIgetQueuePosition(const COLstring& ConsumerId);

// Returns all consumers of the specified queue
COLarray<COLstring> SDBIgetQueueConsumers(const COLstring& QueueId);
// Returns all sources of the specified component
COLarray<COLstring> SDBIgetQueueSources(const COLstring& Component);
// Returns the name of a component given the guid - used because names can be changed but the guids remain the same
COLstring SDBIgetComponentName(const COLstring& ComponentGuid);

// Clears messages in the queue by moving the dequeue position to the end of the queue.
void SDBIclearQueueCount(const COLstring& ConsumerId);

// Set the position of the specified queue consumer to the new position
// Returns false if inputs are invalid.
bool SDBIrepositionQueue(const SDBid& Position, const COLstring& ConsumerId);

// Increments inbound or outbound message count for component statistics
void SDBIincrementStatsCount(const COLstring& ComponentId, bool Inbound = true);

// Returns inbound or outbound messages-per-second statistics for the specified component
float SDBIgetComponentStats(const COLstring& ComponentId, bool Inbound = true);

// Returns log stats. component log size and disk usage for now
void SDBIgetLogStats(COLclosure2<SDBlogStats, SDBlogStats>* pCallback);

// Returns the component name given the component guid
COLstring SDBInameFromGuid(const COLstring& Guid);

// Registers the notification system callback
void SDBregisterNotificationCallback(SDBsystem* pSystem, COLclosure1<SDBmessage*>* pCallback);

bool SDBIlogEncryptionEnabled();

COLstring SDBIencryptionKey();

// internal APIs
void SDBIcleanUpQueueCache(const SDBid& TargetId);

// unit test APIs
void SDBITsetTestQueueCount(const COLstring& QueueId, int Count);
void SDBITsetIsUnitTest();

#endif // end of defensive include
