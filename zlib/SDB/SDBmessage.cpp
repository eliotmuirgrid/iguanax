// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBlog
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Thursday 16 February 2023 - 03:44PM
// ---------------------------------------------------------------------------

#include <SDB/SDBmessage.h>

#include <SDB/SDBdir.h>
#include <SDB/SDBepochTime.h>
#include <SDB/SDBnumber.h>
#include <SDB/SDBstring.h>

#include <MATCH/MATCHsearch.h>
#include <FIL/FILosDefs.h>

#include <COL/COLdateTime.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

SDBmessage::SDBmessage(){
   COL_METHOD(SDBmessage::SDBmessage);
}

SDBmessage::SDBmessage(const COLstring& ComponentId, const COLstring& iData, COLuint64 DateTime, COLuint64 Index){
   COL_METHOD(SDBmessage::SDBmessage);
   Data = iData;
   Id.MessageId.DateTime = DateTime;
   Id.MessageId.Index = Index;
   Id.MessageId.ComponentId = ComponentId;
   // TODO: set sequence and queue id
}

bool operator==(const SDBmessage& lhs, const SDBmessage& rhs) {
   return
      lhs.Type      == rhs.Type      &&
      lhs.Id        == rhs.Id        &&
      lhs.RelatedId == rhs.RelatedId &&
      lhs.Data      == rhs.Data      &&
      // skip Tags for comparison because SDBreadMessage() auto-adds tags
      // lhs.Tags      == rhs.Tags      &&
      lhs.Meta      == rhs.Meta;
}

COLostream& operator<<(COLostream& Stream, const SDBmessage& Obj) {
   Stream << "Type=" << Obj.Type << ", Tags=" << Obj.Tags << ", ID=[ " << Obj.Id << " ], Ref=" << Obj.RelatedId
          << ", Message=" << Obj.Data.substr(0, 32) << ", Size=" << Obj.Data.size() << ", Meta=" << Obj.Meta;
   return Stream;
}

int SDBmessageSize(const SDBmessage* pMessage){
   COL_FUNCTION(SDBmessageSize);
   return
      SDBnumberSize(SDB_MESSAGE) +
      SDBnumberSize(pMessage->Id.MessageId.Index) +
      SDBnumberSize(pMessage->Id.MessageId.DateTime) +
      SDBnumberSize(pMessage->RelatedId.Index) +
      SDBnumberSize(pMessage->RelatedId.DateTime) +
      SDBstringSize(pMessage->RelatedId.ComponentId) +
      SDBstringSize(pMessage->Id.MessageId.ComponentId) +
      SDBstringSize(pMessage->Data) +
      SDBnumberSize(pMessage->Id.SequenceId) +
      SDBnumberSize(pMessage->Id.QueueId) +
      SDBstringSize(pMessage->Tags) +
      SDBstringSize(pMessage->Meta.json());
}

int SDBwriteMessage(const SDBmessage* pMessage, char* pBuffer, int BufferSize){
   COL_FUNCTION(SDBwriteMessage);
   if (BufferSize < SDBmessageSize(pMessage)) {
      COL_ERR("Not enough data in buffer for a message");
      return -1;
   }
   char* pOut = pBuffer;
   // The first element of the data is the message type since this allows to state the contents of a payload
   // rather like C APIs like berkley sockets that follow this pattern - size of struct plus a number identifying it
   pOut += SDBwriteNumber(pMessage->Type,                   pOut);
   pOut += SDBwriteNumber(pMessage->Id.MessageId.Index,     pOut);
   pOut += SDBwriteNumber(pMessage->Id.MessageId.DateTime,  pOut);
   pOut += SDBwriteNumber(pMessage->RelatedId.Index,        pOut);
   pOut += SDBwriteNumber(pMessage->RelatedId.DateTime,     pOut);
   pOut += SDBwriteString(pMessage->RelatedId.ComponentId,  pOut);
   pOut += SDBwriteString(pMessage->Id.MessageId.ComponentId,pOut);
   pOut += SDBwriteString(pMessage->Data,                   pOut);
   // adding sequence id and meta data for HA
   COLASSERT(pMessage->Id.SequenceId >= 0);
   pOut += SDBwriteNumber(pMessage->Id.SequenceId,          pOut);   // maybe better to use fixed width instead of variable
   COLASSERT(pMessage->Id.QueueId >= 0);
   pOut += SDBwriteNumber(pMessage->Id.QueueId,             pOut);   // maybe better to use fixed width instead of variable
   pOut += SDBwriteString(pMessage->Tags,                   pOut);
   pOut += SDBwriteString(pMessage->Meta.json(),            pOut);
   return pOut-pBuffer;
}

static void SDBprependTags(SDBmessage* pMessage, const COLstring& Tags) {
   if (pMessage->Tags.size()) {
      pMessage->Tags.prepend(Tags + " ");
   } else {
      pMessage->Tags = Tags;
   }
}

static void SDBprependTypeTagsForOldLogMessage(SDBmessage* pMessage) {
   switch (pMessage->Type) {
      case SDB_MESSAGE: SDBprependTags(pMessage, "#message"); break;
      case SDB_INFO:    SDBprependTags(pMessage, "#info");    break;
      case SDB_DEBUG:   SDBprependTags(pMessage, "#debug");   break;
      case SDB_WARNING: SDBprependTags(pMessage, "#warning"); break;
      case SDB_ERROR:   SDBprependTags(pMessage, "#error");   break;
      case SDB_SUCCESS: SDBprependTags(pMessage, "#success"); break;
      default:                                                break;
   }
}

