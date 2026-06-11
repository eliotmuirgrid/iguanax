// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBposition
//
// Description:
//
// Implementation
//
// Author: John Qi
// Date:   Tuesday 14 March 2023 - 03:14PM
// ---------------------------------------------------------------------------

#include <SDB/SDBposition.h>

#include <SDB/SDBdir.h>
#include <SDB/SDBmessage.h>
#include <SDB/SDBnumber.h>
#include <SDB/SDBstring.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

SDBposition::SDBposition(const COLstring& iConsumerId, const SDBid& iPosition) {
   ConsumerId = iConsumerId;
   Position = iPosition;
}

bool operator<(const SDBposition& lhs, const SDBposition& rhs) {
   COLASSERT(lhs.ConsumerId == rhs.ConsumerId);
   return lhs.Position < rhs.Position;
}

bool operator==(const SDBposition& lhs, const SDBposition& rhs) {
   return
      lhs.Type       == rhs.Type        &&
      lhs.Position   == rhs.Position    &&
      lhs.ConsumerId == rhs.ConsumerId;
}

COLostream& operator<<(COLostream& Stream, const SDBposition& Obj){
   Stream << "Consumer=" << Obj.ConsumerId << ", Position=[ " << Obj.Position << " ]";
   return Stream;
}

int SDBpositionSize(const SDBposition* pPosition) {
   COL_FUNCTION(SDBpositionSize);
   return
      SDBnumberSize(SDB_POSITION) +
      SDBnumberSize(pPosition->Position.MessageId.Index) +
      SDBnumberSize(pPosition->Position.MessageId.DateTime) +
      SDBstringSize(pPosition->Position.MessageId.ComponentId) +
      SDBstringSize(pPosition->ConsumerId) +
      SDBnumberSize(pPosition->Position.SequenceId) +
      SDBnumberSize(pPosition->Position.QueueId);
}

int SDBwritePosition(const SDBposition* pPosition, char* pBuffer, int BufferSize) {
   COL_FUNCTION(SDBwritePosition);
   if (BufferSize < SDBpositionSize(pPosition)) {
      COL_ERR("Not enough space in buffer to write a position");
      return -1;
   }
   char* pOut = pBuffer;
   pOut += SDBwriteNumber(pPosition->Type,                           pOut);
   pOut += SDBwriteNumber(pPosition->Position.MessageId.Index,       pOut);
   pOut += SDBwriteNumber(pPosition->Position.MessageId.DateTime,    pOut);
   pOut += SDBwriteString(pPosition->Position.MessageId.ComponentId, pOut);
   pOut += SDBwriteString(pPosition->ConsumerId,                     pOut);
   COLASSERT(pPosition->Position.SequenceId >= 0);
   pOut += SDBwriteNumber(pPosition->Position.SequenceId,            pOut);
   COLASSERT(pPosition->Position.QueueId >= 0);
   pOut += SDBwriteNumber(pPosition->Position.QueueId,               pOut);
   return pOut-pBuffer;
}

int SDBreadPosition(SDBposition* pPosition, const char* pBuffer, int BufSize) {
   COL_FUNCTION(SDBreadPosition);
   const char* pIn = pBuffer;
   int Amount = 0;
   do {
      Amount = SDBreadNumber(&pPosition->Type,                           pIn, BufSize); if (Amount < 0) { break; } pIn += Amount; BufSize -= Amount;
      Amount = SDBreadNumber(&pPosition->Position.MessageId.Index,       pIn, BufSize); if (Amount < 0) { break; } pIn += Amount; BufSize -= Amount;
      Amount = SDBreadNumber(&pPosition->Position.MessageId.DateTime,    pIn, BufSize); if (Amount < 0) { break; } pIn += Amount; BufSize -= Amount;
      Amount = SDBreadString(&pPosition->Position.MessageId.ComponentId, pIn, BufSize); if (Amount < 0) { break; } pIn += Amount; BufSize -= Amount;
      Amount = SDBreadString(&pPosition->ConsumerId,                     pIn, BufSize); if (Amount < 0) { break; } pIn += Amount; BufSize -= Amount;
      if (BufSize > 0) {
         Amount = SDBreadNumber(&pPosition->Position.SequenceId,         pIn, BufSize); if (Amount < 0) { break; } pIn += Amount; BufSize -= Amount;
         Amount = SDBreadNumber(&pPosition->Position.QueueId,            pIn, BufSize); if (Amount < 0) { break; } pIn += Amount; BufSize -= Amount;
      } else {
         COL_TRC("Old log. no Sequence Id and Queue Id. Default to 0");
         pPosition->Position.SequenceId = 0;
         pPosition->Position.QueueId = 0;
      }
      return pIn - pBuffer;
   } while (false);
   if (Amount < -1) {
      COL_ERR("Failed to read number: " << Amount);
   } else {
      COL_ERR("Not enough data for a complete position");
   }
   return Amount;
}

int SDBpositionSize(const SDBcontent* pPosition) {
   return SDBpositionSize((const SDBposition*)pPosition);
}

int SDBwritePosition(const SDBcontent* pPosition, char* pBuffer, int BufferSize) {
   return SDBwritePosition((const SDBposition*)pPosition, pBuffer, BufferSize);
}

int SDBreadPosition(SDBcontent* pPosition, const char* pBuffer, int BufferSize) {
   return SDBreadPosition((SDBposition*)pPosition, pBuffer, BufferSize);
}

COLostream& SDBprintPosition(const SDBcontent* pPosition, COLostream& Stream) {
   return Stream << *(const SDBposition*)pPosition;
}
