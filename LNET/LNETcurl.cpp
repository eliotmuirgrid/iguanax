//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LNETcurl
//
// Description:
//
// Implementation
//
// Author: Nasron Cheong
// Date:   Tuesday, March 8th, 2011 @ 11:37:32 AM
//
//---------------------------------------------------------------------------
#include "LNETcurl.h"
#include <NODL/NODLutils.h>

#include <new> // For placement new

#include <CURL/CURLutils.h>

#include <LUA/LUAutils.h>
#include <LUA/LUAtable.h>

#include <COL/COLauto.h>
#include <COL/COLdateTime.h>
#include <COL/COLhashmap.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

// The default cipher suites below were taken from the Intermediate backward compatibility section of Mozilla's server side TLS
// configuration recommendations v5.7 @ https://wiki.mozilla.org/Security/Server_Side_TLS

// TLS 1.2 cipher suites taken from Intermediate backward compatibility
const COLstring DBC_SSL_DEFAULT_CIPHERS = "ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-RSA-AES128-GCM-SHA256:\
ECDHE-ECDSA-AES256-GCM-SHA384:ECDHE-RSA-AES256-GCM-SHA384:ECDHE-ECDSA-CHACHA20-POLY1305:ECDHE-RSA-CHACHA20-POLY1305:\
DHE-RSA-AES128-GCM-SHA256:DHE-RSA-AES256-GCM-SHA384:DHE-RSA-CHACHA20-POLY1305";

// TLS 1.3 cipher suites Intermediate backward compatibility
const COLstring DBC_SSL_DEFAULT_CIPHER_SUITES = "TLS_AES_128_GCM_SHA256:TLS_AES_256_GCM_SHA384:TLS_CHACHA20_POLY1305_SHA256";

//These methods provide enum values used by cURL to set specific options
long LNETgetCurlAuthMethodFromColString(const COLstring& Method) {
   COL_FUNCTION(LNETgetCurlAuthMethodFromColString);
   COL_VAR(Method);
   if (Method == "basic")         return CURLAUTH_BASIC;
   if (Method == "digest")        return CURLAUTH_DIGEST;
   if (Method == "digest-ie")     return CURLAUTH_DIGEST_IE;
   if (Method == "gss-negotiate") return CURLAUTH_GSSNEGOTIATE;
   if (Method == "ntlm")          return CURLAUTH_NTLM;
   if (Method == "any")           return CURLAUTH_ANY;
   if (Method == "anysafe")       return CURLAUTH_ANYSAFE;
   COL_ERROR_STREAM("Unknown authentication method: " << Method, COLerror::PreCondition);
   return CURLAUTH_NONE;
}

long LNETgetCurlSslVersionFromColString(const COLstring& Version) {
   COL_FUNCTION(LNETgetCurlSslVersionFromColString);
   COL_VAR(Version);
   if (Version == "tls-v1"  ) return CURL_SSLVERSION_TLSv1;
   if (Version == "tls-v1.0") return CURL_SSLVERSION_TLSv1_0;
   if (Version == "tls-v1.1") return CURL_SSLVERSION_TLSv1_1;
   if (Version == "tls-v1.2") return CURL_SSLVERSION_TLSv1_2;
   if (Version == "tls-v1.3") return CURL_SSLVERSION_TLSv1_3;
   if (Version == "ssl-v2"  ) return CURL_SSLVERSION_SSLv2;
   if (Version == "ssl-v3"  ) return CURL_SSLVERSION_SSLv3;
   COL_ERROR_STREAM("Unknown SSL version: " << Version, COLerror::PreCondition);
   return CURL_SSLVERSION_DEFAULT;
}

long LNETgetCurlFtpSslAuthFromColString(const COLstring& Method) {
   COL_FUNCTION(LNETgetCurlFtpSslAuthFromColString);
   COL_VAR(Method);
   if (Method == "ssl") return CURLFTPAUTH_SSL;
   if (Method == "tls") return CURLFTPAUTH_TLS;
   COL_ERROR_STREAM("Unknown FTP SSL authentication method: " << Method, COLerror::PreCondition);
   return CURLFTPAUTH_DEFAULT;
}

