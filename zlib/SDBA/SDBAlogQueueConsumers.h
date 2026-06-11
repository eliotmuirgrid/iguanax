#ifndef __SDBA_LOG_QUEUE_CONSUMERS_H__
#define __SDBA_LOG_QUEUE_CONSUMERS_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBAlogQueueConsumers
//
// Description:
//
// Blocking API to return a queue's consumers.
//
// Author: Eliot Muir
// Date:   Friday 05 May 2023 - 03:49PM
// ---------------------------------------------------------------------------

class COLwebRequest;

void SDBAlogQueueConsumers(const COLwebRequest& Request);

#endif // end of defensive include
