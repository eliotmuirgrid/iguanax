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

#include <SDBT/SDBTpositionMap.h>
#include <SDB/SDBpositionMap.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

static void testType(){
   COL_FUNCTION(testType);
   COLhashmap<COLstring, COLhashmap<COLstring, SDBid>> Map;
   SDBpositionMap PosMap(Map);
   UNIT_ASSERT_EQUALS(SDB_POSMAP, PosMap.Type);
}

static void testSimple(){
   COL_FUNCTION(testSimple);

   SDBpositionMap PosMapOut = SDBTcreateTestPositionMap(4);
   COL_VAR(PosMapOut);
   COLstring Buffer;
   Buffer.setCapacity(SDBpositionMapSize(&PosMapOut));
   int Written = SDBwritePositionMap(&PosMapOut, Buffer.get_buffer(), Buffer.capacity());
   Buffer.setSize(Written);

   SDBpositionMap PosMapIn;
   int Read = SDBreadPositionMap(&PosMapIn, Buffer.get_buffer(), Buffer.size());
   COL_VAR4(Read, Written, SDBpositionMapSize(&PosMapOut), SDBpositionMapSize(&PosMapIn));
   COL_VAR2(PosMapOut, PosMapIn);
   UNIT_ASSERT_EQUALS(Read, Written);
   UNIT_ASSERT_EQUALS(PosMapOut, PosMapIn);
   UNIT_ASSERT_EQUALS(PosMapOut.PositionMap.size(), PosMapIn.PositionMap.size());
   for (auto i = PosMapIn.PositionMap.cbegin(); i != PosMapIn.PositionMap.cend(); i++) {
      UNIT_ASSERT_EQUALS(1, PosMapOut.PositionMap.count(i.key()));
      for (auto j = i.value().cbegin(); j != i.value().cend(); j++) {
         UNIT_ASSERT_EQUALS(PosMapIn.PositionMap[i.key()][j.key()], j.value());
      }
   }
}

static void testBufferError(){
   COL_FUNCTION(testBufferError);
   char Buffer[4] = "42";
   COLhashmap<COLstring, COLhashmap<COLstring, SDBid>> Map;
   SDBpositionMap PosMap(Map);
   UNIT_ASSERT_EQUALS(-2, SDBreadPositionMap (&PosMap, Buffer, sizeof(Buffer)));
   UNIT_ASSERT_EQUALS( 2, SDBwritePositionMap(&PosMap, Buffer, sizeof(Buffer)));
   UNIT_ASSERT_EQUALS(-1, SDBwritePositionMap(&PosMap, Buffer, 1));
}

static void testWrite(){
   COL_TRC("test case where SDBqueuePositionSize() used to return 128 instead of 127");
   COLhashmap<COLstring, SDBid> Map;
   Map["GGASTROtol_DVAloxaJcpa73r"] = SDBid(SDBmessageId("test_ymPjQCXT5g9Ins"), 0, 0);
   Map["test_DQw3CXUPQK9E8j"]       = SDBid(SDBmessageId("test_ymPjQCXT5g9Ins"), 0, 0);
   int QueuePositionSize = SDBqueuePositionSize("test_ymPjQCXT5g9Ins", Map);
   UNIT_ASSERT_EQUALS(127, QueuePositionSize);

   COL_TRC("test case where SDBpositionSize() used to return 128 instead of 127");
   SDBid Position(SDBmessageId("test483903yh59r8gvhu439rio720yhf87234hf8434e3ffgfo8347fgfucrigeg45r3fh89r345_ymPjQCXT5g9Ins"), 0, 0);
   UNIT_ASSERT_EQUALS(127, SDBpositionSize("GGASTROtol_DVAloxaJcpa73r", Position));

   COL_TRC("test case to make sure we can read what we wrote");
   SDBpositionMap PosMap;
   PosMap.PositionMap["test_ymPjQCXT5g9Ins"]["GGASTROtol_DVAloxaJcpa73r"] = SDBid(SDBmessageId("test_ymPjQCXT5g9Ins"), 0, 0);
   PosMap.PositionMap["test_ymPjQCXT5g9Ins"]["test_DQw3CXUPQK9E8j"]       = SDBid(SDBmessageId("test_ymPjQCXT5g9Ins"), 0, 0);
   COLstring Buffer;
   Buffer.setCapacity(SDBpositionMapSize(&PosMap));
   int Written = SDBwritePositionMap(&PosMap, Buffer.get_buffer(), Buffer.capacity());
   Buffer.setSize(Written);

   SDBpositionMap PosMapIn;
   int Read = SDBreadPositionMap(&PosMapIn, Buffer.get_buffer(), Buffer.size());
   UNIT_ASSERT_EQUALS(Read, Written);
   UNIT_ASSERT_EQUALS(PosMap, PosMapIn);
}


void testPositionMap(UNITapp* pApp){
   COL_FUNCTION(testPositionMap);
   pApp->add("positionmap/type", &testType);
   pApp->add("positionmap/readwrite", &testSimple);
   pApp->add("positionmap/bufferError", &testBufferError);
   pApp->add("posmap/write", &testWrite);
}