long LNETgetCurlHttpVersionFromColString(const COLstring& Version) {
   COL_FUNCTION(LNETgetCurlHttpVersionFromColString);
   COL_VAR(Version);
   if (Version == "http-1.0")                return CURL_HTTP_VERSION_1_0;
   if (Version == "http-1.1")                return CURL_HTTP_VERSION_1_1;
   if (Version == "http-2")                  return CURL_HTTP_VERSION_2_0;
   if (Version == "http-2-tls")              return CURL_HTTP_VERSION_2TLS;
   if (Version == "http-2-prior-knowledge")  return CURL_HTTP_VERSION_2_PRIOR_KNOWLEDGE;
   COL_ERROR_STREAM("Unknown HTTP version: " << Version, COLerror::PreCondition);
   return CURL_HTTP_VERSION_NONE;
}

const COLstring LNET_CURL_PTR_CACHE = "CurlCache";
const COLstring LNET_CURL_PTR_CACHE_METATABLE = "CurlCacheMetatable";

typedef COLhashmap<COLstring, COLauto<LNETcurlCacheItem> > LNETcurlCache;

int LNETcurlVerboseMode(){
   const COLstring EnableDebug = ::getenv("LNET_CURL_VERBOSE");
   return (EnableDebug == "1" ? 1 : 0);
}

LNETcurlCacheItem::LNETcurlCacheItem() : m_pHandle(0){
   COL_METHOD(LNETcurlCacheItem::LNETcurlCacheItem);
   LNETcurlCacheItemTouch(this);
}

LNETcurlCacheItem::~LNETcurlCacheItem(){
   COL_METHOD(LNETcurlCacheItem::~LNETcurlCacheItem);
   LNETcurlCacheItemCleanup(this);
}

void LNETcurlCacheItemInit(LNETcurlCacheItem* pItem){
   COL_FUNCTION(LNETcurlCacheItemInit);
   LNETcurlCacheItemCleanup(pItem);
   pItem->m_pHandle = CURLeasyInit();
   LNETcurlCacheItemReset(pItem);
}

void LNETcurlCacheItemCleanup(LNETcurlCacheItem* pItem){
   COL_FUNCTION(LNETcurlCacheItemCleanup);
   if (pItem->m_pHandle) {
      curl_easy_cleanup(pItem->m_pHandle);
      pItem->m_pHandle = NULL;
   }
}

void LNETcurlCacheItemTouch(LNETcurlCacheItem* pItem){
   COL_FUNCTION(LNETcurlCacheItemTouch);
   COLcurrentTimeStamp(&pItem->m_LastAccessTimeStamp);
}

void LNETcurlCacheItemReset(LNETcurlCacheItem* pItem){
   COL_FUNCTION(LNETcurlCacheItemReset);
   COL_VAR(pItem->m_pHandle);

   if (pItem->m_pHandle){
      COL_TRC("Handle exists so we are resetting it.");
      curl_easy_reset(pItem->m_pHandle);
      // Can't check error as usual on the error buffer - The error description might not be placed in the buffer
      if (curl_easy_setopt(pItem->m_pHandle, CURLOPT_ERRORBUFFER, pItem->m_ErrorBuffer) != CURLE_OK) {
         COL_ERROR_STREAM("Error setting up the CURL error buffer.", COLerror::PreCondition);
      }
      LNETcurlCacheItemCheckError(pItem, curl_easy_setopt(pItem->m_pHandle, CURLOPT_TIMEOUT, LNET_CURL_DEFAULT_TIMEOUT));
      LNETcurlCacheItemCheckError(pItem, curl_easy_setopt(pItem->m_pHandle, CURLOPT_NOSIGNAL, 1));
      LNETcurlCacheItemCheckError(pItem, curl_easy_setopt(pItem->m_pHandle, CURLOPT_SSL_VERIFYHOST, 0));
      LNETcurlCacheItemCheckError(pItem, curl_easy_setopt(pItem->m_pHandle, CURLOPT_SSL_VERIFYPEER, 0));

      curl_easy_setopt(pItem->m_pHandle, CURLOPT_VERBOSE, LNETcurlVerboseMode());
   }
}

