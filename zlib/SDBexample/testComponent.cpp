// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testComponent
//
// Description:
//
// Implementation
//
// Author: John Q
// Date:   Fri 12 Sep 2025 13:37:47 EDT
// ---------------------------------------------------------------------------
#include <UNIT/UNITapp.h>

#include <SDB/SDBcomponent.h>
#include <SDB/SDBenvelope.h>
#include <SDB/SDBmessage.h>
#include <SDB/SDBpositionMap.h>

#include <FIL/FILutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

static void testComponentInitPosLastIds() {
   COL_FUNCTION(testComponentInitPosLastIds);
   COLstring TestFile = "PosLastIds.init.component.test";
   if (FILfileExists(TestFile)) { FILremove(TestFile); }

   COL_TRC("log file doesn't exist");
   {
      SDBcomponentStatus Status;
      bool Success = SDBfindPositionMapLastDataAndLogId("doesn't exist", "", true, Status);
      UNIT_ASSERT(!Success);
      UNIT_ASSERT_EQUALS(SDBid(),         Status.LastLogId);
      UNIT_ASSERT_EQUALS(SDBid(),         Status.LastDataId);
      UNIT_ASSERT_EQUALS(0,               Status.PositionMap.size());
      UNIT_ASSERT_EQUALS(SDB_INVALID_QID, Status.BOQqueueId);
   }

   COL_TRC("empty log file");
   FILwriteFile(TestFile, "");
   {
      SDBcomponentStatus Status;
      bool Success = SDBfindPositionMapLastDataAndLogId(TestFile, "", true, Status);
      UNIT_ASSERT(!Success);
      UNIT_ASSERT_EQUALS(SDBid(),         Status.LastLogId);
      UNIT_ASSERT_EQUALS(SDBid(),         Status.LastDataId);
      UNIT_ASSERT_EQUALS(0,               Status.PositionMap.size());
      UNIT_ASSERT_EQUALS(SDB_INVALID_QID, Status.BOQqueueId);
   }

   COLstring ComponentId = "Component_1";
   COLuint64 DateTime = SDBtime();
   COLuint64 Index = 1;

   COL_TRC("info qid 0");
   SDBmessage Message1;
   Message1.Type = SDB_TRACE;
   Message1.Tags = "#info";
   Message1.Data = "foo";
   Message1.Id = SDBid(SDBmessageId(ComponentId, DateTime, Index++), 1, 0);
   {
      COLstring Buffer;
      SDBserializeContent((SDBcontent*)&Message1, &Buffer);
      FILappendFile(TestFile, Buffer);
      SDBcomponentStatus Status;
      bool Success = SDBfindPositionMapLastDataAndLogId(TestFile, "", true, Status);
      UNIT_ASSERT(!Success);
      UNIT_ASSERT_EQUALS(Message1.Id,                                   Status.LastLogId);
      // IX-4300 : fast comp init disable for now
      // UNIT_ASSERT_EQUALS(SDBid(SDBmessageId(ComponentId, 0, 0), 0, 0),  Status.LastDataId);
      UNIT_ASSERT_EQUALS(SDBid(),                                       Status.LastDataId);
      UNIT_ASSERT_EQUALS(0,                                             Status.PositionMap.size());
      UNIT_ASSERT_EQUALS(SDB_INVALID_QID,                               Status.BOQqueueId);
   }

   COL_TRC("info qid 0 | info qid 1");
   SDBmessage Message2;
   Message2.Type = SDB_TRACE;
   Message2.Tags = "#info";
   Message2.Data = "bar";
   Message2.Id = SDBid(SDBmessageId(ComponentId, DateTime, Index++), 2, 1);
   {
      COLstring Buffer;
      SDBserializeContent((SDBcontent*)&Message2, &Buffer);
      FILappendFile(TestFile, Buffer);
      SDBcomponentStatus Status;
      bool Success = SDBfindPositionMapLastDataAndLogId(TestFile, "", true, Status);
      UNIT_ASSERT(!Success);
      UNIT_ASSERT_EQUALS(Message2.Id,     Status.LastLogId);
      UNIT_ASSERT_EQUALS(SDBid(),         Status.LastDataId);
      UNIT_ASSERT_EQUALS(0,               Status.PositionMap.size());
      UNIT_ASSERT_EQUALS(SDB_INVALID_QID, Status.BOQqueueId);
   }

   COL_TRC("info | info | data");
   SDBmessage Message3;
   Message3.Type = SDB_MESSAGE;
   Message3.Data = "HL7";
   Message3.Id = SDBid(SDBmessageId(ComponentId, DateTime, Index++), 3, 2);
   {
      COLstring Buffer;
      SDBserializeContent((SDBcontent*)&Message3, &Buffer);
      FILappendFile(TestFile, Buffer);
      SDBcomponentStatus Status;
      bool Success = SDBfindPositionMapLastDataAndLogId(TestFile, "", true, Status);
      UNIT_ASSERT(!Success);
      UNIT_ASSERT_EQUALS(Message3.Id,     Status.LastLogId);
      UNIT_ASSERT_EQUALS(Message3.Id,     Status.LastDataId);
      UNIT_ASSERT_EQUALS(0,               Status.PositionMap.size());
      UNIT_ASSERT_EQUALS(SDB_INVALID_QID, Status.BOQqueueId);
   }

   COL_TRC("info | info | data | info");
   SDBmessage Message4;
   Message4.Type = SDB_TRACE;
   Message4.Tags = "#info";
   Message4.Data = "moo";
   Message4.Id = SDBid(SDBmessageId(ComponentId, DateTime, Index++), 4, 2);
   {
      COLstring Buffer;
      SDBserializeContent((SDBcontent*)&Message4, &Buffer);
      FILappendFile(TestFile, Buffer);
      SDBcomponentStatus Status;
      bool Success = SDBfindPositionMapLastDataAndLogId(TestFile, "", true, Status);
      UNIT_ASSERT(!Success);
      UNIT_ASSERT_EQUALS(Message4.Id,     Status.LastLogId);
      UNIT_ASSERT_EQUALS(Message3.Id,     Status.LastDataId);
      UNIT_ASSERT_EQUALS(0,               Status.PositionMap.size());
      UNIT_ASSERT_EQUALS(SDB_INVALID_QID, Status.BOQqueueId);
   }

   COL_TRC("no consumers");
   {
      SDBcomponentStatus Status;
      bool Success = SDBfindPositionMapLastDataAndLogId(TestFile, "", false, Status);
      UNIT_ASSERT(Success);
   }

   COL_TRC("info | info | data | info | pos_map");
   SDBpositionMap PosMap1;
   PosMap1.PositionMap[ComponentId]["Consumer_1"] = Message3.Id;
   {
      COLstring Buffer;
      SDBserializeContent((SDBcontent*)&PosMap1, &Buffer);
      FILappendFile(TestFile, Buffer);
      SDBcomponentStatus Status;
      bool Success = SDBfindPositionMapLastDataAndLogId(TestFile, "", true, Status);
      UNIT_ASSERT(Success);
      UNIT_ASSERT_EQUALS(Message4.Id,     Status.LastLogId);
      UNIT_ASSERT_EQUALS(Message3.Id,     Status.LastDataId);
      UNIT_ASSERT_EQUALS(1,               Status.PositionMap.size());
      UNIT_ASSERT_EQUALS(SDB_INVALID_QID, Status.BOQqueueId);
   }

   COL_TRC("info | info | data | pos_map | info");
   SDBmessage Message5;
   Message5.Type = SDB_TRACE;
   Message5.Tags = "#info";
   Message5.Data = "car";
   Message5.Id = SDBid(SDBmessageId(ComponentId, DateTime, Index++), 5, 2);
   {
      COLstring Buffer;
      SDBserializeContent((SDBcontent*)&Message5, &Buffer);
      FILappendFile(TestFile, Buffer);
      SDBcomponentStatus Status;
      bool Success = SDBfindPositionMapLastDataAndLogId(TestFile, "", true, Status);
      UNIT_ASSERT(Success);
      UNIT_ASSERT_EQUALS(Message5.Id,     Status.LastLogId);
      UNIT_ASSERT_EQUALS(Message3.Id,     Status.LastDataId);
      UNIT_ASSERT_EQUALS(1,               Status.PositionMap.size());
      UNIT_ASSERT_EQUALS(SDB_INVALID_QID, Status.BOQqueueId);
   }

   COL_TRC("info | info | data | pos_map | info | data");
   SDBmessage Message6;
   Message6.Type = SDB_MESSAGE;
   Message6.Data = "X12";
   Message6.Id = SDBid(SDBmessageId(ComponentId, DateTime, Index++), 6, 3);
   {
      COLstring Buffer;
      SDBserializeContent((SDBcontent*)&Message6, &Buffer);
      FILappendFile(TestFile, Buffer);
      SDBcomponentStatus Status;
      bool Success = SDBfindPositionMapLastDataAndLogId(TestFile, "", true, Status);
      UNIT_ASSERT(Success);
      UNIT_ASSERT_EQUALS(Message6.Id,     Status.LastLogId);
      UNIT_ASSERT_EQUALS(Message6.Id,     Status.LastDataId);
      UNIT_ASSERT_EQUALS(1,               Status.PositionMap.size());
      UNIT_ASSERT_EQUALS(SDB_INVALID_QID, Status.BOQqueueId);
   }

   COL_TRC("info | info | data | pos_map | info | data | pos_map");
   SDBpositionMap PosMap2;
   PosMap2.PositionMap[ComponentId]["Consumer_1"] = Message3.Id;
   PosMap2.PositionMap[ComponentId]["Consumer_2"] = Message6.Id;
   {
      COLstring Buffer;
      SDBserializeContent((SDBcontent*)&PosMap2, &Buffer);
      FILappendFile(TestFile, Buffer);
      SDBcomponentStatus Status;
      bool Success = SDBfindPositionMapLastDataAndLogId(TestFile, "", true, Status);
      UNIT_ASSERT(Success);
      UNIT_ASSERT_EQUALS(Message6.Id,     Status.LastLogId);
      UNIT_ASSERT_EQUALS(Message6.Id,     Status.LastDataId);
      UNIT_ASSERT_EQUALS(2,               Status.PositionMap.size());
      UNIT_ASSERT_EQUALS(SDB_INVALID_QID, Status.BOQqueueId);
   }

   // cleanup
   FILremove(TestFile);
}

