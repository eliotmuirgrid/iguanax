//---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testCallMain
//
// Description:
//
// Implementation
//
// Author: Vismay Shah
// Date:   Thu 30 Mar 2023 1:32pm
//---------------------------------------------------------------------------

#include "testCallMain.h"
#include <UNIT/UNITapp.h>
#include <SCK/SCKloop.h>
#include <SCK/SCKcall.h>
#include <COL/COLvar.h>
#include <COL/COLthreadPool.h>
#include <COL/COLclosure.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static void MyMainThreadFunc(const COLwebRequest& Request){
   COL_FUNCTION(MyMainThreadFunc);
   COL_TRC("I exist to be run on the main thread");
   COL_TRC("Here are the inputs");
   COL_VAR(Request.Data);
   COL_TRC("I'll return some data");
   COLvar Result;
   Result["foo"] = "bar";
   COLrespondSuccess(Request.Address, Result);
   COL_TRC("Exiting function.");
}

static void WorkerThreadFunction(SCKloop* pLoop){
   COL_FUNCTION(WorkerThreadFunction);
   COL_TRC("On worker thread");
   COLvar Input, Result;
   Input["E.T"] = "phone home";
   COL_TRC("Doing SCKcall");
   bool CallResult = SCKcall("mt/func1", Input, COLstring(), &Result, false);
   COL_TRC("Returned to worker thread");
   UNIT_ASSERT(CallResult);
   UNIT_ASSERT_EQUALS(true, Result["success"].asBool());
   UNIT_ASSERT_EQUALS("bar", Result["data"]["foo"].asString());
   pLoop->shutdown();
}

// This test just shows that "MyMainThreadFunc" is running on the main thread when called from the worker thread.
static void testCallBasic(){
   COL_FUNCTION(testCallBasic);
   COLthreadPool ThreadPool;
   ThreadPool.start();
   SCKloop Loop(&ThreadPool);
   // Need a hashmap of registered functions to be run ON MAIN THREAD
   COLhashmap<COLstring, COLwebHandlerFunc> TestFunctionMap;
   TestFunctionMap.add("mt/func1", &MyMainThreadFunc);
   SCKdefineSingleton(&Loop, &TestFunctionMap, nullptr);
   // Set up a function to run on a worker thread. This function will post to the main thread to get some data.
   Loop.threadPool()->scheduleTask(COLnewClosure0(&WorkerThreadFunction, &Loop));
   COL_TRC("We're starting the loop...");
   Loop.start();
   COL_TRC("...the loop was shutdown and is done.");
}

static void MyMainThreadFuncError(const COLwebRequest& Request){
   COL_FUNCTION(MyMainThreadFuncError);
   COL_TRC("About to throw error on the main thread");
   throw COLerror("Something went wrong.");
}

static void WorkerThreadFunctionError(SCKloop* pLoop){
   COL_FUNCTION(WorkerThreadFunctionError);
   COL_TRC("On worker thread");
   COLvar Input, Result;
   COL_TRC("Doing SCKcall");
   bool CallResult = SCKcall("mt/func1", Input, COLstring(), &Result, false);
   COL_TRC("Returned to worker thread");
   UNIT_ASSERT(!CallResult);
   UNIT_ASSERT_EQUALS(false, Result["success"].asBool());
   UNIT_ASSERT_EQUALS("Something went wrong.", Result["error"].asString());
   pLoop->shutdown();
}

// This test just shows that "MyMainThreadFunc" is running on the main thread when called from the worker thread.
static void testCallBasicError(){
   COL_FUNCTION(testCallBasicError);
   COLthreadPool ThreadPool;
   ThreadPool.start();
   SCKloop Loop(&ThreadPool);
   // Need a hashmap of registered functions to be run ON MAIN THREAD
   COLhashmap<COLstring, COLwebHandlerFunc> TestFunctionMap;
   TestFunctionMap.add("mt/func1", &MyMainThreadFuncError);
   SCKdefineSingleton(&Loop, &TestFunctionMap, nullptr);
   // Set up a function to run on a worker thread. This function will post to the main thread to get some data.
   Loop.threadPool()->scheduleTask(COLnewClosure0(&WorkerThreadFunctionError, &Loop));
   COL_TRC("We're starting the loop...");
   Loop.start();
   COL_TRC("...the loop was shutdown and is done.");
}

struct myData{
   COLstring Records;
};

static void DataReader(const COLwebRequest& Request, myData* pMyData){
   COL_FUNCTION(DataReader);
   COLvar Result;
   Result["output"] = pMyData->Records;
   COLrespondSuccess(Request.Address, Result);
}

static void DataWriter(const COLwebRequest& Request, myData* pMyData){
   COL_FUNCTION(DataWriter);
   const COLvar& Input = Request.Data;
   COLvar Result;
   if (Input.exists("record")) pMyData->Records = Input["record"].asString();
   COLrespondSuccess(Request.Address);
}

