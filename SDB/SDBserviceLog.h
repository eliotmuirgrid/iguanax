#ifndef __SDB_SERVICE_LOG_H__
#define __SDB_SERVICE_LOG_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBserviceLog
//
// Description:
//
// ServiceErrorLog for catching service messages logged when the logging and
// queueing system is not available
//
// Author: John Qi
// Date:   Friday 28 April 2023 - 11:02AM
// ---------------------------------------------------------------------------

class COLstring;
class SDBmessage;

#include <memory>

#include <COL/COLthreadPool.h>
#include <COL/COLthreadQueue.h>

#include <SDB/SDBcontent.h>
#include <SDB/SDBmessage.h>

#define SDB_SERVICE_COMPONENT_ID "Iguana-Service"

class SDBserviceLog {
public:
   COLuint64 LastDateTime  = 0;      // Last message's timestamp, used to determine if NextMessageId needs to be incremented or reset to 1
   COLuint64 NextMessageId = 0;      // Used to distinguish messages with the same millisecond timestamp
   COLuint64 SID = 0;
   COLstring EncryptionKey;
   COLthreadQueue<SDBmessage*> TxtQueue;
   COLthreadQueue<SDBmessage*> LogQueue;
   COLthreadQueue<SDBmessage*> LogQueue2;
   COLmutex Mutex;
};

// Note: message is save to the log file right away
SDBid SDBlogService(const COLstring& Data, const COLstring& Tags = "");

// don't forget to call SDBstopServiceLog() unless you want to stuck in pPool destructor
void SDBstartServiceLog(COLthreadPool* pPool, const COLstring& LogEncryptionKey);
void SDBstopServiceLog();

#endif // end of defensive include
