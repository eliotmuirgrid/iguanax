// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: WEBrequest
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Wednesday 15 March 2023 - 12:14PM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <SCKB/SCKBsslConnection.h>
#include <SCKB/SCKBsslContext.h>
#include <SFI/SFIbase64EncodeFilter.h>
#include <SFI/SFIuriEscapeFilter.h>
#include <WEB/WEBchunkEncodingParser.h>
#include <WEB/WEBrequest.h>
#include <WEB/WEBrequestParseHeaders.h>
#include <WEB/WEBresponse.h>
#include <WEB/WEBurl.h>
#include <WEB/WEButils.h>
#include <curl/curl.h>
COL_LOG_MODULE;

COLstring WEBbasicAuthHeader(const COLstring& Username, const COLstring& Password){
   COL_FUNCTION(WEBbasicAuthHeader);
   if (Username.is_null()){
      COL_TRC("No basic authentication.");
      return "";
   }
   // Unfortunately for long usernames and passwords our BASE64 code will put \r\n in the middle
   // (very helpful!)  So I strip that stuff out. Eliot See IX-2416
   COLstring AuthDetails = SFIbase64Encode(Username + ":" + Password);
   AuthDetails.stripAll('\n');
   AuthDetails.stripAll('\r');
   COLstring Result = "Authorization: Basic " + AuthDetails + COL_T("\r\n");
   COL_TRC("Basic auth header set up:" << newline << Result);
   return Result;
}

bool WEBformatRequest(const WEBrequest& Request, const COLstring& Host, int Port, COLstring* pResult){
   COL_FUNCTION(WEBformatRequest);
   for (auto i=Request.Headers.begin(); i != Request.Headers.end(); i++){
      *pResult += i->first + COL_T(": ") + i->second + COL_T("\r\n");
   } 
   *pResult +=  WEBbasicAuthHeader(Request.UserName, Request.Password);
   *pResult += "Host: " + Host + ':' + COLintToString(Port) + COL_T("\r\n");
   *pResult += COL_T("\r\n");
   return true;
}

void WEBformatVariables(const COLmap<COLstring, COLstring>& Map, COLstring* pOut){
   COL_FUNCTION(WEBformatVariables);
   for (auto i=Map.cbegin(); i != Map.cend(); i++){
      *pOut += SFIuriEscape(i->first) + '=' + SFIuriEscape(i->second) + '&';
   }
   pOut->setSize(pOut->size()-1); // Get rid of trailing '&'
}

void WEBformatHttpLine(const COLstring& Action, const COLstring& Path, const COLmap<COLstring, COLstring>& GetVars, COLstring* pData){
   COL_FUNCTION(WEBformatHttpLine);
   *pData += Action + ' ' + Path;
   if (GetVars.size()  > 0){
      (*pData) += '?';
      WEBformatVariables(GetVars, pData);
   }
   *pData += COL_T(" HTTP/1.1\r\n");
}

void WEBformatGetRequest(const WEBrequest& Request, const WEBurl& Url, COLstring* pData){
   COL_FUNCTION(WEBformatGetRequest);
   WEBformatHttpLine("GET", Url.Path, Request.GetVariables, pData);
   WEBformatRequest(Request, Url.Host, Url.Port, pData);
}

void WEBformatDeleteRequest(const WEBrequest& Request, const WEBurl& Url, COLstring* pData){
   COL_FUNCTION(WEBformatDeleteRequest);
   WEBformatHttpLine("DELETE", Url.Path, Request.GetVariables, pData);
   WEBformatRequest(Request, Url.Host, Url.Port, pData);
}

static bool WEBsetPostVariablesToBody(const WEBrequest& Request, COLstring* pOut){
   COL_FUNCTION(WEBsetPostVariablesToBody);
   if (Request.Body.size() > 0){
      COL_TRC("Can't specify both a body and POST variables");
      return false;
   }
   WEBformatVariables(Request.PostVariables, pOut);
   return true;
}

