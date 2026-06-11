//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testTranslatorHeadless
//
// Description:
//
// Implementation
//
// Author: Vismay Shah
// Date:   Wed May 12 2021
//---------------------------------------------------------------------------
#include "testTranslatorHeadless.h"

#include <BLUA/BLUAmanager.h>
#include <BLUA/BLUAstart.h>
#include <LUA/LUAutils.h>
#include <LTMR/LTMRtimer.h>
#include <LHL7/LHL7api.h>
#include <LND/LNDapi.h>
#include <LND/LNDutils.h>
#include <LNET/LNETnet.h>
#include <LNET/LNETcurl.h>
#include <LDB/LDBapi.h>
#include <DB/DBdatabaseFactory.h>
#include <NODL/NODLenvironment.h>
#include <LSTR/LSTRstring.h>
#include <LJSON/LJSONapi.h>
#include <LND/LNDutils.h>
#include <FIL/FILutils.h>

#include <SDB/SDBcapi.h>
#include <CFG/CFGmap.h>
#include <CFG/CFGmapApi.h>
#include <COL/COLioCheck.h>
#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <SCK/SCKcall.h>
#include <SCK/SCKloop.h>
#include <UNIT/UNITapp.h>
COL_LOG_MODULE;


static int BLUAexampleChangeTimer(lua_State* L){
   COL_FUNCTION(BLUAexampleChangeTimer);
   COL_TRC("Timer event - changing TimerTarget to true");
   LUAsetBooleanField(L, "TimerTarget", true);
   return 0;
}

static int BLUAexampleCheckTimer(lua_State* L){
   COL_FUNCTION(BLUAexampleCheckTimer);
   COL_TRC("Timer event - making sure TimerTarget is set to true");
   UNIT_ASSERT(LUAgetBooleanField(L, "TimerTarget"));
   return 0;
}

static void BLUAexampleLuaRegister(lua_State* L, LUAmode Mode, SCKloop* pLoop, COLhashmap<COLstring, COLwebHandlerFunc>* pWorkerMap,
                                   COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >* pMainThreadMap,
                                   DBdatabaseFactory* pDatabaseFactory=nullptr){ // Like DBDluaRegister but more limited
   COL_FUNCTION(BLUAexampleLuaRegister);
   COL_TRC("Lua state created, registering SCKloop and Api maps");
   LUAsetPointerField(L, "_pEventLoop", pLoop);
   LUAsetPointerField(L, "_pWorkerMap", pWorkerMap);
   LUAsetPointerField(L, "_pMainThreadMap", pMainThreadMap);
   LUAsetPointerField(L, "DatabaseFactory", pDatabaseFactory);

   COL_TRC("registering tables and functions");
   lua_newtable(L);
   LUAaddMethod(L, "setTimer",     &LTMRsetTimer);
   lua_setglobal(L, "component");
   LUAsetBooleanField(L, "TimerTarget", false);
   lua_newtable(L);
   LUAaddMethod(L, "change",   &BLUAexampleChangeTimer);
   LUAaddMethod(L, "check",    &BLUAexampleCheckTimer);
   lua_setglobal(L, "TimerFireTest");
   LJSONapiAddJsonNamespace(L);
   LHL7api(L);
   LDBaddDatabaseNamespace(L);
   LDBaddDatabaseConnectionCount(L);
   LSTRaddStringNamespace(L);
   LNETaddNetNamespace(L);
   LNDaddNodeNamespace(L);
   LNDdefineTree(L);
   NODLcreateDatabaseCache(L);
   LNETcreateCurlCache(L);
}

void BLUAexampleDoSetup(SCKloop* pLoop, BLUAinstanceManager* pManager, COLhashmap<COLstring, COLwebHandlerFunc>* pWorkerMap,
                        COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >* pMainMap, DBdatabaseFactory* pDatabaseFactory=nullptr){
   COL_FUNCTION(BLUAexampleDoSetup);
   COL_TRC("Setting up callback for registering tables when Lua state is created");
   LUAsetOnLuaStateCreated(COLnewClosure2(&BLUAexampleLuaRegister, pLoop, pWorkerMap, pMainMap, pDatabaseFactory));
}

// Helper for testThreadedHelloWorld
static void done(SCKloop* pLoop){
   COL_FUNCTION(done);
   COL_TRC("Script execution finished signal received on main thread");
   pLoop->shutdown();
}

static void cleanupApiMaps(COLhashmap<COLstring, COLwebHandlerFunc>* pWorkerMap, COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >* pMainMap){
   COL_FUNCTION(cleanupApiMaps);
   delete pWorkerMap;
   delete pMainMap;
}

