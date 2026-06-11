// ---------------------------------------------------------------------------
//  Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: testDetect.cpp
//
//  Description: Implementation
//
//  Author: Eliot Muir
//  Date:   Tue 13 Sep 2022 15:46:05 EDT
// ---------------------------------------------------------------------------
#include <ctime>

#include <CFG/CFGmap.h>

#include <NOTI/NOTIdetect.h>
#include <NOTI/NOTImodel.h>

#include <SDB/SDBmessage.h>
#include <SDB/SDBcapi.h>

#include <UNIT/UNITapp.h>

#include <COL/COLutils.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static void addComponent(CFGmap* pMap, const COLstring& Id){
   pMap->ConfigMap.add(Id, CFGconfig());
   pMap->ConfigMap[Id].CoreConfig.Guid = Id;
}

static void createMap(CFGmap* pMap){
   addComponent(pMap, "ID1");
   addComponent(pMap, "ID2");
   addComponent(pMap, "ID3");
   pMap->ConfigMap["ID1"].Status.ErrorCount = 10;
   SDBITsetTestQueueCount("ID2", 10);
   pMap->ConfigMap["ID3"].Status.LastActivity = time(NULL)-100;
   pMap->ConfigMap["ID3"].Status.StartTime    = time(NULL)-120;
   COLarray<COLstring> ID1Tags;
   ID1Tags.push_back("#emr");
   ID1Tags.push_back("#prod");
   COLarray<COLstring> ID2Tags;
   ID2Tags.push_back("#prod");
   ID2Tags.push_back("#radiology");
   COLarray<COLstring> ID3Tags;
   ID3Tags.push_back("#prod");
   ID3Tags.push_back("#emr");
   pMap->ConfigMap["ID1"].CoreConfig.Tags = ID1Tags;
   pMap->ConfigMap["ID2"].CoreConfig.Tags = ID2Tags;
   pMap->ConfigMap["ID3"].CoreConfig.Tags = ID3Tags;
}

static void createRules(COLarray<NOTIrule>* pRules){
   COLarray<COLstring> Prod;
   Prod.push_back("#prod");
   pRules->push_back(NOTIrule(Prod, NOTIruleType::INACTIVE, "1"));
   pRules->push_back(NOTIrule(Prod, NOTIruleType::QUEUE, "5"));
   pRules->push_back(NOTIrule(Prod, NOTIruleType::HAS_ERRORS, "1"));
   pRules->push_back(NOTIrule(Prod, NOTIruleType::LOG_CONTAINS, "Exceeded memory limit"));
}

static COLvar createActual(const COLstring& ComponentId, const COLstring& Threshold) {
   COLvar Notification;
   Notification["component"] = ComponentId;
   Notification["criteria"] = "log_contains";
   Notification["priority"] = "low";
   Notification["threshold"] = Threshold;
	Notification["msg_index"] = "0";
	Notification["msg_time"] = "0";
   COLvar Actual;
   Actual.push_back(Notification);
   return Actual;
}

static void testNotiNotLog() {
   COL_FUNCTION(testNotiNotLog);
   CFGmap Map;
	createMap(&Map);
   COLarray<COLstring> Matches;
   Matches.push_back("#prod");
   NOTIrule DashError = NOTIrule(Matches, NOTIruleType::LOG_CONTAINS, "NOT error AND warning");
   SDBmessage Message;
   Message.Id.MessageId.ComponentId = "ID1";
   Message.Data = "error";
   COLvar Result;

   NOTIpatternRule(&Message, &DashError, Map.ConfigMap["ID1"], &Result);
   COL_VAR(Result.json(1));
   UNIT_ASSERT_EQUALS(COLvar(), Result);
   Result.reset();

   NOTIrule NotError = NOTIrule(Matches, NOTIruleType::LOG_CONTAINS, "/NOT error/");
   NOTIpatternRule(&Message, &NotError, Map.ConfigMap["ID1"], &Result);
   COL_VAR(Result.json(1));
   Result.reset();

   Message.Data = "this error should be ignored and result in empty COLvar";
   NOTIpatternRule(&Message, &NotError, Map.ConfigMap["ID1"], &Result);
   COL_VAR(Result.json(1));
   UNIT_ASSERT_EQUALS(COLvar(), Result);

   NOTIrule NotErrorOnlyWarning = NOTIrule(Matches, NOTIruleType::LOG_CONTAINS, "NOT error warning");
   Message.Data = "warning -- possible name conflict";
   NOTIpatternRule(&Message, &NotErrorOnlyWarning, Map.ConfigMap["ID1"], &Result);
   COL_VAR(Result.json(1));
   UNIT_ASSERT_EQUALS(createActual("ID1", "NOT error warning"), Result);
}

// many regex tests can be found in TXTTtestTextSearch
static void testNotiRegex(){
   COL_FUNCTION(testNotiRegex);
   CFGmap Map;
   COLarray<COLstring> Matches;
   Matches.push_back("#prod");
   NOTIrule NameConflict = NOTIrule(Matches, NOTIruleType::LOG_CONTAINS, "/Warning|Error possible name conflict/");
   createMap(&Map);
   SDBmessage Message;
   Message.Id.MessageId.ComponentId = "ID1";
   Message.Data = "Error possible name conflict";
   COLvar Result;
   NOTIpatternRule(&Message, &NameConflict, Map.ConfigMap["ID1"], &Result);
   COL_VAR(Result.json(1));
   UNIT_ASSERT_EQUALS(createActual("ID1", "/Warning|Error possible name conflict/"), Result);

   NOTIrule NotNameConflict = NOTIrule(Matches, NOTIruleType::LOG_CONTAINS, "-error");
   Message.Data = "error";
   COLvar Result2;
   NOTIpatternRule(&Message, &NotNameConflict, Map.ConfigMap["ID1"], &Result2);
   UNIT_ASSERT_EQUALS(COLvar(), Result2);
}