void WEBformatPostRequest(const WEBrequest& Request, const WEBurl& Url, COLstring* pData){
   COL_FUNCTION(WEBformatPostRequest);
   WEBformatHttpLine("POST", Url.Path, Request.GetVariables, pData);
   COLstring FormattedPostVars;
   // If given both POST variables and a body string, we keep the body string and discard the POST variables
   if (Request.Body.size() > 0){
      COL_TRC("POST request has a body.");
      (*pData) += "Content-Length: " + COLintToString(Request.Body.size()) + COL_T("\r\n"); 
      WEBformatRequest(Request, Url.Host, Url.Port, pData);
      *pData += Request.Body;
   } else if (WEBsetPostVariablesToBody(Request, &FormattedPostVars)){
      COL_TRC("No body found, checking for POST variables.");
      (*pData) += "Content-Length: " + COLintToString(FormattedPostVars.size()) + COL_T("\r\n");
      WEBformatRequest(Request, Url.Host, Url.Port, pData);
      *pData += FormattedPostVars;
   }
} 

char* WEBsafeStrStr(const char *haystack, const char *needle, size_t len) {
   //COL_FUNCTION(WEBsafeStrStr);  Too noisy.
   if (!needle || !*needle) {
       return (char *)haystack;
   }

   const char *end = haystack + len - strlen(needle);
   for (; haystack <= end; haystack++) {
       if (*haystack == *needle && strncmp(haystack, needle, strlen(needle)) == 0) {
           return (char *)haystack;
       }
   }
   return NULL;
}

int WEBparseHttpLine(const char* pData, int Size, int* pCode){
   COL_FUNCTION(WEBparseHttpLine);
   const char* pEnd = pData+Size;
   const char* pCursor = pData; 
   int& Code = *pCode; 
   do{
      pCursor++;
   } while (*pCursor != ' ' && (pCursor - pData) < Size );
   // found first space
   pCursor++;
   Code = 0;
   do{
      Code = Code * 10 + (*pCursor - '0');
      pCursor++;
      COL_VAR(Code);
   } while (*pCursor != ' ' && (pCursor - pData) < Size );
   pCursor = WEBsafeStrStr(pCursor, "\r\n", pEnd-pCursor);
   return pCursor - pData+2;
}

static bool WEBrequestParseData(const char* pData, int Size, WEBresponse* pResult){
   COL_FUNCTION(WEBrequestParseData);
   const char* pEnd = pData + Size;
   const char* pCursor = pData + WEBparseHttpLine(pData, Size, &pResult->ReturnCode);
   bool Result =  WEBrequestParseHeaders(pCursor, pEnd, &pResult->Headers);
   COL_VAR(Result);
   return Result;
}

bool WEBrequestReadHeaders(SCKBconnection* pConnection, WEBresponse* pResult){
   COL_FUNCTION(WEBrequestReadHeaders);
   COLstring Headers;
   COLstring ReadBuffer;
   ReadBuffer.setCapacity(10000);
   
   int HeaderSize;
   do {
      int AmountRead = pConnection->read(&ReadBuffer);
      COL_VAR(AmountRead);
      if (AmountRead == -1){
         COL_TRC("Read failed while reading headers.");
         return false;
      }
      Headers += ReadBuffer;
      COL_DUMP("Response", Headers.c_str(), Headers.size());
      HeaderSize = WEBresponseHeaderComplete(Headers);
   } while (HeaderSize == 0);
   COL_VAR(HeaderSize);
   if (!WEBrequestParseData(Headers.c_str(), HeaderSize+2, pResult)) { 
      COL_TRC("WEBrequestParse failed.");
      return false; 
   }
   pResult->Body = Headers.substr(HeaderSize+4);
   COL_VAR2(pResult->Body,pResult->Body.size());
   COL_TRC("Got the complete headers.");
   return true;
}

