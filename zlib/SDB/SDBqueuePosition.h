#ifndef __SDB_QUEUE_POSITION_H__
#define __SDB_QUEUE_POSITION_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBqueuePosition
//
// Description:
//
// Queue positioning functions for SDBsystem
//
// Author: John Qi
// Date:   Tuesday 04 April 2023 - 02:27PM
// ---------------------------------------------------------------------------

class COLstring;
class SDBid;
class SDBsystem;

bool SDBrepositionQueue(SDBsystem* pSystem, const SDBid& Position, const COLstring& ConsumerId);
void SDBclearQueue(SDBsystem* pSystem, const COLstring& ComponentId);

// Internal - used by SDBcommitPosition()

void SDBcommitPositionNow(SDBsystem* pSystem, const SDBid& Position, const COLstring& ConsumerId, bool DecrementQueueCount = true);
void SDBrepositionQueueNow(SDBsystem* pSystem, const SDBid& Position, const COLstring& ConsumerId);
void SDBclearQueueNow(SDBsystem* pSystem, const COLstring& QueueId, const COLstring& ComponentId);

#endif // end of defensive include
