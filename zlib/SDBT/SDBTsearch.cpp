// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBTsearch
//
// Description:
//
// Implementation
//
// Author: John Qi
// Date:   Monday 27 February 2023 - 03:25PM
// ---------------------------------------------------------------------------

#include <SDBT/SDBTsearch.h>

#include <SDB/SDBcontent.h>
#include <SDB/SDBfile.h>
#include <SDB/SDBmessage.h>
#include <SDB/SDBnumber.h>

#include <SCK/SCKloop.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

static int SDBTfindMessage(COLstring& Buffer, int TargetId, SDBmessage* pResult) {
   COL_FUNCTION(SDBTfindMessage);
   const char* pIn = Buffer.data();
   int BufferSize = Buffer.size();
   do {
      COLuint64 MessageSize = 0;
      int Amount = SDBreadNumber(&MessageSize, pIn, BufferSize);
      if (Amount == -1) { break; }
      BufferSize -= Amount;
      pIn += Amount; // advance to beginning of message
      COLint64 Type = SDBcontentType(pIn, BufferSize);
      SDBmessage Message;
      Amount = SDBreadMessage(&Message, pIn, BufferSize);
      if (Amount == -1) {
         // Undo 'advance to beginning of message".
         pIn -= SDBnumberSize(MessageSize);
         break;
      }
      BufferSize -= MessageSize;
      pIn += MessageSize;  // advance pass message
      COL_VAR(Message);
      if (Message.Id.MessageId.Index == TargetId) {
         COL_TRC("Found Message!!!");
         *pResult = Message;
      }
   } while (BufferSize > 0);
   COL_TRC("insufficient data in buffer.");
   Buffer.shift(pIn-Buffer.data());
   COL_VAR(Buffer.size());
   return -1;  // more data
}

static void SDBTsearchCallback(COLstring Chunk, COLuint64 Id, int FD, SCKloop* pLoop) {
   COL_FUNCTION(SDBTsearchCallback);
   static SDBmessage Result;
   if (Chunk.size() == 0){
      COL_TRC("End of file.");
      if (Result.Id.MessageId.Index == 0) {
         COL_ERR("Message Not Found");
         pLoop->shutdown();
         return;
      }
      // COLcout << "Found Message " << Result << newline;
      if (Result.RelatedId.Index == 0) {
         pLoop->shutdown();
      } else {
         // COLcout << "Search for related message " << Result.RelatedMessageId << newline;
         SDBseekLogFile(FD, 0);
         SDBstreamFile(pLoop, FD, COLnewClosure1(&SDBTsearchCallback, Result.RelatedId.Index, FD, pLoop));
      }
      return;
   }
   static COLstring Buffer;
   Buffer.append(Chunk);
   SDBTfindMessage(Buffer, Id, &Result);
}

// Toy algo for searching a message id and its related messages in a log file
// we search through the entire log file multiple times because of SDBstreamFile
void SDBTseachMessageId(COLint64 Id, int FD, SCKloop* pLoop) {
   COL_FUNCTION(SDBTseachMessageId);
   SDBstreamFile(pLoop, FD, COLnewClosure1(&SDBTsearchCallback, (COLuint64)6543, FD, pLoop));
}