bool WEBrequestReadBody(SCKBconnection* pConnection, int ExpectedLength, COLstring* pBody){
   COL_FUNCTION(WEBrequestReadBody);
   COL_VAR(pBody->size());
   if (pBody->size() == ExpectedLength){
      COL_TRC("We already have our data.");
      return true;
   }
   COLstring Buffer;
   Buffer.setCapacity(ExpectedLength);  // fugly since I have to use COLstring with SCKB interface which gets written to end
   pBody->setCapacity(ExpectedLength);
   do{
      int AmountRead = pConnection->read(&Buffer);
      COL_VAR(AmountRead);
      if (-1 == AmountRead){
         COL_TRC("Socket error while reading the body.");
         return false;
      }
      *pBody += Buffer;
      COL_VAR(*pBody);
   } while(pBody->size() < ExpectedLength);
   return true;
}

bool WEBrequestSendRequest(SCKBconnection* pConnection, const COLstring& RequestData, int Port, const COLstring& Host){
   COL_FUNCTION(WEBrequestSendRequest);
   COL_TRC(RequestData);
   COL_DUMP("Request", RequestData.c_str(), RequestData.size());
   COL_TRC("Connect " << Host << ":" << Port);
   COL_TRC("Connected, write request...");
   int AmountSent = pConnection->write(RequestData);
   COL_VAR(AmountSent);
   if (AmountSent == -1){
      return false;
   } else {
      return true;
   }
}

static bool WEBrequestGetChunks(COLstring* pBuffer, COLstring* pBody){
   COL_FUNCTION(WEBrequestGetChunks);
   int ChunkSize = -1;
   do{
      ChunkSize = WEBfindNextChunk(*pBuffer, pBody);
      COL_TRC("Consumed chunk of size " << ChunkSize);
   } while(ChunkSize > 0);
   if (ChunkSize == 0){
      COL_TRC("We have the last zero chunk");
      return true;
   }
   COL_TRC("Need more data");
   return false;
}

static bool WEBrequestReadChunkedBody(SCKBconnection* pConnection, COLstring* pBody){
   COL_FUNCTION(WEBrequestReadChunkedBody);
   COL_VAR(pBody->size());
   // This is required because after parsing the headers, any extra data is put into pBody. We need to check if that extra data has the rest of the chunks we need.
   COLstring ActualBody;
   if (WEBrequestGetChunks(pBody, &ActualBody)){
      COL_TRC("We already have our data.");
      *pBody = ActualBody;
      return true;
   }
   COLstring Buffer;
   Buffer.setCapacity(512);
   while(true){
      int AmountRead = pConnection->read(&Buffer);
      COL_VAR(AmountRead);
      if (-1 == AmountRead){
         COL_TRC("Socket error while reading the body.");
         return false;
      }
      *pBody += Buffer;  // Add new data to existing body
      if(WEBrequestGetChunks(pBody, &ActualBody)) {  // Process combined data
         COL_TRC("Got all chunks:");
         break;
      }
      if(Buffer.size() == Buffer.capacity() - 1) {
         COL_TRC("Need to expand buffer");
         Buffer.setCapacity(Buffer.capacity() * 2);
      }
   }
   *pBody = ActualBody;
   return true;
}

