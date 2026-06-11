// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testPosition
//
// Description:
//
// Implementation
//
// Author: John Qi
// Date:   Tuesday 14 March 2023 - 03:14PM
// ---------------------------------------------------------------------------
#include "testPosition.h"

#include <UNIT/UNITapp.h>

#include <SDB/SDBposition.h>
#include <SDB/SDBmessageId.h>
#include <SDB/SDBepochTime.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

static void testType(){
   COL_FUNCTION(testType);
   SDBid Id;
   Id.MessageId = SDBmessageId("queue_id");
   SDBposition Pos("position_test", Id);
   UNIT_ASSERT_EQUALS(SDB_POSITION, Pos.Type);
}

static void testSimple(){
   COL_FUNCTION(testSimple);
   SDBid Id;
   Id.MessageId = SDBmessageId("queue_id");
   SDBposition PosOut("position_test", Id);
   COLstring Buffer;
   Buffer.setCapacity(SDBpositionSize(&PosOut));
   int Written = SDBwritePosition(&PosOut, Buffer.get_buffer(), Buffer.capacity());
   Buffer.setSize(Written);

   SDBposition PosIn;
   int Read = SDBreadPosition(&PosIn, Buffer.get_buffer(), Buffer.size());
   COL_VAR4(Read, Written, SDBpositionSize(&PosOut), SDBpositionSize(&PosIn));
   COL_VAR2(PosOut, PosIn);
   UNIT_ASSERT_EQUALS(Read, Written);
   UNIT_ASSERT_EQUALS(PosOut, PosIn);
}

static void testBufferError(){
   COL_FUNCTION(testBufferError);
   SDBid Id;
   Id.MessageId = SDBmessageId("queue_id");
   char Buffer[4] = "42";
   SDBposition Pos("position_test", Id);
   UNIT_ASSERT_EQUALS(-1, SDBwritePosition(&Pos, Buffer, sizeof(Buffer)));
   UNIT_ASSERT_EQUALS(-1, SDBreadPosition (&Pos, Buffer, sizeof(Buffer)));
}

void testPosition(UNITapp* pApp){
   COL_FUNCTION(testPosition);
   pApp->add("position/type", &testType);
   pApp->add("position/readwrite", &testSimple);
   pApp->add("position/bufferError", &testBufferError);
}
