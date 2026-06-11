#ifndef __SDB_ID_H__
#define __SDB_ID_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBid
//
// Description:
//
// A wrapper for SDBmessageId, sequence id and queue id
//
// Every message has a sequence id (SID) and a queue id (QID)
// SID starts at 1 and is incremented each time a message (of any type) is added to a component log
// QID starts at 0 and is incremented only after an SDB_MESSAGE messages is added to the component log
// for non-SDB_MESSAGE messages, their QID is the same of the previous SDB_MESSAGE messages's QID
// for example
// Type   Info, Warn, Msg, Dbg, Info, Info, Msg, Warn
// SID    1     2     3    4    5     6     7    8
// QID    0     0     1    1    1     1     2    2
//
// Author: John Qi
// Date:   Tue Jun 17 13:11:56 PDT 2025
// ---------------------------------------------------------------------------
class COLstring;
#include <COL/COLminimumInclude.h>
#include <SDB/SDBmessageId.h>

#define SDB_INVALID_SID COL_UINT_64_MAX
#define SDB_INVALID_QID COL_UINT_64_MAX

#define SDB_DEFAULT_SID 0
#define SDB_DEFAULT_QID 0

class SDBid {
public:
   SDBid() {};
   SDBid(SDBmessageId Mid, COLuint64 Sid, COLuint64 Qid) : MessageId(Mid), SequenceId(Sid), QueueId(Qid) {};
   void reset();
   bool isNull() const;
   void setBOQ(const COLstring& ComponentId);
   bool isBOQ() const;
   SDBmessageId MessageId;
   COLuint64    SequenceId { SDB_INVALID_SID };
   COLuint64    QueueId    { SDB_INVALID_QID };
};

inline bool operator< (const SDBid& lhs, const SDBid& rhs) {
   return lhs.MessageId < rhs.MessageId;
}
inline bool operator==(const SDBid& lhs, const SDBid& rhs) {
   return lhs.MessageId  == rhs.MessageId;
}
inline bool operator> (const SDBid& lhs, const SDBid& rhs) { return rhs < lhs; }
inline bool operator<=(const SDBid& lhs, const SDBid& rhs) { return !(lhs > rhs); }
inline bool operator>=(const SDBid& lhs, const SDBid& rhs) { return !(lhs < rhs); }
inline bool operator!=(const SDBid& lhs, const SDBid& rhs) { return !(lhs == rhs); }

COLostream& operator<<(COLostream& Stream, const SDBid& Obj);

#endif // end of defensive include
