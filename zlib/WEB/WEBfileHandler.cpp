//---------------------------------------------------------------------------
// Copyright (C) 1997-2019 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: WEBfileHandler
//
// Author: John Qi
//
// Description:
//
// Implementation
//---------------------------------------------------------------------------

#include <WEB/WEBfileHandler.h>
#include <WEB/WEBrequestData.h>
#include <WEB/WEBresponse.h>
#include <WEBI/WEBImimeLookup.h>

#include <WEBI/WEBIindex.h>
#include <WEBI/WEBIindexFile.h>

#include <CACHE/CACHEfile.h>

#include <SCK/SCKloop.h>

#include <GZIP/GZIPutils.h>

#include <FIL/FILutils.h>
#include <FIL/FILpathUtils.h>

#include <COL/COLvar.h>
#include <COL/COLthreadPool.h>
#include <COL/COLclosure6.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;


bool WEBaboveRootAttackDetected(const COLstring& WebAssertDir, COLstring* pFilePath) {
   COL_FUNCTION(WEBaboveRootAttackDetected);
   COLstring& FilePath = *pFilePath;
   FilePath = FILpathResolve(FilePath);
   COL_VAR2(WebAssertDir, FilePath);
   return FilePath.find(WebAssertDir) == npos;
}

COLstring WEBrootDir(){
   COL_FUNCTION(WEBrootDir);
   COLstring Result = FILpathResolve("../web", FILexecutableDir()) + FIL_DIR_SEPARATOR;
   COL_VAR(Result);
   return Result;
}

static void WEBreturnOnMain(COLclosure1<const COLstring&>* pCallback, COLstring HttpResponse){
   COL_FUNCTION(WEBreturnOnMain);
   pCallback->runAndDelete(HttpResponse);
}

// WORKER THREAD
static void WEBfetchFile(SCKloop* pLoop, COLclosure1<const COLstring&>* pCallback, COLstring Location, COLstring Index, bool Compressed) {
   COL_FUNCTION(WEBfetchFile);
   if ( Location == "/" ) {
      // Not even possible to reach here currently.
      Location = "/" + Index;
   }
   COLstring RootDir = WEBrootDir();
   COL_VAR(Location);
   COLstring FilePath = RootDir + Location.substr(1);
   COL_VAR(FilePath);

   COLstring HttpResponse;
   if (WEBaboveRootAttackDetected(RootDir, &FilePath)){
      COL_TRC("Above root attack.");
      FilePath = RootDir + "/notfound.html";
   } else {
      if (!FILfileExists(FilePath) || FILisDirectory(FilePath) ) {
         COL_TRC("File not found or is directory.");
         FilePath = RootDir + "/notfound.html";
      }
   }
   try{
      COL_VAR(FilePath);
      COLstring Data;
      FILreadFile(FilePath, &Data);
      if (Location == "/" + Index){
         WEBIindexGetCssAndJs(RootDir, &Data);
      }
      COLstring Extension = FILpathExtLast(FilePath, false);
      COLstring MimeType = WEBImimeType(Extension);
      COL_VAR2(Extension, MimeType);
      if (Compressed) {
         COL_TRC("Compressed response");
         WEBresponseFileGzip(MimeType, Data, &HttpResponse);
      } else {
         WEBresponseFile(MimeType, Data, &HttpResponse);
      }
   } catch(const COLerror& Error){
      WEBresponseHttpError(500, "Failed to fetch file: " + Error.description(), &HttpResponse);
   }
   pLoop->post(COLnewClosure0(&WEBreturnOnMain, pCallback, HttpResponse));
}

static void WEBfetchCachedFile(COLstring Location, bool Compressed, const COLstring& Index, COLstring* pHttpResponse) {
   COL_FUNCTION(WEBfetchCachedFile);
   COL_VAR(Compressed);
   if(Location == "/") { Location = "/web/" + Index; }
   else                { Location = "/web" + Location; }
   COLstring FileBody;
   bool Exists = CACHEreadFile(Location, &FileBody);
   COL_VAR3(Location, Exists, FileBody.size());
   if(!Exists) { Location = "/web/notfound.html"; }
   COLstring MimeType  = WEBImimeType(FILpathExtLast(Location, false));

   if (Compressed) {
      COL_TRC("Compressed response");
      WEBresponseBodyHeaders(FileBody.size(), pHttpResponse);
      WEBheaderSetContentType(MimeType,pHttpResponse);
      *pHttpResponse += COL_T("Content-Encoding: gzip\r\n\r\n") + FileBody;
   } else {
      COLstring Uncompressed;
      GZIPuncompress(FileBody, &Uncompressed);
      WEBresponseBodyHeaders(Uncompressed.size(), pHttpResponse);
      WEBheaderSetContentType(MimeType,pHttpResponse);
      *pHttpResponse += COL_T("\r\n") + Uncompressed;
   }
}

void WEBfileResponseReal(const COLstring& Path, COLclosure1<const COLstring&>* pCallback, SCKloop* pLoop, bool Compressed, const COLstring& Index){
   COL_FUNCTION(WEBfileResponseReal);
   COL_VAR(Compressed);
   if(CACHEexists()) {
      COLstring Result;
      WEBfetchCachedFile(Path, Compressed, Index, &Result);
      pCallback->runAndDelete(Result);
      return;
   }
   pLoop->threadPool()->scheduleTask(COLnewClosure0(&WEBfetchFile, pLoop, pCallback, Path, Index, Compressed));
}

void WEBsendResponseBySocket(const COLstring& HttpResponse, COLuint64 ConnectionId){
   COL_FUNCTION(WEBsendResponseBySocket);
   WEBresponseSend(ConnectionId, HttpResponse);
}

bool WEBhasCompressionFormat(COLstring EncodingList, COLstring Format) {
   COL_FUNCTION(WEBhasCompressionFormat);
   COL_VAR2(EncodingList, Format);
   Format.toLowerCase();
   bool Success = false;
   COLstring Algo;
   do {
      Success = EncodingList.split(Algo, EncodingList, ",");
      Algo.toLowerCase();
      if (Algo.strip(COLstring::Both) == Format) { return true; }
   } while (Success);
   COL_TRC("Not found");
   return false;
}

static bool WEBfileCompressable(const WEBrequestData& Data) {
   COL_FUNCTION(WEBfileCompressable);
   auto Result = Data.Headers.find("accept-encoding");
   if (Data.Headers.cend() == Result) { return false; }
   return WEBhasCompressionFormat(Result.value(), "gzip");
}

void WEBfileResponse(const WEBrequestData& Data, SCKloop* pLoop, const COLstring& Index){
   COL_FUNCTION(WEBfileResponse);
   bool Compressed = WEBfileCompressable(Data);
   WEBfileResponseReal(Data.Path, COLnewClosure1(&WEBsendResponseBySocket, Data.ConnectionId), pLoop, Compressed, Index);
}
