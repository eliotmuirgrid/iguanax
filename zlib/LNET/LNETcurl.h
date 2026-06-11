#ifndef __LNET_CURL_H__
#define __LNET_CURL_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LNETcurl
//
// Description:
//
// LNETcurl class.
//
// Author: Nasron Cheong
// Date:   Tuesday, March 8th, 2011 @ 11:37:32 AM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>
#include <COL/COLdateTime.h>
#include <LUA/LUAheader.h>
#include <curl/curl.h>

#define LNET_CURL_DEFAULT_TIMEOUT_STR "15"
#define LNET_CURL_DEFAULT_TIMEOUT 15

extern const COLstring DBC_SSL_DEFAULT_CIPHERS;
extern const COLstring DBC_SSL_DEFAULT_CIPHER_SUITES;

class COLerror;
struct CURLdebugLog;

// These convert COLstring method/version names into cURL enums for use in setting options
long LNETgetCurlAuthMethodFromColString(const COLstring& Method);
long LNETgetCurlSslVersionFromColString(const COLstring& Version);
long LNETgetCurlFtpSslAuthFromColString(const COLstring& Method);
long LNETgetCurlHttpVersionFromColString(const COLstring& Version);

class LNETcurlStringList{
public:
   LNETcurlStringList() : pList(0){}
   ~LNETcurlStringList(){freeList();}
   void freeList(){
      if (pList){
         curl_slist_free_all(pList);
         pList = 0;
      }
   }
   curl_slist* pList;
};

class LNETcurlCacheItem{
public:
   LNETcurlCacheItem();
   ~LNETcurlCacheItem();
   CURL*        m_pHandle;
   COLtimeStamp m_LastAccessTimeStamp;
   char         m_ErrorBuffer[CURL_ERROR_SIZE];
private:
   LNETcurlCacheItem(const LNETcurlCacheItem&);
   LNETcurlCacheItem& operator=(const LNETcurlCacheItem&);
};


void LNETcurlCacheItemInit      (LNETcurlCacheItem* pItem);
void LNETcurlCacheItemCleanup   (LNETcurlCacheItem* pItem);
void LNETcurlCacheItemTouch     (LNETcurlCacheItem* pItem);
void LNETcurlCacheItemReset     (LNETcurlCacheItem* pItem);
// This one returns the error string
COLstring LNETcurlGetError      (LNETcurlCacheItem* pItem, int Code);
// This one throws the error string
void LNETcurlCacheItemCheckError(LNETcurlCacheItem* pItem, int Code);

// Standard behaviour is to use everything before a ? as the key,
// so this:
//     http://bret.net?querystring=good
// resolves to the same handle as this:
//     http://bret.net
//
// Setting CacheByHost to true allows more aggressive caching of
// connections, so this:
//    http://bret.net/extremely/good
// resolves to the same handle as this:
//    http://bret.net/not/all/that/good
LNETcurlCacheItem* LNETcurlGetCachedHandle(lua_State* L, const COLstring& Url, bool CacheByHost = false);

void LNETcreateCurlCache(lua_State* L);

void LNETcurlInactiveCleanup(lua_State* L);

int LNETcurlVerboseMode();

void LNETcurlAppendDebugLog(const CURLdebugLog& DebugLog, const COLerror& OrigError);

void LNETcurlSetSslCipherSuiteList(lua_State* L, CURL* pCurl);
void LNETcurlSetSslCipherList(lua_State* L, CURL* pCurl);
void LNETcurlSetSslCaBundle(lua_State* L, CURL* pCurl);
void LNETcurlSetSslVersion(lua_State* L, CURL* pCurl);

#endif // end of defensive include