static void testComponentInitBOQqueueId() {
   COL_FUNCTION(testComponentInitBOQqueueId);
   COLstring TestFile = "BOQqueueId.init.component.test";
   if (FILfileExists(TestFile)) { FILremove(TestFile); }

   COL_TRC("log file doesn't exist");
   {
      COLuint64 BOQqueueId = SDB_INVALID_QID;
      bool Success = SDBgetBOQqueueId("doesn't exist", "", &BOQqueueId);
      UNIT_ASSERT(!Success);
      UNIT_ASSERT_EQUALS(SDB_INVALID_QID, BOQqueueId);
   }

   COL_TRC("empty log file");
   FILwriteFile(TestFile, "");
   {
      COLuint64 BOQqueueId = SDB_INVALID_QID;
      bool Success = SDBgetBOQqueueId(TestFile, "", &BOQqueueId);
      UNIT_ASSERT(!Success);
      UNIT_ASSERT_EQUALS(SDB_INVALID_QID, BOQqueueId);
   }

   COLstring ComponentId = "Component_1";
   COLuint64 DateTime = SDBtime();
   COLuint64 Index = 1;

   COL_TRC("pos_map");
   {
      SDBpositionMap PosMap;
      PosMap.PositionMap[ComponentId]["Consumer_1"] = SDBid(SDBmessageId("Consumer_1"), 23, 13);
      COLstring Buffer;
      SDBserializeContent((SDBcontent*)&PosMap, &Buffer);
      FILappendFile(TestFile, Buffer);
      COLuint64 BOQqueueId = SDB_INVALID_QID;
      bool Success = SDBgetBOQqueueId(TestFile, "", &BOQqueueId);
      UNIT_ASSERT(!Success);
      UNIT_ASSERT_EQUALS(SDB_INVALID_QID, BOQqueueId);
   }

   COL_TRC("pos_map | info v1");
   {
      SDBmessage Message(ComponentId, "far", DateTime, Index++);
      Message.Type = SDB_TRACE;
      Message.Tags = "#info";
      COLstring Buffer;
      SDBserializeContent((SDBcontent*)&Message, &Buffer);
      FILappendFile(TestFile, Buffer);
      COLuint64 BOQqueueId = SDB_INVALID_QID;
      bool Success = SDBgetBOQqueueId(TestFile, "", &BOQqueueId);
      UNIT_ASSERT(Success);
      UNIT_ASSERT_EQUALS(0, BOQqueueId);
   }

   COL_TRC("info v2 qid=0");
   {
      SDBmessage Message;
      Message.Type = SDB_TRACE;
      Message.Tags = "#info";
      Message.Data = "tar";
      Message.Id = SDBid(SDBmessageId(ComponentId, DateTime, Index++), 5, 0);
      COLstring Buffer;
      SDBserializeContent((SDBcontent*)&Message, &Buffer);
      FILwriteFile(TestFile, Buffer);
      COLuint64 BOQqueueId = SDB_INVALID_QID;
      bool Success = SDBgetBOQqueueId(TestFile, "", &BOQqueueId);
      UNIT_ASSERT(Success);
      UNIT_ASSERT_EQUALS(0, BOQqueueId);
   }

   COL_TRC("info v2 qid>0");
   {
      SDBmessage Message;
      Message.Type = SDB_TRACE;
      Message.Tags = "#info";
      Message.Data = "tar";
      Message.Id = SDBid(SDBmessageId(ComponentId, DateTime, Index++), 13, 24);
      COLstring Buffer;
      SDBserializeContent((SDBcontent*)&Message, &Buffer);
      FILwriteFile(TestFile, Buffer);
      COLuint64 BOQqueueId = SDB_INVALID_QID;
      bool Success = SDBgetBOQqueueId(TestFile, "", &BOQqueueId);
      UNIT_ASSERT(Success);
      UNIT_ASSERT_EQUALS(24, BOQqueueId);
   }

   COL_TRC("data v2 qid=1");
   {
      SDBmessage Message;
      Message.Type = SDB_MESSAGE;
      Message.Data = "boo";
      Message.Id = SDBid(SDBmessageId(ComponentId, DateTime, Index++), 8, 1);
      COLstring Buffer;
      SDBserializeContent((SDBcontent*)&Message, &Buffer);
      FILwriteFile(TestFile, Buffer);
      COLuint64 BOQqueueId = SDB_INVALID_QID;
      bool Success = SDBgetBOQqueueId(TestFile, "", &BOQqueueId);
      UNIT_ASSERT(Success);
      UNIT_ASSERT_EQUALS(0, BOQqueueId);
   }

   COL_TRC("data v2 qid>1");
   {
      SDBmessage Message;
      Message.Type = SDB_MESSAGE;
      Message.Data = "boo";
      Message.Id = SDBid(SDBmessageId(ComponentId, DateTime, Index++), 13, 24);
      COLstring Buffer;
      SDBserializeContent((SDBcontent*)&Message, &Buffer);
      FILwriteFile(TestFile, Buffer);
      COLuint64 BOQqueueId = SDB_INVALID_QID;
      bool Success = SDBgetBOQqueueId(TestFile, "", &BOQqueueId);
      UNIT_ASSERT(Success);
      UNIT_ASSERT_EQUALS(23, BOQqueueId);
   }

   // cleanup
   FILremove(TestFile);
}

void testComponent(UNITapp* pApp){
   COL_FUNCTION(testComponent);
   pApp->add("component/init/PosLastIds", &testComponentInitPosLastIds);
   pApp->add("component/init/BOQqueueId", &testComponentInitBOQqueueId);
}
