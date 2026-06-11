#ifndef __CURL_UTILS_H__
#define __CURL_UTILS_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CURLutils
//
// Description:
//
// Utility functions used with cURL wrapper library.
//
// Author: Kevin Senn
// Date:   Thursday, November 10th, 2005 @ 04:53:24 PM
//
//---------------------------------------------------------------------------

#include <curl/curl.h>
#include <COL/COLstring.h>


COLstring CURLdefaultCaBundlePath();

// Escape all special characters in a string, to format it for an HTTP request.
// These "special characters" include ?, =, and &.
COLstring CURLurlEscape(const COLstring& String);
// this function was left for compatibility with the old source code

// Initialize libcurl (and libssh2).  Call this exactly once, from main(),
// when no other threads exist; never call it elsewhere.
//
void CURLglobalInit(const COLstring& CaFilePath = "");

// Use CURLeasyInit() instead of curl_easy_init().  This will check that
// CURL is ready, before calling curl_easy_init().  It will also check the
// result of curl_easy_init() and throw an error instead of returning NULL.
//
CURL* CURLeasyInit();

// For recording CURL's CURLOPT_VERBOSE logging output to a string.  No
// logging is done until you call trace() on a CURL handle; that handle
// must remain valid until the CURLdebugLog instance is destroyed or
// you call trace() on another handle or NULL.
//
struct CURLdebugLog {
   CURLdebugLog()
      : m_pCurl(NULL)
      , m_LastInfo(CURLINFO_TEXT)
   { }
   ~CURLdebugLog() { trace(NULL); }
   void trace(CURL*);
   bool tracing() const { return m_pCurl != NULL; }
   COLstring Output;
private:
   static int debugCallback(CURL*, curl_infotype, char*, size_t, void*);
   CURL*         m_pCurl;
   curl_infotype m_LastInfo;
};

#endif // end of defensive include