static void cleanupDbFactory(DBdatabaseFactory* pDatabaseFactory){
   COL_FUNCTION(cleanupDbFactory);
   delete pDatabaseFactory;
}

static void boilerplate(SCKloop* pLoop, BLUAinstanceManager* pManager, COLhashmap<COLstring, COLwebHandlerFunc>** ppWorkerMap,
                        COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >** ppMainMap, const COLarray<CFGconfig*>& Configs,
                        DBdatabaseFactory* pDatabaseFactory=nullptr){
   COL_FUNCTION(boilerplate);
   SDBITsetIsUnitTest(); // This prevents SDBIlogComponent from trying to do anything.
   COL_TRC("Creating Api maps");
   *ppWorkerMap = new COLhashmap<COLstring, COLwebHandlerFunc>();
   *ppMainMap   = new COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >();
   BLUAexampleDoSetup(pLoop, pManager, *ppWorkerMap, *ppMainMap, pDatabaseFactory);
}

///////////////////////////////////////
// UNIT TESTS
///////////////////////////////////////

// Runs an empty script - just to make sure nothing is leaked when starting and stopping a component.
static void testEmptyScript(){
   COL_FUNCTION(testEmptyScript);
   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);
   BLUAinstanceManager Manager;
   CFGconfig Config;
   Config.CoreConfig.Guid="AAA";
   COLarray<CFGconfig*> Configs;
   Configs.push_back(&Config);
   COLhashmap<COLstring, COLwebHandlerFunc>* pWorkerMap = nullptr;
   COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >* pMainMap = nullptr;
   boilerplate(&Loop, &Manager, &pWorkerMap, &pMainMap, Configs);
   BLUAinstanceStart(&Manager, &Config, &Loop);
   BLUAinstanceStopAll(COLnewClosure0(&done, &Loop), &Manager);
   Loop.start();
   COL_VAR(Manager.m_Instances.size());
   cleanupApiMaps(pWorkerMap, pMainMap);
}

static void timerExpired(SCKloop* pLoop, BLUAinstanceManager* pManager, COLstring Guid, CFGconfig* pConfig){
   COL_FUNCTION(timerExpired);
   COL_TRC("Timer should have expired by now");
   COL_VAR(Guid); // Super helpful if tracing on multiple tests.
   COL_VAR(pConfig->Status.ErrorCount);
   if (Guid == "BadScript") { UNIT_ASSERT_EQUALS(1, pConfig->Status.ErrorCount); } // This test does result in a compile failure and an error being set.
   else                     { UNIT_ASSERT_EQUALS(0, pConfig->Status.ErrorCount); }
   BLUAinstanceStopAll(COLnewClosure0(&done, pLoop), pManager);
}

// Runs an bad script with no main function defined (IX-2662).
static void testNoMain(){
   COL_FUNCTION(testNoMain);
   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);
   BLUAinstanceManager Manager;
   CFGconfig Config;
   Config.CoreConfig.Guid="NoMain";
   COLarray<CFGconfig*> Configs;
   Configs.push_back(&Config);
   COLhashmap<COLstring, COLwebHandlerFunc>* pWorkerMap = nullptr;
   COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >* pMainMap = nullptr;
   boilerplate(&Loop, &Manager, &pWorkerMap, &pMainMap, Configs);
   BLUAinstanceStart(&Manager, &Config, &Loop);
   int TimerId = Loop.addTimer(COLnewClosure0(timerExpired, &Loop, &Manager, COLstring("BadScript"), &Config), 200, false, "badscripttest");
   Loop.start();
   COL_VAR(Manager.m_Instances.size());
   cleanupApiMaps(pWorkerMap, pMainMap);
   UNIT_ASSERT_EQUALS(1, Config.Status.ErrorCount);
}

// Runs a script which sets up a 200ms timer to call BLUAexampleChangeTimer to change the value of TimerTarget,
// and a 400ms timer to call BLUAexampleCheckTimer to verify via UNITassert that the bool was changed
static void testBluaTimer(){
   COL_FUNCTION(testBluaTimer);
   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);
   BLUAinstanceManager Manager;
   CFGconfig Config;
   Config.CoreConfig.Guid="Timer";
   COLarray<CFGconfig*> Configs;
   Configs.push_back(&Config);
   COLhashmap<COLstring, COLwebHandlerFunc>* pWorkerMap = nullptr;
   COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >* pMainMap = nullptr;
   boilerplate(&Loop, &Manager, &pWorkerMap, &pMainMap, Configs);
   BLUAinstanceStart(&Manager, &Config, &Loop);
   int TimerId = Loop.addTimer(COLnewClosure0(timerExpired, &Loop, &Manager, COLstring("Timer"), &Config), 1000, false, "timerfiretest");
   COL_VAR(TimerId);
   Loop.start();
   cleanupApiMaps(pWorkerMap, pMainMap);
}