void WEBrequestBlockingGetImp(SCKBconnection* pConnection, const WEBurl& Url, const WEBrequest& Request, const COLstring& RequestData, WEBresponse* pResult){
   COL_FUNCTION(WEBrequestBlockingGetImp);

   pConnection->setTimeout(Request.Timeout);
   bool Connected = pConnection->connect(Url.Host, Url.Port);
   if (!Connected){
      COL_TRC("Unable to connect.");
      pResult->Body = "Unable to connect to: " + Url.Host;
      pResult->ReturnCode = 503;
      return;
   }
   if (!WEBrequestSendRequest(pConnection, RequestData, Url.Port, Url.Host)){
      COL_TRC("Unable to send request.");
      pResult->Body = "Unable to send request.";
      pResult->ReturnCode = 503;
      return;     
   }
   COL_TRC("Read response...");
   if (!WEBrequestReadHeaders(pConnection, pResult)){
      COL_TRC("Unable to read headers of response.");
      pResult->Body = "Unable to read headers.";
      pResult->ReturnCode = 503;
      return;         
   }
   if (pResult->Headers.count("content-length") > 0){
      int ContentLength = atoi(pResult->Headers["content-length"].c_str());
      COL_VAR(ContentLength);
      if (!WEBrequestReadBody(pConnection, ContentLength, &pResult->Body)){
         COL_TRC("Unable to read body of response.");
         pResult->Body = "Unable to read body of response.";
         pResult->ReturnCode = 503;
      }
   }
   if (pResult->Headers.count("transfer-encoding")){
      if (pResult->Headers["transfer-encoding"] == "chunked"){
         // COL_ERR("Opps Eliot hasn't implemented transfer-encoding yet.  It should be easy to do but not done yet.");
         COL_TRC("We have chunked transfer encoding.");
         if (!WEBrequestReadChunkedBody(pConnection, &pResult->Body)){
            COL_TRC("Unable to read body of chunked response.");
            pResult->Body = "Unable to read body of chunked response.";
            pResult->ReturnCode = 503;
         }
      }
   }
   pConnection->close();
   COL_TRC("Closed connection.");
}

void WEBimplementRequest(const WEBurl& Url, const WEBrequest& Request, const COLstring& RequestData, WEBresponse* pResult){
   COL_FUNCTION(WEBimplementRequest);
   if (Url.IsHTTPS){
      SCKBsslContext Context;
      SCKBsslConnection Connection(&Context);
      WEBrequestBlockingGetImp(&Connection, Url, Request, RequestData, pResult);   
   } else {
      SCKBconnection Connection;
      WEBrequestBlockingGetImp(&Connection, Url, Request, RequestData, pResult);   
   }
}

void WEBrequestRedirect(const WEBrequest& Request, WEBresponse* pResult, const COLstring& Method) {
   COL_FUNCTION(WEBrequestRedirect);
   COLstring RedirectUrl;
   WEBheaderGetValue(pResult->Headers, "Location", &RedirectUrl);
   COL_TRC("Received redirect request -- making new connection to " << RedirectUrl);
   if(RedirectUrl.equals(Request.Url)) {
      pResult->Body = "Infinite redirect loop detected";
      pResult->ReturnCode = 500;  // returning  internal server error in this case
      return;
   }
   pResult->Body.clear();
   pResult->Headers.clear();
   WEBrequest RedirectReq = Request;
   RedirectReq.Url = RedirectUrl;
   if (Method == "DELETE")    { WEBrequestBlockingDelete(RedirectReq, pResult); }
   else if (Method == "POST") { WEBrequestBlockingPost  (RedirectReq, pResult); }
   else                       { WEBrequestBlockingGet   (RedirectReq, pResult); }
}

// parses body as we get it
static size_t WEBcurlWriteCallback(void* Contents, size_t Size, size_t Nmemb, void* Userp) {
   COL_FUNCTION(WEBcurlWriteCallback);
   const size_t RealSize = Size * Nmemb;
   const auto   pBody     = (COLstring*)Userp;
   pBody->append((const char*)Contents, RealSize);
   return RealSize;
}

// parses headers as we get them
static size_t WEBcurlHeaderCallback(const void* Contents, const size_t Size, const size_t Nmemb, void* Userp) {
   COL_FUNCTION(WEBcurlHeaderCallback);
   const size_t    RealSize = Size * Nmemb;
   const auto      Headers  = (COLmap<COLstring, COLstring>*)Userp;
   const COLstring Header((const char*)Contents, RealSize);

   int ColonPos;
   if(Header.find(&ColonPos, ":")) {
      COLstring       Key   = COLstripWhiteSpace(Header.substr(0, ColonPos));
      const COLstring Value = COLstripWhiteSpace(Header.substr(ColonPos + 1));
      Key                   = COLlowerCase(Key);
      (*Headers)[Key]       = Value;
   }
   return RealSize;
}

