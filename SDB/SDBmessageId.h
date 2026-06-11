#ifndef __SDB_MESSAGE_ID_H__
#define __SDB_MESSAGE_ID_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBmessageId
//
// Description:
//
// Data structure that contains information necessary to find a message in
// the log files.
//
// Author: John Qi
// Date:   Tuesday 14 March 2023 - 03:14PM
// ---------------------------------------------------------------------------

#include <SDB/SDBepochTime.h>
#include <SDB/SDBdir.h>
#include <COL/COLstring.h>
#include <COL/COLostream.h>
#include <COL/COLerror.h>
#include <COL/COLminimumInclude.h>

#define SDB_DELETE_CONSUMER 0x44454C   // DEL
#define SDB_NIL_ID          0x4E494C   // NIL

class SDBmessageId {
  public:
   SDBmessageId() {}
   explicit SDBmessageId(const COLstring& iComponentId, COLuint64 iDateTime = SDBtime(), COLuint64 iIndex = 1)
       : DateTime(iDateTime),  ComponentId(iComponentId) {
      if(DateTime != 0) { Index = iIndex; }
   }
   bool isNull() const { return Index == 0 && DateTime == 0 &&  ComponentId.is_null(); }
   bool isZero() const { return Index == 0 && DateTime == 0 && !ComponentId.is_null(); }
   bool isBOQ()  const { return isZero(); }
   void setBOQ(const COLstring& iComponentId) {
      ComponentId = iComponentId;
      DateTime = 0;
      Index = 0;
   }
   void reset() {
      Index = 0;
      DateTime  = 0;
      ComponentId.clear();
   }
   operator COLstring() const {
      COLstring Out;
      COLostream Stream(Out);
      Stream << ComponentId << "-" << DateTime << "-" << Index;
      return Out;
   }

   COLuint64 DateTime  = 0;
   COLuint64 Index = 0;
   COLstring ComponentId;
};

inline bool operator< (const SDBmessageId& lhs, const SDBmessageId& rhs) {
   COLASSERT(lhs.ComponentId == rhs.ComponentId);
   if (lhs.DateTime < rhs.DateTime) { return true;  }
   if (lhs.DateTime > rhs.DateTime) { return false; }
   // lhs.DateTime == rhs.DateTime
   return lhs.Index < rhs.Index;
}
inline bool operator==(const SDBmessageId& lhs, const SDBmessageId& rhs) {
   return
      lhs.DateTime     == rhs.DateTime     &&
      lhs.Index        == rhs.Index        &&
      lhs.ComponentId  == rhs.ComponentId;
}
inline bool operator> (const SDBmessageId& lhs, const SDBmessageId& rhs) { return rhs < lhs; }
inline bool operator<=(const SDBmessageId& lhs, const SDBmessageId& rhs) { return !(lhs > rhs); }
inline bool operator>=(const SDBmessageId& lhs, const SDBmessageId& rhs) { return !(lhs < rhs); }
inline bool operator!=(const SDBmessageId& lhs, const SDBmessageId& rhs) { return !(lhs == rhs); }
inline COLostream& operator<<(COLostream& Stream, const SDBmessageId& Obj) {
   Stream << Obj.ComponentId << "-" << Obj.DateTime << "-" << Obj.Index;
   return Stream;
}

inline SDBmessageId SDBdefaultPosition(const COLstring& QueueId) {
   return SDBmessageId(QueueId, 0, 0);
}

inline bool SDBisDefaultPosition(const SDBmessageId& Id) {
   return 0 == Id.Index && 0 == Id.DateTime && !Id.ComponentId.is_null();
}

inline SDBmessageId SDBnilPosition(const COLstring& QueueId) {
   return SDBmessageId(QueueId, SDB_NIL_ID, SDB_NIL_ID);
}

inline bool SDBisNilPosition(const SDBmessageId& Id) {
   return SDB_NIL_ID == Id.DateTime && SDB_NIL_ID == Id.Index && !Id.ComponentId.is_null();
}

inline SDBmessageId SDBdeleteConsumer(const COLstring& QueueId) {
   return SDBmessageId(QueueId, SDB_DELETE_CONSUMER, SDB_DELETE_CONSUMER);
}

inline bool SDBdeleteConsumer(const SDBmessageId& Id) {
   return SDB_DELETE_CONSUMER == Id.DateTime && SDB_DELETE_CONSUMER == Id.Index && !Id.ComponentId.is_null();
}

#endif // end of defensive include
