// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LNETrespond
//
// Description:
//
// Implementation
//
// Author: Vismay Shah
// Date:   Friday 27 October 2023 - 03:25PM
// ---------------------------------------------------------------------------

#include <LNET/LNETrespond.h>
#include <LNET/LNEThttp.h>

#include <BLUA/BLUAinstance.h>

#include <LUA/LUAcheck.h>
#include <LUA/LUAerror.h>
#include <LUA/LUAstartup.h>
#include <LUA/LUAtest.h>
#include <LUA/LUAutils.h>

#include <SCK/SCKconnection.h>
#include <SCK/SCKloop.h>

#include <WEB/WEBresponse.h>

#include <COL/COLclosure6.h>
#include <COL/COLevent.h>
#include <COL/COLvarUtils.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static const char* s_RespondOptions[] = {
   "body", "code", "headers", "entity_type", "use_gzip", "persist", "debug_result", "cache_connection_by_host", "connection", NULL
};

static void LNETsetupRespondOnMainThread(BLUAinstance2* pInstance, COLstring HttpResponse, int* pConnId, COLstring* pError, COLevent* pEvent) {
   COL_FUNCTION(LNETsetupRespondOnMainThread);
   int& ConnId = *pConnId;
   if (!pInstance->m_Connections.count(ConnId)) {
      COL_TRC("No longer have connection " << ConnId);
      *pError = "Connection is gone";
   } else {
      COL_TRC("Connection exists, writing response");
      pInstance->m_Connections[ConnId]->writeString(HttpResponse);
      ConnId = -1;
   }
   COL_TRC("Response done, signal Lua thread to continue");
   pEvent->signal();
}

// must be called from Lua worker thread
bool LNEThttpRespondAsync(BLUAinstance2* pInstance, const COLstring& HttpResponse, int* pConnId, COLstring* pError) {
   COL_FUNCTION(LNEThttpRespondAsync);
   pError->clear();
   COLevent Event;
   pInstance->m_pLoop->post(COLnewClosure0(&LNETsetupRespondOnMainThread, pInstance, HttpResponse, pConnId, pError, &Event));
   COL_TRC("Posting to main and running...");
   COL_TRC("Blocking until response is sent...");
   Event.wait();
   return pError->is_null();
}


static void LNETaddDefaultParams(COLvar* pParams){
   COL_FUNCTION(LNETaddDefaultParams);
   // Set up some default values
   COLsetDefault(pParams, "code", 200);
   COLsetDefaultMap(pParams, "headers");
   COLsetDefault(&((*pParams)["headers"]), "Cache-Control", "max-age=0");
   COLsetDefault(pParams, "debug_result", true);
}

static bool LNETcheckForSpecificHeader(const COLvar& Params, COLstring Header){
   COL_FUNCTION(LNETcheckForSpecificHeader-COLvar);
   COLarray<COLstring> Keys;
   COLvarKeys(Params, &Keys);
   for (int i =0; i < Keys.size(); i++){
      COLstring Check = Keys[i].toLowerCase();
      if (Check == Header.toLowerCase()) {
         COL_TRC("Headers contain " << Header);
         return true;
      }
   }
   return false;
}

static bool LNETcheckForSpecificHeader(const COLmap<COLstring, COLstring>& Map, COLstring Header){
   COL_FUNCTION(LNETcheckForSpecificHeader-COLmap);
   auto it = Map.cbegin();
   for (it; it!= Map.cend(); it++){
      COLstring Check = it->first;
      Check = Check.toLowerCase();
      if (Check == Header.toLowerCase()) {
         COL_TRC("Headers contain " << Header);
         return true;
      }
   }
   return false;
}