COLstring LNETcurlGetError(LNETcurlCacheItem* pItem, int Code){
   COL_FUNCTION(LNETcurlGetError);
   if ( strlen(pItem->m_ErrorBuffer) ) {
      COL_TRC("Error buffer is not empty, throw what's there...");
      return pItem->m_ErrorBuffer;
   } else {
      COL_TRC("Error buffer is empty, get cURL error string based on code...");
      return curl_easy_strerror( (CURLcode) Code);
   }
}

void LNETcurlCacheItemCheckError(LNETcurlCacheItem* pItem, int Code){
   COL_FUNCTION(LNETcurlCacheItemCheckError);
   if (Code == CURLE_OK) {
      return;
   }
   COLstring Error = LNETcurlGetError(pItem, Code);
   COL_ERROR_STREAM(Error, Code);
}

static LNETcurlCache* LNETgetCurlCache(lua_State* L){
   COL_FUNCTION(LNETgetCurlCache);
   const int StackTop = lua_gettop(L);
   LUApushString(L, LNET_CURL_PTR_CACHE);
   lua_gettable(L, LUA_REGISTRYINDEX);
   COLASSERT(!lua_isnil(L, -1));
   void* UserData = LUAcheckUdata(L, -1, LNET_CURL_PTR_CACHE_METATABLE.c_str(),
      "'curl ptr cache' expected");
   lua_pop(L, 1); // Remove the table from the stack
   COLASSERT(StackTop == lua_gettop(L));
   return (LNETcurlCache*)UserData;
}

void LNETtrimUrlForCache(const COLstring& Original, COLstring* pTrimmed) {
   COL_FUNCTION(LNETtrimUrlForCache);
   COLstring& Trimmed = *pTrimmed;
   size_t Offset = 0;
   if (Original.find("http://") != npos) {
      Offset = Original.find("http://");
   } else if (Original.find("https://") != npos) {
      Offset = Original.find("https://");
   }
   Trimmed = Original.substr(Offset, Original.find("/"));
   Trimmed = Trimmed.substr(0, Trimmed.find("?"));
}

LNETcurlCacheItem* LNETcurlGetCachedHandle(lua_State* L, const COLstring& InUrl, bool CacheByHost) {
   COL_FUNCTION (LNETcurlGetCachedHandle);
   LNETcurlCache* pCache = LNETgetCurlCache(L);
   COLASSERT(pCache);
   COLstring Url;
   if (CacheByHost) {
      LNETtrimUrlForCache(InUrl, &Url);
   } else {
      Url = InUrl.substr(0, InUrl.find("?"));
   }

   LNETcurlCacheItem* pItem = 0;
   COLhashmapPlace Place = pCache->find(Url) ;
   if (Place != 0){
      COL_TRC("Found cached connection.");
      pItem = ((*pCache)[Place]);
      LNETcurlCacheItemReset(pItem);
   } else {
      COL_TRC("Creating new curl cache item.");
      pItem = new LNETcurlCacheItem;
      pCache->add(Url, pItem);
      LNETcurlCacheItemInit(pItem);
   }
   LNETcurlCacheItemTouch(pItem);
   return pItem;
}

void LNETcreateCurlCache(lua_State* L) {
   COL_FUNCTION(LNETcreateCurlCache);
   const int StackTop = lua_gettop (L);
   LUApushString(L, LNET_CURL_PTR_CACHE);
   lua_gettable(L, LUA_REGISTRYINDEX);
   COLASSERT(lua_isnil(L, -1)); // curl cache should not be in the registry
   lua_pop (L, 1); // Swallow the nil table pointer
   LUApushString(L, LNET_CURL_PTR_CACHE);
   LUApushObject<LNETcurlCache>(L, LNET_CURL_PTR_CACHE_METATABLE.c_str());
   lua_settable(L, LUA_REGISTRYINDEX);
   COLASSERT(StackTop == lua_gettop (L))
}