bool SDBhasTag(COLstring Tags, const COLstring& Tag) {
   if (Tags.size() == 0) { return false; }
   return MATCHsearch("#error", Tags, true);
}

// log browser relies on SDB_ERROR type to render the icon for error messages. IX-4185
static void SDBsetTypeForErrorMessages(SDBmessage* pMessage) {
   if (SDB_TRACE != pMessage->Type) { return; }
   if (SDBhasTag(pMessage->Tags, "#error")) {
      pMessage->Type = SDB_ERROR;
   }
}

int SDBreadMessage(SDBmessage* pMessage, const char* pBuffer, int BufSize){
   COL_FUNCTION(SDBreadMessage);
   const char* pIn = pBuffer;
   int Amount = 0;
   do {
      Amount = SDBreadNumber(&pMessage->Type,                  pIn, BufSize); if (Amount < 0) { break; } pIn += Amount; BufSize -= Amount;
      Amount = SDBreadNumber(&pMessage->Id.MessageId.Index,    pIn, BufSize); if (Amount < 0) { break; } pIn += Amount; BufSize -= Amount;
      Amount = SDBreadNumber(&pMessage->Id.MessageId.DateTime, pIn, BufSize); if (Amount < 0) { break; } pIn += Amount; BufSize -= Amount;
      Amount = SDBreadNumber(&pMessage->RelatedId.Index,       pIn, BufSize); if (Amount < 0) { break; } pIn += Amount; BufSize -= Amount;
      Amount = SDBreadNumber(&pMessage->RelatedId.DateTime,    pIn, BufSize); if (Amount < 0) { break; } pIn += Amount; BufSize -= Amount;
      Amount = SDBreadString(&pMessage->RelatedId.ComponentId, pIn, BufSize); if (Amount < 0) { break; } pIn += Amount; BufSize -= Amount;
      Amount = SDBreadString(&pMessage->Id.MessageId.ComponentId,pIn, BufSize); if (Amount < 0) { break; } pIn += Amount; BufSize -= Amount;
      Amount = SDBreadString(&pMessage->Data,                  pIn, BufSize); if (Amount < 0) { break; } pIn += Amount; BufSize -= Amount;
      COL_VAR(BufSize);
      if (BufSize > 0) {
         COL_TRC("we have sequence ID and queue ID");
         COLuint64 SequenceId = 0;
         Amount = SDBreadNumber(&SequenceId, pIn, BufSize);
         if (Amount > 0) {
            pMessage->Id.SequenceId = SequenceId;
            // COL_VAR(SequenceId);
            pIn += Amount; BufSize -= Amount;
         } else { break; }

         COLuint64 QueueId = 0;
         Amount = SDBreadNumber(&QueueId, pIn, BufSize);
         if (Amount > 0) {
            pMessage->Id.QueueId = QueueId;
            // COL_VAR(QueueId);
            pIn += Amount; BufSize -= Amount;
         } else { break; }

         COLstring Tags;
         Amount = SDBreadString(&Tags, pIn, BufSize);
         if (Amount > 0) {
            pMessage->Tags = Tags;
            SDBprependTypeTagsForOldLogMessage(pMessage);
            SDBsetTypeForErrorMessages(pMessage);
            pIn += Amount; BufSize -= Amount;
         } else { break; }

         COLstring Metadata;
         Amount = SDBreadString(&Metadata, pIn, BufSize);
         if (Amount > 0) {
            if (!pMessage->Meta.parse(Metadata)) {
               COL_TRC("failed to parse metadata, assign raw");
               pMessage->Meta = Metadata;
            };
            pIn += Amount;
            BufSize -= Amount;
         } else { break; }
      } else {
         COL_TRC("Old log. No sequence ID and queue ID. Default to 0");
         pMessage->Id.SequenceId = SDB_DEFAULT_SID;
         pMessage->Id.QueueId    = SDB_DEFAULT_QID;
      }
      if (!SDBisMessage(pMessage->Type)) { COL_ERR("Not a message."); return -2; }
      return pIn - pBuffer;
   } while (false);
   if (Amount < -1) {
      COL_ERR("Failed to read number: " << Amount);
   } else {
      // COL_ERR("Not enough data for a complete message");
   }
   return Amount;
}

bool SDBisMessage(COLuint64 Type){
   COL_FUNCTION(SDBisMessage);
   switch(Type){
   case SDB_MESSAGE:
   case SDB_TRACE:
   case SDB_INFO:
   case SDB_DEBUG:
   case SDB_WARNING:
   case SDB_ERROR:
   case SDB_SUCCESS:
      COL_TRC("Is Message");
      return true;
   default:
      COL_TRC("Not message");
      return false;
   }
}

bool SDBisMessage(const SDBcontent* pContent){
   COL_FUNCTION(SDBisMessage);
   return SDBisMessage(pContent->Type);
}

int SDBmessageSize(const SDBcontent* pMessage){
   return SDBmessageSize((const SDBmessage*)pMessage);
}

int SDBwriteMessage(const SDBcontent* pMessage, char* pBuffer, int BufferSize){
   return SDBwriteMessage((const SDBmessage*)pMessage, pBuffer, BufferSize);
}

int SDBreadMessage(SDBcontent* pMessage, const char* pBuffer, int BufferSize){
   return SDBreadMessage((SDBmessage*)pMessage, pBuffer, BufferSize);
}

COLostream& SDBprintMessage(const SDBcontent* pMessage, COLostream& Stream) {
   return Stream << *(const SDBmessage*)pMessage;
}