//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: COLweb
//
// Author: Matthew Sobkowski and Eliot Muir
//
// Implementation
//---------------------------------------------------------------------------

#include "COLweb.h"
#include "COLerror.h"
#include "COLvar.h"
#include "COLlog.h"
COL_LOG_MODULE;

static COLclosure2<COLuint64, const COLstring&>* pCOLwebResponse = NULL;
void COLwebSetResponseCallback(COLclosure2<COLuint64, const COLstring&>* pCallback){
   COL_FUNCTION(COLwebSetResponseCallback);
   pCOLwebResponse = pCallback;
}

void COLsendWebResponse(COLuint64 ConnectionId, const COLstring& Data){
   COL_FUNCTION(COLsendWebResponse);
   if (pCOLwebResponse){
      pCOLwebResponse->run(ConnectionId, Data);
   } else {
      COL_ERR("No web response set.");
   }
}

void COLsendInternalResponse(const COLaddress& Address, const COLvar& Data){
   COL_FUNCTION(COLsendInternalResponse);
   if (Address.pInternalResponseCallback){
      Address.pInternalResponseCallback->runAndDelete(Data);
   }
}

void COLwebJsonResponse(const COLvar& Data, COLstring* pHttpOut){
   COL_FUNCTION(COLwebJsonResponse);
   COLstring JsonData = Data.json();
   *pHttpOut = COL_T("HTTP/1.1 200 OK\r\n")
             + COL_T("Content-Type: application/json\r\n")                          
             + COL_T("Content-Length: ") + COLintToString(JsonData.size()) + COL_T("\r\n\r\n") + JsonData;
   COL_DUMP("Response:", pHttpOut->data(), pHttpOut->size());
   COL_DUMP("Json:", JsonData.data(), JsonData.size());
   COL_VAR(JsonData.size());
}

void COLrouteResponse(const COLvar& Result, const COLaddress& Address){
   COL_VAR(Address.ConnectionId);
   if (Address.pInternalResponseCallback != NULL){
      COL_TRC("This was an internal request, use callback instead of writing to the socket.");
      return COLsendInternalResponse(Address, Result);
   }
   COL_TRC("Write to the socket");
   COLstring HttpResponse;
   COLwebJsonResponse(Result, &HttpResponse);
   COLsendWebResponse(Address.ConnectionId, HttpResponse);
}

void COLrespondError(const COLaddress& Address, const COLstring& ErrorMessage, const COLvar& ErrorVar){
   COL_FUNCTION(COLrespondError);
   COLvar Result;
   Result["success"] = false;
   Result["error"]   = ErrorMessage;
   Result["data"]    = ErrorVar;
   COLrouteResponse(Result, Address);
}

void COLrespondError(const COLaddress& Address, const COLstring& Error){
   COL_FUNCTION(COLrespondError);
   COLvar Result;
   Result["success"] = false;
   Result["error"] = Error;
   COLrouteResponse(Result, Address);
}

void COLrespondSuccess(const COLaddress& Address){
   COL_FUNCTION(COLrespondSuccess);
   COLvar Result;
   Result["success"] = true;
   Result["data"].setMapType();
   COLrouteResponse(Result, Address);
}

void COLrespondSuccess(const COLaddress& Address, const COLvar& Data){
   COL_FUNCTION(COLrespondSuccess);
   COL_VAR(Address.ConnectionId);
   COLvar Result;
   Result["success"] = true;
   Result["data"]    = Data;
   COLrouteResponse(Result, Address);
}

COLostream& operator<<(COLostream& Stream, const COLwebRequest& Request){
   Stream << Request.Data.json(1) << " connection=" << Request.Address.ConnectionId;
   // TODO - the rest of the members
   return Stream;
}

