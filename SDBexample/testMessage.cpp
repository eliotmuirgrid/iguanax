// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testMessage
//
// Description:
//
// Implementation
//
// Author: John Qi
// Date:   Thursday 30 March 2023 - 05:52PM
// ---------------------------------------------------------------------------
#include "testMessage.h"

#include <UNIT/UNITapp.h>

#include <SDB/SDBmessage.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

static void testType(){
   COL_FUNCTION(testType);
   SDBmessage Msg("message_test", "data");
   UNIT_ASSERT_EQUALS(SDB_MESSAGE, Msg.Type);
}

static bool SDBTisContentMessageType(SDBtype Type) {
   SDBcontent Content;
   Content.Type = Type;
   return SDBisMessage(&Content);
}

static void testIsMessage(){
   COL_FUNCTION(testIsMessage);
   UNIT_ASSERT_EQUALS(false, SDBTisContentMessageType(SDB_MARKER));
   UNIT_ASSERT_EQUALS(false, SDBTisContentMessageType(SDB_POSMAP));
   UNIT_ASSERT_EQUALS(false, SDBTisContentMessageType(SDB_POSITION));
   UNIT_ASSERT_EQUALS(true,  SDBTisContentMessageType(SDB_MESSAGE));
   UNIT_ASSERT_EQUALS(true,  SDBTisContentMessageType(SDB_TRACE));
   UNIT_ASSERT_EQUALS(true,  SDBTisContentMessageType(SDB_INFO));
   UNIT_ASSERT_EQUALS(true,  SDBTisContentMessageType(SDB_DEBUG));
   UNIT_ASSERT_EQUALS(true,  SDBTisContentMessageType(SDB_WARNING));
   UNIT_ASSERT_EQUALS(true,  SDBTisContentMessageType(SDB_ERROR));
   UNIT_ASSERT_EQUALS(true,  SDBTisContentMessageType(SDB_SUCCESS));
   // update this test when new content types are introduced
   UNIT_ASSERT_EQUALS(12, SDB_EOT);
}

static void testSimple(){
   COL_FUNCTION(testSimple);
   SDBmessage Msg("message_test", "data");
   COLstring Buffer;
   Buffer.setCapacity(SDBmessageSize(&Msg));
   int Written = SDBwriteMessage(&Msg, Buffer.get_buffer(), Buffer.capacity());
   Buffer.setSize(Written);

   SDBmessage MsgIn;
   int Read = SDBreadMessage(&MsgIn, Buffer.get_buffer(), Buffer.size());
   COL_VAR4(Read, Written, SDBmessageSize(&MsgIn), SDBmessageSize(&Msg));
   COL_VAR2(MsgIn, Msg);
   UNIT_ASSERT_EQUALS(Read, Written);
   UNIT_ASSERT_EQUALS(MsgIn, Msg);
}

void testMessage(UNITapp* pApp){
   COL_FUNCTION(testMessage);
   pApp->add("message/type", &testType);
   pApp->add("message/ismessage", &testIsMessage);
   pApp->add("message/readwrite", &testSimple);
}
