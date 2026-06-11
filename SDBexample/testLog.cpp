// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testLog
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Mon 13 Feb 2023 10:01:00 EST
// ---------------------------------------------------------------------------
#include "testDir.h"
#include <UNIT/UNITapp.h>

#include <SDB/SDBmessage.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

static void testReadWrite(){
   COL_FUNCTION(testReadWrite);
   SDBmessage M1("component99", "The quick brown fox");

   COLstring Buffer;
   Buffer.setCapacity(SDBmessageSize(&M1));
   int Written = SDBwriteMessage(&M1, Buffer.get_buffer(), Buffer.capacity());
   Buffer.setSize(Written);

   SDBmessage M2;
   int Read = SDBreadMessage(&M2, Buffer.get_buffer(), Buffer.size());

   COL_VAR4(Read, Written, SDBmessageSize(&M1), SDBmessageSize(&M2));

   COL_VAR2(M1, M2);

   UNIT_ASSERT_EQUALS(Read, Written);
   UNIT_ASSERT_EQUALS(Read, SDBmessageSize(&M1));
   // SDBreadMessage() auto-adds #message to M2.Tags
   int TagSize = sizeof("#message") - 1;
   UNIT_ASSERT_EQUALS(Read, SDBmessageSize(&M2) - TagSize);
   UNIT_ASSERT_EQUALS(M1, M2);
}

static void testBufferError(){
   COL_FUNCTION(testBufferError);
   char Buffer[4] = "42";
   SDBmessage M1("42", "The lazy orange cat");
   UNIT_ASSERT_EQUALS(-1, SDBwriteMessage(&M1, Buffer, sizeof(Buffer)));
   UNIT_ASSERT_EQUALS(-1, SDBreadMessage (&M1, Buffer, sizeof(Buffer)));
}

void testLog(UNITapp* pApp){
   COL_FUNCTION(testLog);
   pApp->add("log/readwrite", &testReadWrite);
   pApp->add("log/bufferError", &testBufferError);
}
