// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SCKcall
//
// Description:
//
// Implementation
//
// Author: Vismay Shah
// Date:   Thursday 30 March 2023 - 12:47PM
// ---------------------------------------------------------------------------
#include <SCK/SCKcall.h>
#include <SCK/SCKloop.h>

#include <COL/COLevent.h>
#include <COL/COLlog.h>
#include <COL/COLvar.h>
COL_LOG_MODULE;

// This is a singleton.
struct SCKsingleton {
   SCKloop*                                              pLoop            { nullptr };
   COLhashmap<COLstring, COLwebHandlerFunc>*             pWorkerThreadMap { nullptr };
   COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >* pMainThreadMap   { nullptr };
};
// We have a SINGLE GLOBAL instance of it
static SCKsingleton GlobalSCKsingleton;

void SCKdefineSingleton(SCKloop* pLoop, COLhashmap<COLstring, COLwebHandlerFunc>* pFuncMap, COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >* pDataMap){
   COL_FUNCTION(SCKdefineSingleton);
   GlobalSCKsingleton.pLoop            = pLoop;
   GlobalSCKsingleton.pWorkerThreadMap = pFuncMap;
   GlobalSCKsingleton.pMainThreadMap   = pDataMap;
}

struct SCKapiRunner{
   SCKloop*         pLoop           { nullptr };
   COLhashmap<COLstring, COLwebHandlerFunc>*             pWorkerThreadApi { nullptr };
   COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >* pMainThreadApi   { nullptr };
   COLevent         Event;
};

static SCKapiRunner* SCKcreateRunner(){
   COL_FUNCTION(SCKcreateRunner);
   SCKapiRunner* pRunner       = new SCKapiRunner();
   pRunner->pLoop              = GlobalSCKsingleton.pLoop;
   pRunner->pWorkerThreadApi   = GlobalSCKsingleton.pWorkerThreadMap;
   pRunner->pMainThreadApi     = GlobalSCKsingleton.pMainThreadMap;
   return pRunner;
}

static SCKapiRunner* SCKcreateRunner(SCKloop* pLoop, COLhashmap<COLstring, COLwebHandlerFunc>* pWorkerThreadApi, COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >* pMainThreadApi){
   COL_FUNCTION(SCKcreateRunner);
   SCKapiRunner* pRunner       = new SCKapiRunner();
   pRunner->pLoop              = pLoop;
   pRunner->pWorkerThreadApi   = pWorkerThreadApi;
   pRunner->pMainThreadApi     = pMainThreadApi;
   return pRunner;
}


static void SCKsetupInternalRequest(COLwebRequest& Request, const COLstring& CallPath, const COLvar& In, const COLstring& Agent, bool IsComponent, COLvar* pOut, COLevent* pEvent){
   COL_FUNCTION(SCKsetupInternalRequest);
   Request.Path = CallPath;
   Request.Data = In;
   Request.FromTranslator = IsComponent;
   Request.User = Agent;
   Request.Address.ConnectionId = 0; // internal request -- this shouldn't affect anything but it will make for cleaner tracing.
   Request.Address.pInternalResponseCallback = COLnewClosure1(&SCKcallInternalResponse, pOut, pEvent);
   // TODO - think about IsSecure, User arguments for COLwebRequest
}

static bool SCKcallSyncFunction(SCKapiRunner* pRunner, const COLstring& CallPath, const COLvar& In, const COLstring& Agent, bool IsComponent, COLvar* pOut){
   COL_FUNCTION(SCKcallSyncFunction);
   COLhashmapPlace i = pRunner->pWorkerThreadApi->find(CallPath);
   if (!i) {
      COL_TRC("Call path not found in sync map, or map not set up");
      return false;
   }
   COL_TRC("Setting up internal request");
   COLwebRequest Request;
   SCKsetupInternalRequest(Request, CallPath, In, Agent, IsComponent, pOut, &pRunner->Event);
   COL_TRC("Running worker api on current worker thread");
   try {
      (*pRunner->pWorkerThreadApi)[i](Request);
   } catch(const COLerror& Error) {
      COLrespondError(Request.Address, Error.description());
   }
   COL_TRC("Got response");
   return true;
}

