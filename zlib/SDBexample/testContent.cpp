// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testContent
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Mon 13 Feb 2023 10:01:00 EST
// ---------------------------------------------------------------------------
#include "testContent.h"

#include <UNIT/UNITapp.h>

#include <SDB/SDBcontent.h>
#include <SDB/SDBmessage.h>
#include <SDB/SDBmagicMarker.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

static void testType(){
   COL_FUNCTION(testType);
   SDBmessage Message;
   Message.Type = SDB_MESSAGE;

   COLuint64 Type = ((SDBcontent*)&Message)->Type;
   COL_VAR2(Type, Message.Type);

   UNIT_ASSERT_EQUALS(Type, Message.Type);
}

static void testSizeHelper(SDBcontent* pContent, int ExpectedSize){
   COL_FUNCTION(testSizeHelper);
   int Size = SDBcontentSize(pContent);
   COL_VAR2(Size, ExpectedSize);
   UNIT_ASSERT_EQUALS(Size, ExpectedSize);
}

static void testSwitchSize(){
   COL_FUNCTION(testSwitchSize);
   SDBmessage Message("content_switch", "The lazy orange cat");
   testSizeHelper((SDBcontent*)&Message, SDBmessageSize(&Message));

   SDBmagicMarker Marker;
   testSizeHelper((SDBcontent*)&Marker, SDBmagicMarkerSize(&Marker));
}

static void testReadWriteHelper(SDBcontent* pSource, SDBcontent* pDestination){
   COL_FUNCTION(testReadWriteHelper);
   COLstring Buffer;
   Buffer.setCapacity(SDBcontentSize(pSource));
   int Amount = SDBwriteContent(pSource, Buffer.get_buffer(), Buffer.capacity());
   Buffer.setSize(Amount);
   Amount = SDBreadContent(pDestination, Buffer.get_buffer(), Buffer.size());

}

static void testSwitchReadWrite(){
   COL_FUNCTION(testSwitchReadWrite);

   SDBmessage Source1("content_switch", "The lazy orange cat"), Dest1;
   testReadWriteHelper((SDBcontent*)&Source1 ,(SDBcontent*)&Dest1);
   // SDBreadContent() auto-adds #message. Mod Source1 for test
   Source1.Tags = "#message";
   UNIT_ASSERT_EQUALS(Source1, Dest1);

   SDBmagicMarker Source2, Dest2;
   testReadWriteHelper((SDBcontent*)&Source2 ,(SDBcontent*)&Dest2);
   UNIT_ASSERT_EQUALS(Source2,  Dest2);
}

static void testBufferErrorHelper(SDBcontent* pContent, int ReadErrorCode){
   COL_FUNCTION(testBufferErrorHelper);
   char Buffer[4] = "42";
   UNIT_ASSERT_EQUALS(-1, SDBwriteContent(pContent, Buffer, sizeof(Buffer)));
   UNIT_ASSERT_EQUALS(ReadErrorCode, SDBreadContent(pContent, Buffer, sizeof(Buffer)));
}

static void testBufferError(){
   COL_FUNCTION(testBufferError);
   SDBmessage Message("content_switch", "The lazy orange cat");
   testBufferErrorHelper((SDBcontent*)&Message, -1);

   SDBmagicMarker Marker;
   testBufferErrorHelper((SDBcontent*)&Marker, -1);
}

static void testCreateDestroy(){
   COL_FUNCTION(testCreateDestroy);
   SDBcontent* pMagic = SDBcontentCreate(SDB_MARKER);
   SDBcontentDestroy(pMagic);
   SDBcontent* pMessage = SDBcontentCreate(SDB_MESSAGE);
   SDBcontentDestroy(pMessage);
}

// test operator<<() for all content types
static void testPrint() {
   COL_FUNCTION(testPrint);
   for (int i = 1; i < SDB_EOT; ++i) {
      COL_VAR(i);
      SDBcontent* pContent = SDBcontentCreate(i);
      COL_VAR(*pContent);  // calls operator<<()
      SDBcontentDestroy(pContent);
   }
}

void testContent(UNITapp* pApp){
   COL_FUNCTION(testContent);
   pApp->add("content/type", &testType);
   pApp->add("content/size",       &testSwitchSize);
   pApp->add("content/readwrite",  &testSwitchReadWrite);
   pApp->add("content/bufferError", &testBufferError);
   pApp->add("content/createdestroy", &testCreateDestroy);
   pApp->add("content/print", &testPrint);
}