static void testNotiLogContains(){
   COL_FUNCTION(testNotiLogContains);
   CFGmap Map;
   COLarray<COLstring> Matches;
   Matches.push_back("#radiology");
   NOTIrule MemoryMatch = NOTIrule(Matches, NOTIruleType::LOG_CONTAINS, "Exceeded memory limit");
   createMap(&Map);
   SDBmessage Message;
   Message.Id.MessageId.ComponentId = "ID2";
   Message.Data = "Random log error: Exceeded memory limit";
   COLvar Result;
   NOTIpatternRule(&Message, &MemoryMatch, Map.ConfigMap["ID2"], &Result);
   COL_VAR(Result.json(1));
   UNIT_ASSERT_EQUALS(createActual("ID2", "Exceeded memory limit"), Result);
   Result.reset();

   NOTIpatternRule(&Message, &MemoryMatch, Map.ConfigMap["ID1"], &Result);
   UNIT_ASSERT_EQUALS(COLvar(), Result);
   Result.reset();

   NOTIpatternRule(&Message, &MemoryMatch, Map.ConfigMap["ID3"], &Result);
   UNIT_ASSERT_EQUALS(COLvar(), Result);
}

static void testDetectSimple(){
   COL_FUNCTION(testDetectSimple);
   CFGmap Map;
   createMap(&Map);
	COLarray<NOTIrule> Rules;
   createRules(&Rules);
   COLvar ErrorList;
   for (auto i : Rules){ NOTIdetectRule(Map, &i, &ErrorList); }
   COL_VAR(ErrorList);
   UNIT_ASSERT_EQUALS(3, ErrorList.size());
}

static void testInactive(){
   COL_FUNCTION(testInactive);
   CFGmap Map;
   createMap(&Map);
   Map.ConfigMap["ID3"].Status.LastActivity = 0;
   Map.ConfigMap["ID3"].Status.StartTime = 0;
   COLarray<COLstring> Prod;
   Prod.push_back("#prod");
   NOTIrule rule(Prod, NOTIruleType::INACTIVE, "1");
   rule.Limit = 0;
   COLvar ErrorList;

   COL_TRC("Default case, component not running yet");
   NOTIdetectRule(Map, &rule, &ErrorList);
   UNIT_ASSERT_EQUALS(0, ErrorList.size());
   ErrorList.reset();

   COL_TRC("Started component 40 seconds ago, no enqueue or dequeue");
   Map.ConfigMap["ID3"].Status.StartTime    = time(NULL)-40;
   NOTIdetectRule(Map, &rule, &ErrorList);
   UNIT_ASSERT_EQUALS(0, ErrorList.size());
   ErrorList.reset();

   COL_TRC("Started component 100 seconds ago, no enqueue or dequeue");
   Map.ConfigMap["ID3"].Status.StartTime    = time(NULL)-100;
   NOTIdetectRule(Map, &rule, &ErrorList);
   UNIT_ASSERT_EQUALS(1, ErrorList.size());
   ErrorList.reset();
   
   COL_TRC("Started the component 100 seconds ago, enqueued/dequeued 80 seconds ago.");
   Map.ConfigMap["ID3"].Status.LastActivity = time(NULL)-80;
   NOTIdetectRule(Map, &rule, &ErrorList);
   UNIT_ASSERT_EQUALS(1, ErrorList.size());
   ErrorList.reset();

   COL_TRC("Started the component 100 seconds ago, enqueued/dequeued 50 seconds ago.");
   Map.ConfigMap["ID3"].Status.LastActivity = time(NULL)-50;
   NOTIdetectRule(Map, &rule, &ErrorList);
   UNIT_ASSERT_EQUALS(0, ErrorList.size());
   ErrorList.reset();

   COL_TRC("enqueued/dequeued 40 seconds ago, then stop the component.");
   Map.ConfigMap["ID3"].Status.StartTime = 0;
   Map.ConfigMap["ID3"].Status.LastActivity = time(NULL)-40;
   NOTIdetectRule(Map, &rule, &ErrorList);
   UNIT_ASSERT_EQUALS(0, ErrorList.size());
   ErrorList.reset();
}

static void testInterval(){
   COL_FUNCTION(testInterval);
   CFGmap Map;
   createMap(&Map);
   COLarray<COLstring> Prod;
   Prod.push_back("#prod");
   NOTIrule rule(Prod, NOTIruleType::HAS_ERRORS, "1");
   rule.Limit = 3600;
   COLvar ErrorList;
   COL_TRC("Started the component 100 seconds ago, last activity met interval requirement.");
   NOTIdetectRule(Map, &rule, &ErrorList);
   UNIT_ASSERT_EQUALS(1, ErrorList.size());
   ErrorList.reset();

   NOTIdetectRule(Map, &rule, &ErrorList);
   UNIT_ASSERT_EQUALS(0, ErrorList.size());
   ErrorList.reset();

   COLsleep(1000);

   NOTIdetectRule(Map, &rule, &ErrorList);
   UNIT_ASSERT_EQUALS(1, ErrorList.size());
   ErrorList.reset();
}

void testDetect(UNITapp* pApp) {
   pApp->add("notification/detect_simple", &testDetectSimple);
   pApp->add("notifications/log_contains", &testNotiLogContains);
   pApp->add("notifications/regex",        &testNotiRegex);
   pApp->add("notifications/not_log",      &testNotiNotLog);
   pApp->add("notifications/inactive",     &testInactive);
   pApp->add("notifications/interval",     &testInterval);
}
