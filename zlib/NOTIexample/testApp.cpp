// ---------------------------------------------------------------------------
//  Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: testApp.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:  8/11/21, 1:53 PM
// ---------------------------------------------------------------------------
#include "testApp.h"

#include <NOTI/NOTImodel.h>
#include <UNIT/UNITapp.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>

#include <SCK/SCKloop.h>

#include <COL/COLthreadPool.h>

COL_LOG_MODULE;

static COLvar makeTestConfigVar(){
   COLvar ConfigVar;
   ConfigVar["component_id"] = "test";
   ConfigVar["is_on"] = false;
   COLvar Rule1;
   Rule1["tags"].push_back("#prod");
   Rule1["type"] = "error_count";
   Rule1["threshold"] = "1";
   Rule1["limit"] = 6;
   Rule1["priority"] = "low";
   ConfigVar["rules"].push_back(Rule1);
   COLvar Rule2;
   Rule2["tags"].push_back("#prod");
   Rule2["type"] = "inactive_time";
   Rule2["threshold"] = "2";
   Rule2["limit"] = 6;
   Rule2["priority"] = "low";
   ConfigVar["rules"].push_back(Rule2);
   COLvar Rule3;
   Rule3["tags"].push_back("#prod");
   Rule3["type"] = "queue_count";
   Rule3["threshold"] = "3";
   Rule3["limit"] = 6;
   Rule3["priority"] = "low";
   ConfigVar["rules"].push_back(Rule3);
   return ConfigVar;
}

static COLvar makeTestConfigVarOld(){
   COLvar ConfigVar;
   COLvar Rule1;
   Rule1["components"] = "#prod";
   Rule1["condition"] = "error_count";
   Rule1["parameter"] = 1;
   Rule1["invoked_component"] = "test";
   Rule1["limit"] = 6;
   Rule1["priority"] = "low";
   ConfigVar.push_back(Rule1);
   COLvar Rule2;
   Rule2["components"] = "#prod";
   Rule2["condition"] = "inactive_time";
   Rule2["parameter"] = 2;
   Rule2["invoked_component"] = "test";
   Rule2["limit"] = 6;
   Rule2["priority"] = "low";
   ConfigVar.push_back(Rule2);
   COLvar Rule3;
   Rule3["components"] = "#prod";
   Rule3["condition"] = "queue_count";
   Rule3["parameter"] = 3;
   Rule3["invoked_component"] = "test";
   Rule3["limit"] = 6;
   Rule3["priority"] = "low";
   // Old configs would not have a log_contains style rule
   ConfigVar.push_back(Rule3);
   return ConfigVar;
}

static void testNOTImodelFromToDiskVar(){
   COL_FUNCTION(testNOTImodelFromToDiskVar);
   COLvar ConfigVar = makeTestConfigVar();
   COL_VAR(ConfigVar.json(1));
   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);
   NOTImodel Model(&Loop);
   NOTImodelFromDiscVar(&Model, ConfigVar);
   COLvar OutVar;
   NOTImodelToDiscVar(&OutVar, Model);
   COL_VAR(OutVar.json(1));
   UNIT_ASSERT_EQUALS(ConfigVar.json(0), OutVar.json(0));
}

static void testNOTImodelFromToDiskVarOldFormat(){
   COL_FUNCTION(testNOTImodelFromToDiskVarOldFormat);
   COLvar ConfigVarNew = makeTestConfigVar();
   COLvar ConfigVarOld = makeTestConfigVarOld();
   COL_VAR(ConfigVarOld.json(1));
   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);
   NOTImodel Model(&Loop);
   NOTImodelFromDiscVar(&Model, ConfigVarOld);
   COLvar OutVar;
   NOTImodelToDiscVar(&OutVar, Model);
   COL_VAR(OutVar.json(1));
   UNIT_ASSERT_EQUALS(ConfigVarNew.json(1), OutVar.json(1));   // should convert to new format
}

static void testNOTImodelFromToWebVar(){
   COL_FUNCTION(testNOTImodelFromToWebVar);
   COLvar ConfigVar = makeTestConfigVar();
   COL_VAR(ConfigVar.json(1));
   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);
   NOTImodel Model(&Loop);
	COLstring Err;
   NOTImodelFromWebVar(&Model, ConfigVar["component_id"], ConfigVar["is_on"], ConfigVar["rules"], &Err);
   COLvar OutVar;
   NOTImodelToWebVar(&OutVar, Model);
   COL_VAR(OutVar.json(1));
   UNIT_ASSERT_EQUALS(ConfigVar.json(0), OutVar.json(0));
}

void testApp(UNITapp* pApp) {
   // IX-663 add 4 unit tests for the new COLvar serialization functions.
   pApp->add("notification/diskVar", &testNOTImodelFromToDiskVar);
   pApp->add("notification/diskVarOldFormat", &testNOTImodelFromToDiskVarOldFormat);
   pApp->add("notification/webVar", &testNOTImodelFromToWebVar);
}
