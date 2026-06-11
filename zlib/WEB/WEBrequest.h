#ifndef __WEB_REQUEST_H__
#define __WEB_REQUEST_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: WEBrequest
//
// Description:
//
// More type safe interface for implementing a web request.  I am initially implementing
// this for a blocking client request.
//
// Author: Eliot Muir 
// Date:   Wednesday 15 March 2023 - 12:14PM
// ---------------------------------------------------------------------------

#include <COL/COLmap.h>
#include <COL/COLstring.h>

class WEBresponse;
class WEBurl;

class WEBrequest{
public:
   COLstring Url;
   COLmap<COLstring, COLstring> Headers;
   COLmap<COLstring, COLstring> GetVariables;  // GET variables are supported.
   COLmap<COLstring, COLstring> PostVariables; // POST variables are supported.

   // If the Username is not null then request will include a basic HTTP authorization header
   COLstring UserName;
   COLstring Password;

   COLstring Body;  // Need this for post requests.

   int Timeout = 5000;  // Timeout in milliseconds.

   // TODO this interface lends itself to having a callback interface to stream a large file
};

COLostream& operator<<(COLostream& Stream, const WEBrequest& Request);

// These do blocking web requests
void WEBrequestBlockingGet(const WEBrequest& Request, WEBresponse* pResult);
void WEBrequestBlockingPost(const WEBrequest& Request, WEBresponse* pResult);
void WEBrequestBlockingDelete(const WEBrequest& Request, WEBresponse* pResult);

void WEBformatGetRequest (const WEBrequest& Request, const WEBurl& Url, COLstring* pData);
void WEBformatPostRequest(const WEBrequest& Request, const WEBurl& Url, COLstring* pData);

// PRIVATE IMPLEMENTATION FUNCTIONS
bool WEBformatRequest(const WEBrequest& Request, COLstring* pResult);
bool WEBparseUrl(const COLstring& UrlIn, bool* pIsHTTPS, COLstring* pHost, COLstring* pPath, int* pPort);
// Returns 0 if not complete, the size of the header if it is complete.
int WEBresponseHeaderComplete(const COLstring& Buffer);

// Also used by WEBclient
bool WEBrequestParseHeaders(const char* pStart, const char* pEnd, COLmap<COLstring, COLstring>* pHeaders);
int WEBparseHttpLine(const char* pData, int Size, int* pCode);

char* WEBsafeStrStr(const char *haystack, const char *needle, size_t len); 

#endif // end of defensive include
