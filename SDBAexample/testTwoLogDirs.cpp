// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testTwoLogDirs
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Mon 13 Feb 2023 10:01:00 EST
// ---------------------------------------------------------------------------
#include "testTwoLogDirs.h"

#include <UNIT/UNITapp.h>

#include <SDBU/SDBUlogFile.h>
#include <SDB/SDBmessage.h>
#include <SDB/SDBcontentDelete.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

static void testMerge() {
   COL_FUNCTION(testMerge);
   COLuint64 DateTime = SDBtime();
   COLlist<SDBcontent*> List1;
   COLlist<SDBcontent*> List2;
   COLlist<SDBcontent*> List;
   COLuint64 LastMsgId;

   COL_TRC("merge full list from both log dirs ascending order");
   for (int i = 1; i <= 100; ++i) {
      List1.push_back((SDBcontent*) new SDBmessage("Component_1", "dir1", DateTime, i));
      List2.push_back((SDBcontent*) new SDBmessage("Component_1", "dir2", DateTime, i));
   }
   SDBUmergeFileToList(List1, List2, &List);
   UNIT_ASSERT_EQUALS(100, List.size());
   LastMsgId = 1;
   for (auto pMsg : List) {
      UNIT_ASSERT_EQUALS(LastMsgId++, ((SDBmessage*)pMsg)->Id.MessageId.Index);
   }
   SDBcontentDelete(&List);
   List1.clear();
   List2.clear();
   List.clear();

   COL_TRC("merge even from one dir odd from the other ascending order");
   for (int i = 1; i <= 100; ++i) {
      if (i % 2) {
         List1.push_back((SDBcontent*) new SDBmessage("Component_1", "dir1", DateTime, i));
      } else {
         List2.push_back((SDBcontent*) new SDBmessage("Component_1", "dir2", DateTime, i));
      }
   }
   COL_VAR2(List1.size(), List2.size());
   SDBUmergeFileToList(List1, List2, &List);
   UNIT_ASSERT_EQUALS(100, List.size());
   LastMsgId = 1;
   for (auto pMsg : List) {
      UNIT_ASSERT_EQUALS(LastMsgId++, ((SDBmessage*)pMsg)->Id.MessageId.Index);
   }
   SDBcontentDelete(&List);
   List1.clear();
   List2.clear();
   List.clear();

   COL_TRC("merge half from one dir half from the other ascending order");
   for (int i = 1; i <= 100; ++i) {
      if (i > 50) {
         List1.push_back((SDBcontent*) new SDBmessage("Component_1", "dir1", DateTime, i));
      } else {
         List2.push_back((SDBcontent*) new SDBmessage("Component_1", "dir2", DateTime, i));
      }
   }
   COL_VAR2(List1.size(), List2.size());
   SDBUmergeFileToList(List1, List2, &List);
   UNIT_ASSERT_EQUALS(100, List.size());
   LastMsgId = 1;
   for (auto pMsg : List) {
      UNIT_ASSERT_EQUALS(LastMsgId++, ((SDBmessage*)pMsg)->Id.MessageId.Index);
   }
   SDBcontentDelete(&List);
   List1.clear();
   List2.clear();
   List.clear();
}

void testTwoLogDirs(UNITapp* pApp){
   COL_FUNCTION(testTwoLogDirs);
   pApp->add("two/log/dirs/merge", &testMerge);
}