void LNETcurlInactiveCleanup(lua_State* L){
   COL_FUNCTION(LNETcurlInactiveCleanup);
   const int MAX_CONNECTION_IDLE_TIME = 60; // In seconds
   COLtimeStamp CurrentTime;
   COLcurrentTimeStamp (&CurrentTime);

   LNETcurlCache* pCache = LNETgetCurlCache(L);
   if (!pCache) { return; }
   // COLPRECONDITION(pCache);
   LNETcurlCache::TPlace Place = pCache->first();
   while(Place){
      LNETcurlCache::TPlace NextPlace = pCache->next(Place);
      if (CurrentTime.tv_sec -  (*pCache)[Place]->m_LastAccessTimeStamp.tv_sec > MAX_CONNECTION_IDLE_TIME ){
         pCache->remove(Place);
      }
      Place = NextPlace;
   }
}

void LNETcurlAppendDebugLog(const CURLdebugLog& DebugLog, const COLerror& Error){
   COL_FUNCTION(LNETcurlAppendDebugLog);
   //append debug info if available
   if (DebugLog.tracing()){
      COLstring NewDesc(Error.description());
      COLostream Stream(NewDesc);

      Stream << newline << "--DEBUG OUTPUT--" << newline
         << DebugLog.Output << newline;
      COLerror NewError(Error);
      NewError.setDescription(NewDesc);
      throw NewError;
   }
   else{
      throw Error;
   }
}

void LNETcurlSetSslCipherSuiteList(lua_State* L, CURL* pCurl) {
   COL_FUNCTION(LNETcurlSetSslCipherSuiteList);
   if (LUAoptionalStringParamPushed(L, 1, "cipher_suite_list")) {
      COL_TRC("User set their own SSL cipher suite list.");
      curl_easy_setopt(pCurl, CURLOPT_TLS13_CIPHERS, lua_tostring(L,-1));
      lua_pop(L,1);
   }
   else {
      COL_TRC("Defaulting to iNTERFACEWARE recommended cipher suite list.");
      const COLstring& RecommendedCipherList = DBC_SSL_DEFAULT_CIPHER_SUITES;
      curl_easy_setopt(pCurl, CURLOPT_TLS13_CIPHERS, RecommendedCipherList.c_str());
   }
}

void LNETcurlSetSslCipherList(lua_State* L, CURL* pCurl) {
   COL_FUNCTION(LNETcurlSetSslCipherList);
   if (LUAoptionalStringParamPushed(L, 1, "cipher_list")) {
      COL_TRC("User set their own SSL cipher list.");
      curl_easy_setopt(pCurl, CURLOPT_SSL_CIPHER_LIST, lua_tostring(L,-1));
      lua_pop(L,1);
   }
   else {
      COL_TRC("Defaulting to iNTERFACEWARE recommended cipher list.");
      const COLstring& RecommendedCipherList = DBC_SSL_DEFAULT_CIPHERS;
      curl_easy_setopt(pCurl, CURLOPT_SSL_CIPHER_LIST, RecommendedCipherList.c_str());
   }
}

void LNETcurlSetSslCaBundle(lua_State* L, CURL* pCurl) {
   COL_FUNCTION(LNETcurlSetSslCaBundle);
   if (LUAoptionalStringParamPushed(L, 1, "ca_file")) {
      COL_TRC("User set their own ca bundle file.");
      curl_easy_setopt(pCurl, CURLOPT_CAINFO, lua_tostring(L,-1));
      lua_pop(L,1);
   }
   else {
      COL_TRC("Defaulting to iNTERFACEWARE ca-bundle.");
      COLstring DefaultCaPath = CURLdefaultCaBundlePath();
      curl_easy_setopt(pCurl, CURLOPT_CAINFO, DefaultCaPath.c_str());
   }
}

void LNETcurlSetSslVersion(lua_State* L, CURL* pCurl) {
   COL_FUNCTION(LNETcurlSetSslVersion);
   long SslVersion = CURL_SSLVERSION_TLSv1_2;   // TLS v1.2 or later

   if (LUAoptionalStringParamPushed(L, 1, "ssl_version")) {
      COL_TRC("Setting user specified SSL version.");
      const char* GivenVersion = lua_tostring(L, -1);
      SslVersion = LNETgetCurlSslVersionFromColString( COLstring(GivenVersion) );
      lua_pop(L, 1);
   }

   curl_easy_setopt(pCurl, CURLOPT_SSLVERSION, SslVersion);
}