static void WEBrequestBlockingWrapperCurl(const WEBrequest& Request, WEBresponse* pResult, const COLstring& Method) {
   COL_FUNCTION(WEBrequestBlockingWrapperCurl);
   WEBurl      Url;
   WEBresponse BadUrlResponse;
   if(!WEBtryUrlParse(&Url, Request.Url, &BadUrlResponse)) {
      COL_TRC("Couldn't parse url - return");
      *pResult = BadUrlResponse;
      return;
   }

   CURL* Curl = curl_easy_init();
   if(!Curl) {
      COL_TRC("Failed to initialize curl");
      pResult->ReturnCode = 503;
      pResult->Body       = "Failed to initialize curl";
      return;
   }

   COLstring FullUrl = Request.Url;
   if(Request.GetVariables.size() > 0) {
      FullUrl += "?";
      WEBformatVariables(Request.GetVariables, &FullUrl);
   }

   curl_easy_setopt(Curl, CURLOPT_URL, FullUrl.c_str());
   COL_TRC("URL: " << FullUrl);

   if(Method == "DELETE") {
      curl_easy_setopt(Curl, CURLOPT_CUSTOMREQUEST, "DELETE");
      COL_TRC("Method: DELETE");
   } else if(Method == "POST") {
      curl_easy_setopt(Curl, CURLOPT_POST, 1L);
      COL_TRC("Method: POST");
      if(Request.Body.size() > 0) {
         COL_TRC("POST with body, size: " << Request.Body.size());
         curl_easy_setopt(Curl, CURLOPT_POSTFIELDS, Request.Body.c_str());
         curl_easy_setopt(Curl, CURLOPT_POSTFIELDSIZE, Request.Body.size());
      } else if(Request.PostVariables.size() > 0) {
         COL_TRC("POST with variables");
         COLstring PostData;
         WEBformatVariables(Request.PostVariables, &PostData);
         curl_easy_setopt(Curl, CURLOPT_COPYPOSTFIELDS, PostData.c_str());
      }
   } else {
      COL_TRC("Method: GET");
   }

   curl_slist* Headers = NULL;
   for(auto i = Request.Headers.begin(); i != Request.Headers.end(); i++) {
      COLstring Header = i->first + ": " + i->second;
      Headers          = curl_slist_append(Headers, Header.c_str());
      COL_TRC("Header: " << Header);
   }

   if(!Request.UserName.is_null()) {
      COL_TRC("Setting basic auth for user: " << Request.UserName);
      curl_easy_setopt(Curl, CURLOPT_USERNAME, Request.UserName.c_str());
      curl_easy_setopt(Curl, CURLOPT_PASSWORD, Request.Password.c_str());
      curl_easy_setopt(Curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
   }

   if(Headers) { curl_easy_setopt(Curl, CURLOPT_HTTPHEADER, Headers); }

   curl_easy_setopt(Curl, CURLOPT_TIMEOUT_MS, (long)Request.Timeout);
   COL_TRC("Timeout: " << Request.Timeout << "ms");

   curl_easy_setopt(Curl, CURLOPT_FOLLOWLOCATION, 1L);
   curl_easy_setopt(Curl, CURLOPT_MAXREDIRS, 10L);

   curl_easy_setopt(Curl, CURLOPT_WRITEFUNCTION, WEBcurlWriteCallback);
   curl_easy_setopt(Curl, CURLOPT_WRITEDATA, &pResult->Body);
   curl_easy_setopt(Curl, CURLOPT_HEADERFUNCTION, WEBcurlHeaderCallback);
   curl_easy_setopt(Curl, CURLOPT_HEADERDATA, &pResult->Headers);

   COL_TRC("Performing curl request...");
   CURLcode Res = curl_easy_perform(Curl);

   if(Res != CURLE_OK) {
      COL_TRC("Curl error: " << curl_easy_strerror(Res));
      pResult->ReturnCode = 503;
      pResult->Body       = COLstring("Curl error: ") + curl_easy_strerror(Res);
   } else {
      long ResponseCode;
      curl_easy_getinfo(Curl, CURLINFO_RESPONSE_CODE, &ResponseCode);
      pResult->ReturnCode = (int)ResponseCode;
      COL_TRC("Response code: " << pResult->ReturnCode);
      COL_TRC("Response body size: " << pResult->Body.size());
   }
   if(Headers) curl_slist_free_all(Headers);
   curl_easy_cleanup(Curl);
}

static void WEBrequestBlockingWrapper(const WEBrequest& Request, WEBresponse* pResult, const COLstring& Method){
   COL_FUNCTION(WEBrequestBlockingWrapper);
   if(getenv("USE_CURL") != NULL) { return WEBrequestBlockingWrapperCurl(Request, pResult, Method); }
   WEBurl Url;
   WEBresponse BadUrlResponse;
   if (!WEBtryUrlParse(&Url, Request.Url, &BadUrlResponse)){
      COL_TRC("Couldn't parse url - return");
      return;
   }
   COLstring RequestData;
   if (Method == "DELETE")    { WEBformatDeleteRequest(Request, Url, &RequestData); }
   else if (Method == "POST") { WEBformatPostRequest  (Request, Url, &RequestData); }
   else                       { WEBformatGetRequest   (Request, Url, &RequestData); }
   WEBimplementRequest(Url, Request, RequestData, pResult);
   if(WEBisRedirectCode(pResult->ReturnCode)) { WEBrequestRedirect(Request, pResult, Method); }
}

// Does a blocking web request
void WEBrequestBlockingGet(const WEBrequest& Request, WEBresponse* pResult){
   COL_FUNCTION(WEBrequestBlockingGet);
   WEBrequestBlockingWrapper(Request, pResult,COL_T("GET"));
}

void WEBrequestBlockingDelete(const WEBrequest& Request, WEBresponse* pResult){
   COL_FUNCTION(WEBrequestBlockingDelete);  
   WEBrequestBlockingWrapper(Request, pResult,COL_T("DELETE"));
}

// Does a blocking web request
void WEBrequestBlockingPost(const WEBrequest& Request, WEBresponse* pResult){
   COL_FUNCTION(WEBrequestBlockingPost);  
   WEBrequestBlockingWrapper(Request, pResult,COL_T("POST"));
}

int WEBresponseHeaderComplete(const COLstring& Buffer){
   COL_FUNCTION(WEBresponseHeaderComplete);
   int FoundPosition = 0;
   if (Buffer.find(&FoundPosition, "\r\n\r\n")){
      COL_TRC("Response buffer is complete.");
      COL_VAR(FoundPosition);
      return FoundPosition;
   }
   COL_TRC("Response buffer is complete.");
   return 0;
}

COLostream& operator<<(COLostream& Stream, const WEBrequest& Request){
   Stream << "WEB Request url = " << Request.Url << newline
          << "  Username = " << Request.UserName << newline
          << "  Password = " << Request.Password << newline;
   Stream << "  Headers count = " << Request.Headers.size() << newline;
   for (auto i = Request.Headers.cbegin(); i != Request.Headers.cend(); i++){
      Stream << "  H: " << i->first << " : " << i->second << newline; 
   } 
   Stream << "  GET vars = " << Request.GetVariables.size() << newline;
   for (auto i = Request.GetVariables.cbegin(); i != Request.GetVariables.cend(); i++){
      Stream << "  V: " << i->first << " : " << i->second << newline; 
   } 
   Stream << "  POST vars = " << Request.PostVariables.size() << newline;
   for (auto i = Request.PostVariables.cbegin(); i != Request.PostVariables.cend(); i++){
      Stream << "  V: " << i->first << " : " << i->second << newline; 
   }  
   Stream << "  Body = " << Request.Body << newline;
   return Stream;
}