static void ModifyMainThreadData(SCKloop* pLoop){
   COL_FUNCTION(ModifyMainThreadData);
   COL_TRC("On worker thread");
   COLvar Input, Result;
   COL_TRC("Read data from main thread data structure");
   bool CallResult = SCKcall("mt/data/get", Input, COLstring(), &Result, false);
   UNIT_ASSERT_EQUALS("Main Thread Data", Result["output"]);
   UNIT_ASSERT(CallResult);
   COL_TRC("Change the data in the main thread data structure");
   Input["record"] = "Set from worker thread";
   Result.reset();
   CallResult = SCKcall("mt/data/set", Input, COLstring(), &Result, false);
   UNIT_ASSERT(Result["success"]);
   UNIT_ASSERT(CallResult);
   COL_TRC("Read again to see the updated value");
   CallResult = SCKcall("mt/data/get", Input, COLstring(), &Result, false);
   UNIT_ASSERT_EQUALS("Set from worker thread", Result["output"]);
   UNIT_ASSERT(CallResult);
   pLoop->shutdown();
}

// This test shows modification of a main-thread owned data structure from a worker thread
static void testMainThreadDataModification(){
   COL_FUNCTION(testMainThreadDataModification);
   myData MainThreadData;
   MainThreadData.Records = "Main Thread Data";
   COLthreadPool ThreadPool;
   ThreadPool.start();
   SCKloop Loop(&ThreadPool);
   // Need a hashmap of registered functions to be run ON MAIN THREAD
   COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> > TestFunctionMap;
   TestFunctionMap.add("mt/data/get", COLnewClosure1(&DataReader, &MainThreadData));
   TestFunctionMap.add("mt/data/set", COLnewClosure1(&DataWriter, &MainThreadData));
   SCKdefineSingleton(&Loop, nullptr, &TestFunctionMap);
   // Set up a function to run on a worker thread. This function will post to the main thread to get some data.
   Loop.threadPool()->scheduleTask(COLnewClosure0(&ModifyMainThreadData, &Loop));
   COL_TRC("We're starting the loop...");
   Loop.start();
   COL_TRC("...the loop was shutdown and is done.");
   UNIT_ASSERT_EQUALS("Set from worker thread", MainThreadData.Records);
}

static void ModifyMainThreadDataSerial1(){
   COL_FUNCTION(ModifyMainThreadDataSerial1);
   COL_TRC("On worker thread");
   COLvar Input, Result;
   COL_TRC("Change the data in the main thread data structure");
   Input["record"] = "Set from worker thread 1";
   bool CallResult = SCKcall("mt/data/set", Input, COLstring(), &Result, false);
   UNIT_ASSERT(Result["success"]);
   UNIT_ASSERT(CallResult);
}

static void ModifyMainThreadDataSerial2(SCKloop* pLoop){
   COL_FUNCTION(ModifyMainThreadDataSerial2);
   COL_TRC("On worker thread");
   COLvar Input, Result;
   COL_TRC("Change the data in the main thread data structure");
   Input["record"] = "Set from worker thread 2";
   bool CallResult = SCKcall("mt/data/set", Input, COLstring(), &Result, false);
   UNIT_ASSERT(Result["success"]);
   UNIT_ASSERT(CallResult);
   pLoop->shutdown();
}

// This test ensures sequential updates to the main thread data structure are safe
static void testMainThreadDataSerialUpdate(){
   COL_FUNCTION(testMainThreadDataSerialUpdate);
   myData MainThreadData;
   MainThreadData.Records = "Main Thread Data";
   COLthreadPool ThreadPool;
   ThreadPool.start();
   SCKloop Loop(&ThreadPool);
   // Need a hashmap of registered functions to be run ON MAIN THREAD
   COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> > TestFunctionMap;
   TestFunctionMap.add("mt/data/get", COLnewClosure1(&DataReader, &MainThreadData));
   TestFunctionMap.add("mt/data/set", COLnewClosure1(&DataWriter, &MainThreadData));
   SCKdefineSingleton(&Loop, nullptr, &TestFunctionMap);
   // Set up a function to run on a worker thread. This function will post to the main thread to get some data.
   Loop.threadPool()->scheduleTask(COLnewClosure0(&ModifyMainThreadDataSerial1));
   Loop.threadPool()->scheduleTask(COLnewClosure0(&ModifyMainThreadDataSerial2, &Loop));
   COL_TRC("We're starting the loop...");
   Loop.start();
   COL_TRC("...the loop was shutdown and is done.");
   UNIT_ASSERT_EQUALS("Set from worker thread 2", MainThreadData.Records);
}

void testCallMain(UNITapp* pApp) {
   // pApp->add("call/basic",            &testCallBasic);
   // pApp->add("call/basic/error",      &testCallBasicError);
   // pApp->add("call/edit_data",        &testMainThreadDataModification);
   // pApp->add("call/edit_data/serial", &testMainThreadDataSerialUpdate);
}
