#ifndef __LNET_HTTP_CURL_H__
#define __LNET_HTTP_CURL_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2014 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LNEThttpCurl
//
// Description:
//
// LNEThttpCurlItem class - Used to perform HTTP requests using cURL
//
// The LNEThttpCurlItem uses cURL as the client for sending an HTTP request
// To get started with it, build up a COLvar with the request details,
// validate/process them, and execute
// Specifics of the request can be read as they are required by the caller
//    (e.g. message body, debug log, etc.)
//
// Author: Akshay Ganeshen
// Date:   Fri  7 Nov 2014 15:25:40 EST
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>
#include <COL/COLstring.h>
#include <LUA/LUAheader.h>
#include <curl/curl.h>

class COLerror;
class COLvar;
class LNETcurlCacheItem;
struct CURLdebugLog;

// This class holds all the information necessary to complete an HTTP request using cURL as a client
class LNEThttpCurlItem {
public:
   LNEThttpCurlItem(LNETcurlCacheItem* pCurl);
   // Build common components directly from valid HTTP parameters
   LNEThttpCurlItem(LNETcurlCacheItem* pCurl, const COLvar& Params);
   ~LNEThttpCurlItem();

   void setCurlCacheItem(LNETcurlCacheItem* pCurl) { m_pCurl = pCurl; } // NOT USED -- VISMAY

   // These macros interact with the cURL library directly, which has very little type safety
   // The underlying code essentially just accepts a void* since different options have different parameter types
   // The first argument to each of these macros is a pointer to a LNEThttpCurlItem class
   // When setting options, an error is thrown if the option or value is invalid
   // NOTE : This is not namespaced in NODL and it, in fact, behaves as a member function of LNEThttpCurlItem
#undef setCurlOption
#define setCurlOption(pHttp, CurlOption, Value) \
   do {  \
      LNETcurlCacheItemCheckError(pHttp->m_pCurl, curl_easy_setopt((pHttp)->m_pCurl->m_pHandle, (CurlOption), (Value))); \
   } while(0)
#undef setCurlOptionFromVar
#define setCurlOptionFromVar(pHttp, CurlOption, Var) \
   do {  \
      LNETcurlCacheItemCheckError(pHttp->m_pCurl, curl_easy_setopt((pHttp)->m_pCurl->m_pHandle, (CurlOption), (Var).str().c_str())); \
   } while(0)
#undef setCurlOptionIfExists
#define setCurlOptionIfExists(pHttp, CurlOption, Map, Param) \
   do {  \
      if ((Map).exists((Param))) {  \
         setCurlOptionFromVar(pHttp, CurlOption, (Map)[(Param)]);   \
      }  \
   } while(0)

   // String buffers to hold responses from cURL
   COLstring m_MessageBody, m_ResponseHeaders, m_Error;
   long m_ResponseCode;

   LNETcurlCacheItem* m_pCurl;
   curl_slist* m_pHeaders;

   // String buffers used for setting up an SSL context
   struct {
      COLstring m_Cert, m_Key, m_Ca;
   } m_SslData;

   // A debugging log for details on the cURL object
   CURLdebugLog* m_pDebugLog;

   // The cURL item requires holding onto some extra data for use in callbacks later on
   // TODO : If extra fields are added, set up a union here instead
   //    So far only PUT requires extra data, so no union necessary
   struct {
      // These are used to access the lua reader function
      lua_State* m_pL;
      int m_ReaderFunctionIndex;
      // This holds static data that is sent in place of a reader function
      // TODO (note) : This could be unioned with m_pL + reader index
      COLstring m_StaticPutData;
      // This field holds data that is waiting to be pushed out to cURL
      COLstring m_SendDataBuffer;
      int m_BufferedSendDataPosition;
      // cURL is pretty stupid about this:
      // Despite setting the size of a PUT explicitly, cURL will still keep asking for more data
      // It's up to us to monitor how much data we've sent, and tell cURL to stop when it's greedily asking for more and more data
      size_t m_BytesRemaining;
      bool m_IsKnownSize;
   } m_PutData;
   // This index refers to where the on-complete callback function is in thee lua stack
   struct {
      lua_State* m_pL;
      int m_OnCompleteFunctionIndex;
   } m_AsyncData;
};

void LNEThttpCurlReset(LNEThttpCurlItem* pItem);

// This is called via callback when cURL requests more data to send to the server
// Returns true if there is data for cURL to use, and false if it's not possible to supply more data
//    In the case of false, it may be useful to do something like abort the cURL request
bool LNEThttpCurlRequestSendData(LNEThttpCurlItem* pItem, size_t RequestSize, COLstring* pDest);
// Similarly, this seeks to a specific position in the sent data
// Returns true if the position could be set, and false if the position is invalid (i.e. never sent that much data to begin with)
//    In the case of false, it may be useful to do something like notify cURL that the seek failed
bool LNEThttpCurlRequestSeek(LNEThttpCurlItem* pItem, int SeekPosition);

// TODO : This can manually bind a reader function for PUT requests, but that's not good - Find another way
// (see comment in LNEThttpPut)
void LNEThttpCurlAttachReader(LNEThttpCurlItem* pItem, lua_State* L, int i);
// TODO : Similar to above, this binds a lua callback function that gets called with the lua return values as arguments
void LNEThttpCurlAttachOnCompleteCallback(LNEThttpCurlItem* pItem, lua_State* L, int i); // NOT USED -- VISMAY

/* After the cURL object is built, this will execute it */
void LNEThttpCurlExecuteRequest(lua_State* L, LNEThttpCurlItem* pItem);

void LNEThttpBuildCommonFromParams(LNEThttpCurlItem* pItem, const COLvar& Params);
/* These build the whole cURL object from the supplied parameters */
void LNEThttpBuildPOST  (LNEThttpCurlItem* pItem, const COLvar& Params);
void LNEThttpBuildGET   (LNEThttpCurlItem* pItem, const COLvar& Params);
void LNEThttpBuildPUT   (LNEThttpCurlItem* pItem, const COLvar& Params);
void LNEThttpBuildDELETE(LNEThttpCurlItem* pItem, const COLvar& Params);

#endif // end of defensive include