// Runs the same script as testBluaTimer but immediately stops the instance so the timers don't have a chance to fire to test the cleanup logic.
static void testBluaTimerEarlyClose(){
   COL_FUNCTION(testBluaTimerEarlyClose);
   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);
   BLUAinstanceManager Manager;
   CFGconfig Config;
   Config.CoreConfig.Guid="Timer";
   COLarray<CFGconfig*> Configs;
   Configs.push_back(&Config);
   COLhashmap<COLstring, COLwebHandlerFunc>* pWorkerMap = nullptr;
   COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >* pMainMap = nullptr;
   boilerplate(&Loop, &Manager, &pWorkerMap, &pMainMap, Configs);
   BLUAinstanceStart(&Manager, &Config, &Loop);
   COLclosure0* pStopped = COLnewClosure0(&done, &Loop);
   // stop instance in 100ms, before the 200ms and 400ms timer fires
   Loop.addTimer(COLnewClosure0(&BLUAinstanceStopAll, pStopped, &Manager), 100);
   Loop.start();
   cleanupApiMaps(pWorkerMap, pMainMap);
}

// This test parses an hl7 message using ack.vmd, then writes out an ack message using ack.vmd
// This test exercises hl7.parse and hl7.message. It tests out the various memory leaks found in IX-2603
static void testBluaHl7ParseAndAck(){
   COL_FUNCTION(testBluaHl7ParseAndAck);
   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);
   BLUAinstanceManager Manager;
   CFGconfig Config;
   Config.CoreConfig.Guid="Hl7";
   COLarray<CFGconfig*> Configs;
   Configs.push_back(&Config);
   COLhashmap<COLstring, COLwebHandlerFunc>* pWorkerMap = nullptr;
   COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >* pMainMap = nullptr;
   boilerplate(&Loop, &Manager, &pWorkerMap, &pMainMap, Configs);
   BLUAinstanceStart(&Manager, &Config, &Loop);
   int TimerId = Loop.addTimer(COLnewClosure0(timerExpired, &Loop, &Manager, COLstring("Hl7"), &Config), 1500, false, "hl7test");
   COL_VAR(TimerId);
   Loop.start();
   cleanupApiMaps(pWorkerMap, pMainMap);
}

// This test sets up a web server using net.http.listen on port 12321 which will reply to requests with 'hi there'
// This test then uses net.http.get to call https://www.google.com and http://localhost:12321
static void testBluaWebGet(){
   COL_FUNCTION(testBluaWebGet);
   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);
   BLUAinstanceManager Manager;
   // Setup for component which does net.http.get calls
   CFGconfig Config1;
   Config1.CoreConfig.Guid="Web";
   COLarray<CFGconfig*> Configs;
   Configs.push_back(&Config1);
   // Setup for component which acts as a server and uses net.http.listen and net.http.respond
   CFGconfig Config2;
   Config2.CoreConfig.Guid="Server";
   Configs.push_back(&Config2);
   // do boiler plate, then start sever first, then the web component
   COLhashmap<COLstring, COLwebHandlerFunc>* pWorkerMap = nullptr;
   COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >* pMainMap = nullptr;
   boilerplate(&Loop, &Manager, &pWorkerMap, &pMainMap, Configs);
   BLUAinstanceStart(&Manager, &Config2, &Loop); // start server first
   BLUAinstanceStart(&Manager, &Config1, &Loop);
   int TimerId = Loop.addTimer(COLnewClosure0(timerExpired, &Loop, &Manager, COLstring("Web"), &Config1), 2000, false, "web_get");
   COL_VAR(TimerId);
   Loop.start();
   cleanupApiMaps(pWorkerMap, pMainMap);
}


static void initializeDbFactory(DBdatabaseFactory* pDbFactory, COLstring* pLog){
   COL_FUNCTION(initializeDbFactory);
   COL_TRC("Initializing database factory to gain access to sqlite");
   COLostream LogStream(*pLog);
   pDbFactory->initialize(LogStream);
   COL_VAR(*pLog);
}

