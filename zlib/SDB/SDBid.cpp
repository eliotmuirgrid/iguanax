// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBid
//
// Description:
//
// Implementation
//
// Author: John Qi
// Date:   Tue Jun 17 13:11:56 PDT 2025
// ---------------------------------------------------------------------------
#include <SDB/SDBid.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void SDBid::reset() {
   MessageId.reset();
   SequenceId = SDB_INVALID_SID;
   QueueId    = SDB_INVALID_QID;
}

bool SDBid::isNull() const {
   return MessageId.isNull() && SDB_INVALID_SID == SequenceId && SDB_INVALID_QID == QueueId;
}

void SDBid::setBOQ(const COLstring& ComponentId) {
   MessageId.setBOQ(ComponentId);
   SequenceId = SDB_DEFAULT_SID;
   QueueId = SDB_DEFAULT_SID;
}

bool SDBid::isBOQ() const {
   return MessageId.isBOQ() && SDB_INVALID_SID == SequenceId && SDB_INVALID_SID == QueueId;
}

COLostream& operator<<(COLostream& Stream, const SDBid& Obj) {
   Stream << "MessageId=" << Obj.MessageId << ", SequenceId=" << Obj.SequenceId << ", QueueId=" << Obj.QueueId;
   return Stream;
}
