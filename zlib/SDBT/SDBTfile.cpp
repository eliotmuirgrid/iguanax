// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBTfile
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Monday 27 February 2023 - 10:15AM
// ---------------------------------------------------------------------------

#include <SDBT/SDBTfile.h>
#include <SDBT/SDBTmessage.h>

#include <SDB/SDBfile.h>
#include <SDB/SDBmessage.h>
#include <SDB/SDBenvelope.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void SDBTmakeTestMessageSet(const COLstring& LogFile, COLuint64 MessageIdStart, int Count){
   COL_FUNCTION(SDBTmakeTestMessageSet);
   int File = SDBreplaceLogFile(LogFile);
   COLstring Buffer;
   COLuint64 End = MessageIdStart+Count;
   COLstring BigBuffer;
   COLstring Space(50000, 'A');
   COLuint64 Time = time(NULL)*1000;
   for (COLuint64 i = MessageIdStart; i < End; i++){
      SDBmessage Message("Dummy", "This is message number " + COLintToString(i) + Space, Time, i);
      if (SDBmessageSize(&Message) >= Buffer.capacity()){
         Buffer.setCapacity(SDBmessageSize(&Message)+1);
         COL_TRC("Set capacity to " << Buffer.capacity());
      }
      int AmountWritten = SDBwriteMessage(&Message, Buffer.get_buffer(), Buffer.capacity());
      //COL_VAR2(AmountWritten, Buffer.capacity());
      Buffer.setSize(AmountWritten);
      SDBenvelopeWrite(&BigBuffer, Buffer);
      if (BigBuffer.size() > 1000000){
         SDBwriteLogFile(File, BigBuffer.get_buffer(), BigBuffer.size());
         BigBuffer.clear();
      }
   }
   COL_VAR(BigBuffer.size());
   SDBwriteLogFile(File, BigBuffer.get_buffer(), BigBuffer.size());
   SDBcloseLogFile(File);
}

void SDBTmakeTestMessageFile(const COLstring& LogFile, COLuint64 MessageIdStart, int Count, int Size, time_t Date) {
   COL_FUNCTION(SDBTmakeTestMessageFile);
   int File = SDBreplaceLogFile(LogFile);
   COLstring Data(Size, '-');
   COLstring Buffer;
   int BufferSize = 16777216;
   Buffer.setCapacity(BufferSize);
   for (COLuint64 i = MessageIdStart; i < MessageIdStart + Count; i++) {
      COLstring MsgBuf = SDBTcreateSerializedTestMessage("Dummy", i, Size, Date);
      SDBenvelopeWrite(&Buffer, MsgBuf);
      if (Buffer.size() > 16252928) {
         SDBwriteLogFile(File, Buffer.get_buffer(), Buffer.size());
         Buffer.clear();
      }
   }
   SDBwriteLogFile(File, Buffer.get_buffer(), Buffer.size());
   SDBcloseLogFile(File);
}
