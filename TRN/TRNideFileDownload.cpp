// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNideFileDownload
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Tuesday 24 October 2023 - 07:51AM
// ---------------------------------------------------------------------------

#include <TRN/TRNideFileDownload.h>

#include <FIL/FILutils.h>
#include <CFG/CFGconfigApi.h>

#include <PRM/PRMcallbacks.h>

#include <ROL/ROLid.h>

#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void TRNideFileDownloadImp(const COLstring& ComponentId, bool Development, const COLstring& File, const COLstring& Name, const COLaddress& Address, COLvar& Result){
   COL_FUNCTION(TRNideFileDownloadImp);
   COLstring Path = CFGresolveRepoFile(ComponentId, File, Development);
   if (Path.is_null()){
      COLrespondError(Address, "Cannot open files above the root.");
      return;
   }
   COLstring Content;
   FILreadFile(Path, &Content);
   COLstring HttpResponse = COL_T("HTTP/1.1 200 OK\r\n");
   HttpResponse += COL_T("Content-Length: ") + COLintToString(Content.size()) + COL_T("\r\n");
   HttpResponse += COL_T("Content-Type: application/octet-stream\r\n");
   HttpResponse += COL_T("Content-Disposition: attachment; filename=\"") + Name + COL_T("\"\r\n");
   HttpResponse += COL_T("\r\n") + Content;
   COL_DUMP("Response:", HttpResponse.data(), HttpResponse.size());
   COL_TRC(HttpResponse.data());
   COLsendWebResponse(Address.ConnectionId, HttpResponse);
}

// /file/download
void TRNideFileDownload(const COLwebRequest& Request){
   COL_FUNCTION(TRNideFileDownload);
   const COLvar& Data = Request.Data;
   bool      Development = COLvarRequiredBool  (Data, "development");
   COLstring File        = COLvarRequiredString(Data, "file");
   COLstring Name        = COLvarRequiredString(Data, "name");
   COLstring ComponentId = COLvarRequiredString(Data, "component");
   ROLid Id = Development ? ROLid::EDIT_SCRIPT : ROLid::VIEW_SCRIPT;
   COLstring Error;
   if (!PRMcheckRoleAccess(Request.FromTranslator, Request.User, ComponentId, Id, &Error)){
      return COLrespondError(Request.Address, PRMsetError(Id));
   }

   COLvar Result;
   COL_VAR3(Development, File, ComponentId);
   TRNideFileDownloadImp(ComponentId, Development, File, Name, Request.Address, Result);
}