static void LNETcheckParams(COLvar* pParams, COLstring* pError){
   COL_FUNCTION(LNETcheckParams);
   try {
      // The common processing only applies to POST, GET, PUT, DELETE, so do the checks and processing manually here
      const COLvar& Argument = *pParams; // For better error messages from COLassert macros
      COLassertParamIsString(Argument, "body");
      // Checking for the existence of a header won't throw exceptions, so throw it here manually
      // NOTE : Error messages and error code were copied over from previous behaviour
      if (LNETcheckForSpecificHeader(Argument["headers"], "Content-Length")){
         *pError = "Response headers cannot contain the 'Content-Length' header, it is automatically calculated if required.";
         return;
      } else {
         (*pParams)["headers"]["Content-Length"] = Argument["body"].str().size();
      }
      if (!LNETcheckForSpecificHeader(Argument["headers"], "Content-Type")){
         if (Argument.exists("entity_type") && Argument["entity_type"].isString()){
            COL_TRC("Setting Content-Type header with entity_type for compatibility");
            (*pParams)["headers"]["Content-Type"] = Argument["entity_type"];
         } else {
            (*pParams)["headers"]["Content-Type"] = "text/html";
         }
      }
      COL_TRC("No overwrite of Content-Length detected.");
      LNETprocessHeaders(pParams);  // Turn the headers into a consistent form
      COLassertParamIsInt   (Argument, "code");
      COLassertParamIsBool  (Argument, "debug_result");
   } catch (const COLerror& Error) {
      *pError = Error.description();
   }
}

static void LNETprocessParams(COLvar* pParams, COLstring* pError){
   COL_FUNCTION(LNETprocessParams);
   if (pParams->isString()) {
      COL_TRC("Parameters was just a single string - process to allow a single argument to be considered the 'body' argument");
      COLvar ParamMap = COLvar().setMapType();
      ParamMap["body"] = pParams->str();
      ParamMap.swap(pParams);
   }
   COL_VAR(*pParams);
   COL_TRC("Adding some default values to the COLvar");
   LNETaddDefaultParams(pParams);
   COLstring UnknownParameterDetails;
   COL_TRC("Scanning for unknown arguments provided from Lua");
   if (!(LUAparamsAreKnown(*pParams, s_RespondOptions, &UnknownParameterDetails))) {
      *pError = "Parameter '" + UnknownParameterDetails + "' is not recognized.";
      return;
   }
   COL_TRC("Checking for invalid header overwrites and types. Also standardizes header format to array of COLstring");
   LNETcheckParams(pParams, pError);
}

static void LNETaddResponseHeadersFromColVar(WEBresponse* pResponse, const COLvar& Headers) {
   COL_FUNCTION(LNETaddResponseHeadersFromColVar);
   if (Headers.isArray()){
      COL_TRC("Got an array of strings for headers - adding them to the response");
      for (int i = 0; i < Headers.size(); i++) {
         COLstring Key, Value;
         LNETsplitHeader(Headers[i].str(), &Key, &Value);
         pResponse->Headers[Key] = Value;
      }
   }
}

static void LNETextractConnectionHeaders(COLvar& Headers, int* pConnectionId, int* pStreamId){
   COL_FUNCTION(LNETextractConnectionHeaders);
   if (!Headers.isMap()) { COL_TRC("Headers not a map - skip"); return; }
   if (Headers.exists("ifw_conn_id")) {
      *pConnectionId = Headers["ifw_conn_id"].asInt();
      Headers.erase("ifw_conn_id");
   }
   if (Headers.exists("ifw_stream_id")) {
      *pStreamId = Headers["ifw_stream_id"].asInt();
      Headers.erase("ifw_stream_id");
   }
}

static void LNETcreateHttpResponse(const COLvar& Params, COLstring* pOut) {
   COL_FUNCTION(LNETcreateHttpResponse);
   WEBresponse Response;
   Response.ReturnCode = Params["code"].asInt();
   Response.Body = Params["body"];
   LNETaddResponseHeadersFromColVar(&Response, Params["headers"]);
   bool HaveContentType = LNETcheckForSpecificHeader(Response.Headers, "Content-Type");
   if (!HaveContentType){
      if (Params.exists("entity_type") && Params["entity_type"].isString()){
         COL_TRC("Setting Content-Type header with entity_type for compatibility");
         Response.Headers["Content-Type"] = Params["entity_type"];
      } else {
         Response.Headers["Content-Type"] = "text/html";
      }
   }
   COL_VAR(Params.json(true));
   *pOut = WEBresponseSerialize(&Response);
}

