// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DOCdownload
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Monday 15 September 2025 - 08:33AM
// ---------------------------------------------------------------------------

#include <DOC/DOCdownload.h>
#include <DOC/DOCdir.h>
#include <DOC/DOChex.h>
#include <DOC/DOCserialize.h>
#include <DOC/DOCdocument.h>

#include <FIL/FILutils.h>

#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <COL/COLarray.h>
#include <COL/COLstring.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

COLstring GetMimeType(const COLstring &ext){
   static COLhashmap<COLstring, COLstring> MimeTypes = []{
      COLhashmap<COLstring, COLstring> m;
      m.add(COL_T("html"), COL_T("text/html"));
      m.add(COL_T("htm"), COL_T("text/html"));
      m.add(COL_T("pdf"), COL_T("application/pdf"));
      m.add(COL_T("txt"), COL_T("text/plain; charset=utf-8"));
      m.add(COL_T("csv"), COL_T("text/csv; charset=utf-8"));
      m.add(COL_T("json"), COL_T("application/json"));
      m.add(COL_T("xml"), COL_T("application/xml"));
      m.add(COL_T("png"), COL_T("image/png"));
      m.add(COL_T("jpg"), COL_T("image/jpeg"));
      m.add(COL_T("jpeg"), COL_T("image/jpeg"));
      m.add(COL_T("gif"), COL_T("image/gif"));
      m.add(COL_T("svg"), COL_T("image/svg+xml"));
      m.add(COL_T("webp"), COL_T("image/webp"));
      m.add(COL_T("mp4"), COL_T("video/mp4"));
      m.add(COL_T("webm"), COL_T("video/webm"));
      m.add(COL_T("ogg"), COL_T("video/ogg"));
      m.add(COL_T("mp3"), COL_T("audio/mpeg"));
      m.add(COL_T("wav"), COL_T("audio/wav"));
      return m;
   }();

   if (MimeTypes.count(ext))
   {
      return MimeTypes[ext];
   }
   return COL_T("application/octet-stream");
}

// Assumes only valid filenames are passed in.
void DOCdownloadImp(const COLaddress& Address, const COLstring& FileName, const COLstring& Content, const COLstring& ContentDispo, const COLstring& ContentType){
   COL_FUNCTION(DOCdownloadImp);
   COL_VAR(FileName);
   COL_VAR2(ContentDispo,ContentType);
   COLstring R = COL_T("HTTP/1.1 200 OK\r\n");
    R += COL_T("Content-Length: ") + COLintToString(Content.size()) + COL_T("\r\n");
    R += COL_T("Content-Type: ") + ContentType + COL_T("\r\n");
    R += COL_T("Content-Disposition: ") + ContentDispo + COL_T("; filename=\"") + FileName + COL_T("\"\r\n");
    R += COL_T("\r\n") + Content;
   COL_DUMP("Response:", R.data(), R.size());
   COLsendWebResponse(Address.ConnectionId, R);
}

void DOCsanitizeName(COLstring* pIn){
   pIn->replace(" ", "_");
   pIn->replace("\n", "");
   pIn->replace("\r", "");
   pIn->replace("\"", "");
   pIn->replace("\'", "");   
   // TODO - are there are other special characters.
}

void DOCdownload(const COLwebRequest& Request){
   COL_FUNCTION(DOCdownload);
   const COLvar& Data = Request.Data;
   COLstring CreatedHex= COLvarRequiredString(Data, "created");
   COLstring Id        = COLvarRequiredString(Data, "id");
   COLstring Component = COLvarRequiredString(Data, "component");
   DOCdocument Doc;
   DOCreadDoc(Component, Id, CreatedHex, &Doc);
   COL_VAR(Doc);
   COLstring Content;
   if (FILfileExists(DOCfileOriginalLegacy(Component, Doc))){ 
      FILreadFile(DOCfileOriginalLegacy(Component, Doc), &Content); 
   } else {
      FILreadFile(DOCfileOriginal      (Component, Doc), &Content);  
   }
   COLstring DownloadName = Doc.Title;
   DOCsanitizeName(&DownloadName);
   DownloadName += COLstring(".") + DOCextension(Doc.Source);
   
   COLstring ContentDispo = COL_T("attachment");
   COLstring ContentType = GetMimeType(Doc.Source);
   if (Request.Data["inline"]){ ContentDispo = COL_T("inline"); };

   COL_VAR2(Request.Data["inline"],ContentType);
   DOCdownloadImp(Request.Address,DownloadName,Content,ContentDispo,ContentType);
}