// This test sets up a sqlite database in repos/Database called test.db.
// The script tries to use a schema file to populate the database using db.tables and db.merge.
// The script checks that dbconnection.query and db.connection.execute are working
// As part of the clean up, the database file is deleted.
static void testBluaDatabase(){
   COL_FUNCTION(testBluaDatabase);
   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);
   BLUAinstanceManager Manager;
   DBdatabaseFactory* pDbFactory = new DBdatabaseFactory();
   COLstring Log;
   initializeDbFactory(pDbFactory, &Log);
   CFGconfig Config;
   Config.CoreConfig.Guid="Database";
   COLarray<CFGconfig*> Configs;
   Configs.push_back(&Config);
   COLhashmap<COLstring, COLwebHandlerFunc>* pWorkerMap = nullptr;
   COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >* pMainMap = nullptr;
   boilerplate(&Loop, &Manager, &pWorkerMap, &pMainMap, Configs, pDbFactory);
   BLUAinstanceStart(&Manager, &Config, &Loop);
   int TimerId = Loop.addTimer(COLnewClosure0(timerExpired, &Loop, &Manager, COLstring("Database"), &Config), 1000, false, "db");
   COL_VAR(TimerId);
   Loop.start();
   cleanupApiMaps(pWorkerMap, pMainMap);
   cleanupDbFactory(pDbFactory);
   COLstring DbPath = "test.db";
   FILremove(DbPath);
}

// Similar to the test above, but this time in global scope to prevent
// issues like IX-4314
static void testBluaDatabaseGlobal(){
   COL_FUNCTION(testBluaDatabaseGlobal);
   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);
   BLUAinstanceManager Manager;
   DBdatabaseFactory* pDbFactory = new DBdatabaseFactory();
   COLstring Log;
   initializeDbFactory(pDbFactory, &Log);
   CFGconfig Config;
   Config.CoreConfig.Guid="DatabaseGlobal";
   COLarray<CFGconfig*> Configs;
   Configs.push_back(&Config);
   COLhashmap<COLstring, COLwebHandlerFunc>* pWorkerMap = nullptr;
   COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >* pMainMap = nullptr;
   boilerplate(&Loop, &Manager, &pWorkerMap, &pMainMap, Configs, pDbFactory);
   BLUAinstanceStart(&Manager, &Config, &Loop);
   int TimerId = Loop.addTimer(COLnewClosure0(timerExpired, &Loop, &Manager, COLstring("DatabaseGlobal"), &Config), 1000, false, "dbglobal");
   COL_VAR(TimerId);
   Loop.start();
   cleanupApiMaps(pWorkerMap, pMainMap);
   cleanupDbFactory(pDbFactory);
   COLstring DbPath = "test2.db";
   FILremove(DbPath);
}

// Another one from IX-4314, ensuring the lua stack remains safe
static void testBluaRequire(){
   COL_FUNCTION(testBluaRequire);
   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);
   BLUAinstanceManager Manager;
   CFGconfig Config;
   Config.CoreConfig.Guid="GlobalRequire";
   COLarray<CFGconfig*> Configs;
   Configs.push_back(&Config);
   COLhashmap<COLstring, COLwebHandlerFunc>* pWorkerMap = nullptr;
   COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >* pMainMap = nullptr;
   boilerplate(&Loop, &Manager, &pWorkerMap, &pMainMap, Configs);
   BLUAinstanceStart(&Manager, &Config, &Loop);
   int TimerId = Loop.addTimer(COLnewClosure0(timerExpired, &Loop, &Manager, COLstring("GlobalRequire"), &Config), 1500, false, "globalrequire");
   COL_VAR(TimerId);
   Loop.start();
   cleanupApiMaps(pWorkerMap, pMainMap);
}

void testTranslatorHeadless(UNITapp* pApp){
   pApp->add("BLUA/EmptyScript",     &testEmptyScript);
   pApp->add("BLUA/BadScript",       &testNoMain);
   // pApp->add("BLUA/Timer/Fire",      &testBluaTimer);
   pApp->add("BLUA/Timer/StopEarly", &testBluaTimerEarlyClose);
   pApp->add("BLUA/hl7/parseAndAck", &testBluaHl7ParseAndAck);
   pApp->add("BLUA/web/get",         &testBluaWebGet);
   pApp->add("BLUA/db/test",         &testBluaDatabase);
   pApp->add("BLUA/db/test/global",  &testBluaDatabaseGlobal);
   pApp->add("BLUA/require",         &testBluaRequire);
}