static bool LNETgenerateOldParamsWarnings(const COLvar& Params, COLstring* pWarnings){
   COL_FUNCTION(LNETgenerateOldParamsWarnings);
   COLstring& Warnings = *pWarnings;
   Warnings = "Warning:";
   bool AnyWarnings = false;
   if (Params.exists("entity_type")){
      Warnings.append("\n- entity_type is supported but not recommended. Set the Content-Type header instead. If a Content-Type header is set, this option will be ignored.");
      AnyWarnings = true;
   }
   if (Params.exists("use_gzip")){
      Warnings.append("\n- use_gzip is no longer supported - this option will be ignored. ");
      AnyWarnings = true;
   }
   if (Params.exists("cache_connection_by_host")){
      Warnings.append("\n- cache_connection_by_host is no longer supported - this option will be ignored. ");
      AnyWarnings = true;
   }
   if (Params.exists("persist")){
      Warnings.append("\n- persist is no longer supported - this option will be ignored.");
      AnyWarnings = true;
   }
   return AnyWarnings;
}

int LNEThttpReturnResponse(lua_State* L, const COLvar& Params, const COLstring& HttpResponse){
   COL_FUNCTION(LNEThttpReturnResponse);
   COLstring Warnings;
   bool DebugResult = Params["debug_result"];
   bool WarningsPresent = LNETgenerateOldParamsWarnings(Params, &Warnings);
   int ReturnCount = 0;
   COL_VAR(DebugResult);
   if (DebugResult){
      COL_TRC("debug_result set to true, return the response generated to Lua (default behaviour).");
      LUApushString(L, HttpResponse);
   } else {
      COL_TRC("debug_result set to false, return an empty string to Lua.");
      LUApushString(L, COLstring());
   }
   ReturnCount++;
   if (WarningsPresent){
      COL_TRC("Some warnings present, send them to Lua");
      LUApushString(L, Warnings);
      ReturnCount++;
   }
   COL_VAR(ReturnCount);
   return ReturnCount;
}

int LNEThttpRespondErrorLua(lua_State* L, const COLstring& ErrorMessage){
   COL_FUNCTION(LNEThttpRespondErrorLua);
   COL_TRC("net.http.respond ran into an error: " << ErrorMessage);
   lua_pop(L, 2);  // pop pInstance and BLUAcurrentHttpConnection
   return LUAraiseLuaError(L, ErrorMessage);
}

static void LNEThttpRespondUpdateConnId(lua_State* L, int ConnId, int StreamId = 0){
   COL_FUNCTION(LNEThttpRespondUpdateConnId);
   COL_VAR(ConnId);
   lua_pushinteger(L, ConnId);
   lua_setfield(L, LUA_REGISTRYINDEX, "BLUAcurrentHttpConnection");
   lua_pushinteger(L, StreamId);
   lua_setfield(L, LUA_REGISTRYINDEX, "BLUAcurrentHttp2Stream");
}

