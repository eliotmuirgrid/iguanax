// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBAdownload
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Friday 19 May 2023 - 10:18AM
// ---------------------------------------------------------------------------

#include <SDBU/SDBUmessageExtract.h>
#include <SDBU/SDBUutils.h>
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
#include <SDBA/SDBAdownload.h>
COL_LOG_MODULE;

static void SDBAdownloadImpl(const COLstring& Component, int Index, COLuint64 Time, const COLaddress& Address){
   COL_FUNCTION(SDBAdownloadImpl);
   COLstring Message, Err;
   if (!SDBUextractMessage(Component, Index, Time, &Message, &Err)) {
      return COLrespondError(Address, Err);
   }
   COL_TRC("Got the message");
   COLstring HttpResponse = COL_T("HTTP/1.1 200 OK\r\n");
   HttpResponse += COL_T("Content-Length: ") + COLintToString(Message.size()) + COL_T("\r\n");
   HttpResponse += COL_T("Content-Type: application/octet-stream\r\n");
   HttpResponse += COL_T("Content-Disposition: attachment; filename=\"") + Component + "_" + COLintToString(Time) + "_" + COLintToString(Index) + COL_T(".txt\"\r\n");
   HttpResponse += COL_T("\r\n") + Message;
   COL_DUMP("Response:", HttpResponse.data(), HttpResponse.size());
   COL_TRC(HttpResponse.data());
   COLsendWebResponse(Address.ConnectionId, HttpResponse);
}

// /log/download
void SDBAdownload(const COLwebRequest& Request){
   COL_FUNCTION(SDBAdownload);
   COL_VAR(Request.Data);
   COL_VAR(Request.Address.ConnectionId);
   const COLvar& Data  = Request.Data;
   COLstring Component, Error;
   if(!SDBUcomponentId(COLvarGetString(Request.Data, "component", ""), &Component, &Error)){ 
      return COLrespondError(Request.Address, Error); 
   }
   if (!PRMcheckRoleAccess(Request.FromTranslator, Request.User, Component, ROLid::VIEW_LOGS, &Error)){
      return COLrespondError(Request.Address, PRMsetError(ROLid::VIEW_LOGS));
   }

   COLstring TimeString = COLvarGetString(Data, "time", "0");
   COLuint64 Time = atoll(TimeString.c_str());
   if(Time < 0) {
      return COLrespondError(Request.Address, "Time parameter must be positive");
   }
   int Index = COLvarGet(Data, "id", 0);
   SDBAdownloadImpl(Component, Index, Time, Request.Address);
}
