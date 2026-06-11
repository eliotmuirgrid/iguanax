//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DASHexample
//
// Description: Implementation
//
// Author: Matthew Sobkowski
// Date:   08/07/25 2:19 PM
//---------------------------------------------------------------------------

#include "testApi.h"
#include <SDB/SDBcapi.h>
#include <DASH/DASHmapData.h>
#include <DASH/DASHlistView.h>
#include <CFG/CFGconfig.h>
#include <CFG/CFGmap.h>
#include <COL/COLlog.h>
#include <UNIT/UNITapp.h>
#include <DASH/DASHlistComparison.h>
#include <DASH/DASHsort.h>
COL_LOG_MODULE;

static CFGmap exampleMap1(){
   CFGmap Map;
   CFGconfig Config1, Config2, Config3, Config4, Config5;
   Config1.CoreConfig.Name = "abc";
   Config2.CoreConfig.Name = "def";
   Config3.CoreConfig.Name = "zzz";
   Config4.CoreConfig.Name = "aAa";
   Config5.CoreConfig.Name = "AAAA";
   Map.ConfigMap["abc"] = Config1;
   Map.ConfigMap["def"] = Config2;
   Map.ConfigMap["zzz"] = Config3;
   Map.ConfigMap["aAa"] = Config4;
   Map.ConfigMap["AAAA"] = Config5;
   return Map;
}

static void testDashboardApi(){
   COL_FUNCTION(testDashboardApi);
}

static void testCompareFunction(){
   COL_FUNCTION(testCompareFunction);
   DASHcompareFunc pFunc =  DASHgetCompareFunction("QUE");
   UNIT_ASSERT(pFunc != NULL);
   CFGconfig ConfigRhs;
   CFGconfig ConfigLhs;
   ConfigLhs.CoreConfig.Guid = "A";
   SDBITsetTestQueueCount("A", 10);
   ConfigRhs.CoreConfig.Guid = "B";
   SDBITsetTestQueueCount("B", 11);
   bool Result = (*pFunc)(&ConfigLhs, &ConfigRhs, true, "QUE");
   COL_VAR(Result);
   UNIT_ASSERT_EQUALS(Result, true);
}

static void testQueueCountEqual(){
   COL_FUNCTION(testQueueCountEqual);
   DASHcompareFunc pFunc =  DASHgetCompareFunction("QUE");
   UNIT_ASSERT(pFunc != NULL);
   CFGconfig ConfigRhs;
   CFGconfig ConfigLhs;
   ConfigLhs.CoreConfig.Name = "Fred";
   ConfigLhs.CoreConfig.Guid = "Fred";
   SDBITsetTestQueueCount("Fred", 10);
   ConfigRhs.CoreConfig.Name = "Larry";
   ConfigLhs.CoreConfig.Guid = "Larry";
   SDBITsetTestQueueCount("Larry", 10);
   bool Result = (*pFunc)(&ConfigLhs, &ConfigRhs, true, "QUE");
   COL_VAR(Result);
}

// These result in the same answer no match which order - how does that work with quick sorts
// maybe
static void testNameEmpty(){
   COL_FUNCTION(testNameEmpty);
   DASHcompareFunc pFunc =  DASHgetCompareFunction("NAME");
   UNIT_ASSERT(pFunc != NULL);
   CFGconfig ConfigRhs;
   CFGconfig ConfigLhs;
   ConfigLhs.CoreConfig.Name = "";
   ConfigRhs.CoreConfig.Name = "";  // deliberately empty
   bool Result = (*pFunc)(&ConfigLhs, &ConfigRhs, true, "NAME");
   COL_VAR(Result);
   Result = (*pFunc)(&ConfigRhs, &ConfigLhs, true, "NAME");
   COL_VAR(Result);
}

static void testCompareName(){
   COL_FUNCTION(testCompareName);
   DASHcompareFunc pFunc =  DASHgetCompareFunction("NAME");
   UNIT_ASSERT(pFunc != NULL);
   CFGconfig ConfigRhs;
   CFGconfig ConfigLhs;
   ConfigLhs.CoreConfig.Name = "abc";
   ConfigRhs.CoreConfig.Name = "def";
   bool Result = (*pFunc)(&ConfigLhs, &ConfigRhs, true, "NAME");
   UNIT_ASSERT_EQUALS(Result, false);
   ConfigLhs.CoreConfig.Name = "abc";
   ConfigRhs.CoreConfig.Name = "Def";
   Result = (*pFunc)(&ConfigLhs, &ConfigRhs,true, "NAME");
   UNIT_ASSERT_EQUALS(Result, false);
   ConfigLhs.CoreConfig.Name = "Def";
   ConfigRhs.CoreConfig.Name = "abc";
   Result = (*pFunc)(&ConfigLhs, &ConfigRhs,true, "NAME");
   UNIT_ASSERT_EQUALS(Result, true);
}

static void testSortName(){
   COL_FUNCTION(testSortName);
   CFGmap Map = exampleMap1();
   COLarray<COLstring> Data;
   Data.push_back("abc");
   Data.push_back("def");
   Data.push_back("zzz");
   Data.push_back("aAa");
   Data.push_back("AAAA");
   DASHsort("name", true, &Map, &Data);
   for(const auto& it : Data) { COL_VAR(it); }
}

static CFGmap exampleMap2(){
   COL_FUNCTION(exampleMap2);
   CFGmap Map;
   for(int i =1; i <=10; i++){
      CFGconfig Config;
      Config.CoreConfig.Name = COLintToString(i);
      Config.CoreConfig.Guid = COLintToString(i);
      Config.CoreConfig.Description = "";
      if(i == 2) {
         Config.Status.ErrorCount = 3;
         Config.Status.Border     = "#ff0000";
      }
      Map.ConfigMap[COLintToString(i)] = Config;
   }
   for(int i = 2; i <= 10; i++){ CFGconfigAddSource(COLintToString(i), &Map.ConfigMap["1"]); }
   return Map;
}

static void testGlobMatching(){
   COL_FUNCTION(testGlobMatching);
   COLarray<COLstring> Components;
   CFGmap Map = exampleMap2();
   DASHglobMatch(" ", &Map, false, &Components);
   for(const auto& it : Components) { COL_VAR(it); }
}

static void testNearestNeighbours(){
   COL_FUNCTION(testNearestNeighbours);
   CFGmap Map = exampleMap2();
   COLvar Result;
   COLarray<COLstring> Selected;
   Selected.push_back("2");
   DASHmapDataImp(" ", false, Selected, true, &Map, Result);
   COL_VAR(Result.json(1));
   UNIT_ASSERT_EQUALS(Result.size(), 2);
   UNIT_ASSERT_EQUALS(Result.exists("1") , true);
   UNIT_ASSERT_EQUALS(Result.exists("2") , true);
   UNIT_ASSERT_EQUALS(Result.exists("3") , false);
   UNIT_ASSERT_EQUALS(Result["2"]["error_count"].asInt(), 3);
   UNIT_ASSERT_EQUALS("#ff0000", Result["2"]["border"].asString());
}

void testApi(UNITapp* pApp){
   pApp->add("dashboard/api", &testDashboardApi);
   pApp->add("compare/queue/count", &testCompareFunction);
   pApp->add("compare/queue/count/equal", &testQueueCountEqual);
   pApp->add("compare/name/empty", &testNameEmpty);
   pApp->add("compare/name", &testCompareName);
   pApp->add("sort/name", &testSortName);
   pApp->add("glob_matching", &testGlobMatching);
   pApp->add("map/neighbours", &testNearestNeighbours);
}