// returns the connection id through <pConnectionId>
// returns false if responding to an HTTP1 request
// returns true if responding to an HTTP2 request and <pStreamId> is populated
static bool LNETisHttp2(lua_State* L, int* pConnectionId, int* pStreamId) {
   COL_FUNCTION(LNETisHttp2);
   lua_getfield(L, LUA_REGISTRYINDEX, "BLUAcurrentHttpConnection"); // This is set by the socket's onData event, see BLUAhandleHttpOnData, or by LNEThttpRespondUpdateConnId above.
   if (lua_isnil(L, -1)) { return LNEThttpRespondErrorLua(L, "No connection found. net.http.respond can only be used after a net.http.listen call."); }
   if (!LUAisInt(L, -1)) { return LNEThttpRespondErrorLua(L, "Connection id not an integer."); }
   *pConnectionId = lua_tointeger(L, -1);
   COL_VAR(*pConnectionId);
   if (*pConnectionId == -1) { return LNEThttpRespondErrorLua(L, "A response was already sent to the connection."); }

   lua_getfield(L, LUA_REGISTRYINDEX, "BLUAcurrentHttp2Stream"); // This is set by the socket's onData event, see BLUAhandleHttpOnData, or by LNEThttpRespondUpdateConnId above.
   if (lua_isnil(L, -1)) {
      COL_TRC("Stream Id not set, we have HTTP1");
      return false;
   }
   if (!LUAisInt(L, -1))  { return LNEThttpRespondErrorLua(L, "Stream id not an integer."); }
   *pStreamId = lua_tointeger(L, -1);
   if (0 == *pStreamId) {
      return false;
   }
   if (*pStreamId % 2 != 1) { return LNEThttpRespondErrorLua(L, "Responding to a server-initiated stream. Expected a client-initiated stream."); }

   return true;
}

// net.http.respond
int LNEThttpSendResponse(lua_State* L){
   COL_FUNCTION(LNEThttpSendResponse);
   if (LUALisEnvironmentStartup(L)) { return 0; }
   if (!lua_gettop(L)) { LUAraiseLuaError(L, "No arguments provided."); }
   else { LUAcheckArgCount(L, 1); }
   COLvar Params;
   Params.fromLua(L);
   int ConnId = -1, StreamId = -1;
   LNETextractConnectionHeaders(Params["headers"], &ConnId, &StreamId); // Do this before processParams turns the headers into an array
   COL_VAR(Params);
   COLstring Error, HttpResponse;
   LNETprocessParams(&Params, &Error);
   if (!Error.is_null()){ return LUAraiseLuaError(L, Error); }
   COL_TRC("Params are good, okay to form HTTP response");
   COL_VAR2(ConnId, StreamId);
   LNETcreateHttpResponse(Params, &HttpResponse);
   COL_TRC("Have the serialized HTTP response string.");
   COL_VAR(HttpResponse);
   if (LUALisEnvironmentTest(L)) {
      COL_TRC("In IDE mode, just push the response string unless debug_result is false");
      return LNEThttpReturnResponse(L, Params, HttpResponse);
   }
   BLUAinstance2* pInstance = BLUAinstanceGet(L);
   bool Http2 = false;
   if (ConnId == -1) {
      COL_TRC("No custom connection parameters, grab connection from Lua Registry");
      Http2 = LNETisHttp2(L, &ConnId, &StreamId);
   } else {
      Http2 = (StreamId != -1);
   }

   if (!Http2) {
      COL_TRC("Responding to connection " << ConnId);
      if(!LNEThttpRespondAsync(pInstance, HttpResponse, &ConnId, &Error)){ return LNEThttpRespondErrorLua(L, Error); }
      lua_pop(L, 3);    // pop pInstance and BLUAcurrentHttpConnection and nil
      COL_TRC("Responded successfully, now setting connection id to -1 to prevent multiple responses.");
      LNEThttpRespondUpdateConnId(L, ConnId);
      return LNEThttpReturnResponse(L, Params, HttpResponse);
   } else {
      COL_TRC("Responding to connection " << ConnId << " stream " << StreamId);
      LNEThttp2RespondAsync(pInstance, Params, &ConnId, &StreamId, &Error);
      lua_pop(L, 3);    // pop pInstance and BLUAcurrentHttpConnection and BLUAcurrentHttp2Stream
      COL_TRC("Responded successfully, now setting connection id to -1 and stream id to 0 to prevent multiple responses.");
      LNEThttpRespondUpdateConnId(L, ConnId, StreamId);
      return LNEThttpReturnResponse(L, Params, HttpResponse);
   }
}