static void SCKrunDataApiOnMain(COLclosure1<const COLwebRequest&>* pApi, COLwebRequest Request, COLvar* pOut, SCKapiRunner* pApiRunner){
   COL_FUNCTION(SCKrunDataApiOnMain);
   try {
      pApi->run(Request);
   } catch(const COLerror& Error) {
      COLrespondError(Request.Address, Error.description());
   }
}

static bool SCKcallDataApi(SCKapiRunner* pRunner, const COLstring& CallPath, const COLvar& In, const COLstring& Agent, bool IsComponent, COLvar* pOut){
   COL_FUNCTION(SCKcallDataApi);
   COLhashmapPlace i = pRunner->pMainThreadApi->find(CallPath);
   if (!i) {
      COL_TRC("Call path not found in data api map, or map not set up.");
      return false;
   }
   COL_TRC("Setting up internal request");
   COLwebRequest Request;
   SCKsetupInternalRequest(Request, CallPath, In, Agent, IsComponent, pOut, &pRunner->Event);
   COL_TRC("Posting to main and running...");
   pRunner->pLoop->post(COLnewClosure0(&SCKrunDataApiOnMain, (*pRunner->pMainThreadApi)[i].get(), Request, pOut, pRunner));
   COL_TRC("Blocking until function at " + CallPath + " is complete...");
   pRunner->Event.wait();
   COL_TRC("Got response");
   return true;
}

void SCKcallInternalResponse(const COLvar& FromApi, COLvar* pOut, COLevent* pEvent){
   COL_FUNCTION(SCKcallInternalResponse);
   *pOut = FromApi;
   pEvent->signal();
}

// This function should be called from a worker thread. It will call a main-thread api on the main thread,
// or call a worker-thread api on the current worker thread. Uses a COLvar in/out interface to provide and return data.
bool SCKcall(const COLstring& CallPath, const COLvar& In, const COLstring& Agent, COLvar* pOut, bool ComponentAgent){
   COL_FUNCTION(SCKcall);
   bool Success = false;
   SCKapiRunner* pApiRunner = SCKcreateRunner();
   if (pApiRunner->pMainThreadApi){
      Success = SCKcallSyncFunction(pApiRunner, CallPath, In, Agent, ComponentAgent, pOut);
   }
   if (!Success && pApiRunner->pMainThreadApi){
      Success = SCKcallDataApi(pApiRunner, CallPath, In, Agent, ComponentAgent, pOut);
   }
   // TODO: what if both maps have the same path? do we execute both functions or raise an error?
   delete pApiRunner;
   return Success;
}

// SCKcall form used by unit tests - for these we don't define Agent
bool SCKcall(const COLstring& CallPath, const COLvar& In, COLvar* pOut, COLhashmap<COLstring, COLwebHandlerFunc>* pWorkerThreadMap,
             COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >* pMainThreadMap, SCKloop* pLoop){
   COL_FUNCTION(SCKcall-UnitTest);
   bool Success = false;
   SCKapiRunner* pApiRunner = SCKcreateRunner(pLoop, pWorkerThreadMap, pMainThreadMap);
   if (pApiRunner->pMainThreadApi){
      Success = SCKcallSyncFunction(pApiRunner, CallPath, In, COLstring(), false, pOut);
   }
   if (!Success && pApiRunner->pMainThreadApi){
      Success = SCKcallDataApi(pApiRunner, CallPath, In, COLstring(), false, pOut);
   }
   // TODO: what if both maps have the same path? do we execute both functions or raise an error?
   delete pApiRunner;
   return Success;
}

void SCKlistApis(COLmap<COLstring, bool>* pOut){
   COL_FUNCTION(SCKlistApis);
   // COLmap for alphabetically sorted list of apis - the bool part doesn't matter
   COL_TRC("Getting main thread apis");
   for( auto i = GlobalSCKsingleton.pMainThreadMap->cbegin(); i != GlobalSCKsingleton.pMainThreadMap->cend(); i++){
      pOut->add(i->first, true);
   }
   COL_TRC("Getting worker thread apis");
   for( auto j = GlobalSCKsingleton.pWorkerThreadMap->cbegin(); j != GlobalSCKsingleton.pWorkerThreadMap->cend(); j++){
      pOut->add(j->first, true);
   }
}
