// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBTmessage
//
// Description:
//
// Implementation
//
// Author: John Qi
// Date:   Wednesday 08 March 2023 - 10:52AM
// ---------------------------------------------------------------------------

#include <SDBT/SDBTmessage.h>

#include <SDB/SDBmessage.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

SDBmessage* SDBTcreateTestMessage(const COLstring& ComponentId, int Index, int Size, time_t Date) {
   COL_FUNCTION(SDBTcreateTestMessage);
   COLstring Space(Size, '\x99');
   SDBmessage* pMessage = new SDBmessage();
   pMessage->Id.MessageId.Index = Index;
   pMessage->Id.MessageId.DateTime = Date * 1000;
   pMessage->Id.MessageId.ComponentId = ComponentId;
   pMessage->Data = "This is message number " + COLintToString(Index) + Space;
   switch (Index % 4) {
      case 0:  pMessage->Type = SDB_MESSAGE;   pMessage->Data += ", a Message log entry.";  break;
      case 1:  pMessage->Type = SDB_INFO;      pMessage->Data += ", an Info log entry.";    break;
      case 2:  pMessage->Type = SDB_WARNING;   pMessage->Data += ", a Warning log entry.";  break;
      case 3:  pMessage->Type = SDB_ERROR;     pMessage->Data += ", an Error log entry.";   break;
      default: COLASSERT(false);                                                                   break;
   }
   COLint64 LastDigit = Index % 10;
   COLint64 RelatedId = Index - LastDigit * 100;
   if (RelatedId > 0) {
      pMessage->RelatedId.Index = RelatedId;
      pMessage->RelatedId.ComponentId = ComponentId;
   }
   return pMessage;
}

COLstring SDBTcreateSerializedTestMessage(const SDBmessage* pMessage) {
   COL_FUNCTION(SDBTcreateSerializedTestMessage);
   COLstring Buffer;
   Buffer.setCapacity(SDBmessageSize(pMessage));
   int Amount = SDBwriteMessage(pMessage, Buffer.get_buffer(), Buffer.capacity()-1);
   Buffer.setSize(Amount);
   return Buffer;
}

COLstring SDBTcreateSerializedTestMessage(const COLstring& ComponentId, int Index, int Size, time_t Date) {
   COL_FUNCTION(SDBTcreateSerializedTestMessage);
   COLauto<SDBmessage> pMessage = SDBTcreateTestMessage(ComponentId, Index, Size, Date);
   return SDBTcreateSerializedTestMessage(pMessage);
}

void SDBTcreateTestMessageList(COLarray<SDBmessage*>* pOut, int Count, const COLstring& ComponentId, int Index, int Size, time_t Date) {
   COL_FUNCTION(SDBTcreateTestMessageList);
   pOut->setCapacity(Count);
   for (COLuint64 i = Index; i < Index+Count; ++i) {
      pOut->push_back(SDBTcreateTestMessage(ComponentId, i, Size, Date+i-Index));
   }
}

void SDBTcreateTestMessageList(COLarray<COLauto<SDBmessage>>* pOut, int Count, const COLstring& ComponentId, int Index, int Size, time_t Date) {
   COL_FUNCTION(SDBTcreateTestMessageList);
   pOut->setCapacity(Count);
   for (COLuint64 i = Index; i < Index+Count; ++i) {
      pOut->push_back(SDBTcreateTestMessage(ComponentId, i, Size, Date+i-Index));
   }
}
