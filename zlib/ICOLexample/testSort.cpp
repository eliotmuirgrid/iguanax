//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: testICOLmap.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   30/01/25 10:27 AM
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <ICOL/ICOLmap.h>
#include <UNIT/UNITapp.h>
COL_LOG_MODULE;

static void testMultipleCases() {
   COL_FUNCTION(testMultipleCases);

   COLmap<COLstring, ICOLinfo> TestMap;
   TestMap.add("ColumnA", ICOLinfo(100, 2, ""));
   TestMap.add("ColumnB", ICOLinfo(150, -1, ""));
   TestMap.add("ColumnC", ICOLinfo(200, 1, ""));
   TestMap.add("ColumnD", ICOLinfo(300, -1, ""));
   TestMap.add("ColumnE", ICOLinfo(250, 2, ""));
   TestMap.add("ColumnF", ICOLinfo(180, 1, ""));

   COLvar Result;
   ICOLmapSort(TestMap, &Result);

   UNIT_ASSERT_EQUALS(6, Result.size());

   // Verify positive priorities are sorted correctly
   UNIT_ASSERT_EQUALS("ColumnC", Result[0]["name"].asString());
   UNIT_ASSERT_EQUALS(200, Result[0]["width"].asInt());
   UNIT_ASSERT_EQUALS("ColumnF", Result[1]["name"].asString());
   UNIT_ASSERT_EQUALS(180, Result[1]["width"].asInt());
   UNIT_ASSERT_EQUALS("ColumnA", Result[2]["name"].asString());
   UNIT_ASSERT_EQUALS(100, Result[2]["width"].asInt());
   UNIT_ASSERT_EQUALS("ColumnE", Result[3]["name"].asString());
   UNIT_ASSERT_EQUALS(250, Result[3]["width"].asInt());

   // Verify -1 priorities are at the end
   UNIT_ASSERT_EQUALS("ColumnB", Result[4]["name"].asString());
   UNIT_ASSERT_EQUALS(150, Result[4]["width"].asInt());
   UNIT_ASSERT_EQUALS("ColumnD", Result[5]["name"].asString());
   UNIT_ASSERT_EQUALS(300, Result[5]["width"].asInt());
}

static void testNegativePrio() {
   COL_FUNCTION(testNegativePrio);
   COLmap<COLstring, ICOLinfo> TestMap;
   TestMap.add("ColumnA", ICOLinfo(100, 2, ""));
   TestMap.add("ColumnB", ICOLinfo(150, -1, ""));
   TestMap.add("ColumnC", ICOLinfo(200, 1, ""));

   COLvar Result;
   ICOLmapSort(TestMap, &Result);

   UNIT_ASSERT_EQUALS(3, Result.size());
   UNIT_ASSERT_EQUALS("ColumnC", Result[0]["name"].asString());
   UNIT_ASSERT_EQUALS(200, Result[0]["width"].asInt());
   UNIT_ASSERT_EQUALS("ColumnA", Result[1]["name"].asString());
   UNIT_ASSERT_EQUALS(100, Result[1]["width"].asInt());
   UNIT_ASSERT_EQUALS("ColumnB", Result[2]["name"].asString());
   UNIT_ASSERT_EQUALS(150, Result[2]["width"].asInt());
}

static void testDuplicatePrio() {
   COL_FUNCTION(testDuplicatePri);
   COLmap<COLstring, ICOLinfo> testMap;
   testMap.add("ColumnB", ICOLinfo(150, 1, ""));
   testMap.add("ColumnA", ICOLinfo(100, 1, ""));
   testMap.add("ColumnC", ICOLinfo(200, 2, ""));
   COLvar result;
   ICOLmapSort(testMap, &result);

   UNIT_ASSERT_EQUALS(3, result.size());
   UNIT_ASSERT_EQUALS("ColumnA", result[0]["name"].asString());
   UNIT_ASSERT_EQUALS(100, result[0]["width"].asInt());
   UNIT_ASSERT_EQUALS("ColumnB", result[1]["name"].asString());
   UNIT_ASSERT_EQUALS(150, result[1]["width"].asInt());
   UNIT_ASSERT_EQUALS("ColumnC", result[2]["name"].asString());
   UNIT_ASSERT_EQUALS(200, result[2]["width"].asInt());
}

static void testSortBasic() {
   COL_FUNCTION(testICOLmapToVarBasic);
   COLmap<COLstring, ICOLinfo> TestMap;
   TestMap.add("ColumnA", ICOLinfo(100, 2, ""));
   TestMap.add("ColumnB", ICOLinfo(150, 1, ""));
   TestMap.add("ColumnC", ICOLinfo(200, 3, ""));

   COLvar Result;
   ICOLmapSort(TestMap, &Result);

   UNIT_ASSERT_EQUALS(3, Result.size());
   UNIT_ASSERT_EQUALS("ColumnB", Result[0]["name"].asString());
   UNIT_ASSERT_EQUALS(150, Result[0]["width"].asInt());
   UNIT_ASSERT_EQUALS("ColumnA", Result[1]["name"].asString());
   UNIT_ASSERT_EQUALS(100, Result[1]["width"].asInt());
   UNIT_ASSERT_EQUALS("ColumnC", Result[2]["name"].asString());
   UNIT_ASSERT_EQUALS(200, Result[2]["width"].asInt());
}

void testSort(UNITapp* pApp) {
   pApp->add("/sort/basic", &testSortBasic);
   pApp->add("/sort/duplicate_prio", &testDuplicatePrio);
   pApp->add("/sort/negative_prio", &testNegativePrio);
   pApp->add("/sort/multiple", &testMultipleCases);
}