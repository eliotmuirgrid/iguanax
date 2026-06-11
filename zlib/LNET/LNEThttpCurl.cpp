//---------------------------------------------------------------------------
// Copyright (C) 1997-2014 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LNEThttpCurl
//
// Description:
//
// Implementation
//
// Author: Akshay Ganeshen
// Date:   Fri  7 Nov 2014 15:25:40 EST
//
//---------------------------------------------------------------------------
#include "LNEThttpCurl.h"
#include <NODL/NODLutils.h>
#include <LNET/LNETcurl.h>
#include <LNET/LNEThttp.h>
#include <LNET/LNEThttpDebug.h>
#include <curl/curl.h>

#include <CURL/CURLutils.h>

// TODO : Unfortunately, LNEThttpCurlItems need to hold onto a lua state to be able to call the 'reader' function
// If a better solution to that emerges, then these includes can be removed
#include <LUA/LUAerror.h>
#include <LUA/LUAtest.h>
#include <LUA/LUAutils.h>
#include <LUA/LUAvar.h>

#include <SCK/SCKloop.h>
#include <TRN/TRNannotationState.h>

#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <openssl/ssl.h>
#include <SFI/SFIuriEscapeFilter.h>


// This method converts a COLvar map (validated to be key-value pairs representing parameters) into a string suitable for use in the URL (for GET) or the body (for POST)
// Escaping is done on certain values so they are properly sent over the network
static void LNETvarToHttpParameters(const COLvar& Parameters, COLstring* pOutput) {
   COL_FUNCTION(LNETvarToHttpParameters);
   *pOutput = "";
   for (COLvar::COLvarMap::const_iterator it = Parameters.map().cbegin(); it != Parameters.map().cend(); it++) {
      COLstring EscapedKey = SFIuriEscape(it->first);
      COLstring EscapedValue = SFIuriEscape(it->second);
      *pOutput += EscapedKey + "=" + EscapedValue + "&";
   }
   if (!pOutput->is_null()) {
      // Remove the trailing &
      *pOutput = pOutput->strip(COLstring::Trailing, '&');
   }
   COL_VAR2(Parameters, *pOutput);
}

// Helper that splits a URL into 'Base', 'Query', and 'Hash' components
// The 'Base' section will hold the protocol, host, and request path
// The 'Query' section will hold any query parameters (including the '?')
// The 'Hash' section will hold any hash parameters (including the '#')
// This function tries to follow (RFC 3986 S4.1)
// e.g. http://somesite.com/hello?foo=bar&fizz=buzz#section=one
//      └────────  Base ────────┘└────  Query ────┘└── Hash ──┘
static void LNEThttpSplitUrlQueryHash(const COLstring& Url, COLstring* pBase, COLstring* pQuery, COLstring* pHash) {
   COL_FUNCTION(LNEThttpSplitUrlQueryHash);
   COLstring Base, Query, Hash;
   // First, find the position of the query string, and the hash parameters
   int QuestionMarkIndex = 0, HashIndex = 0;
   bool HasQueryParams = false, HasHashParams = false;
   if ((HasQueryParams = Url.find(&QuestionMarkIndex, '?'))) {
      COL_TRC("Found a question mark");
      if (QuestionMarkIndex == 0) {
         // edge-case: question mark at the start of the string - leave 'Base' empty
         COL_TRC("Question mark is at the front");
         Base.clear();
      } else {
         Base = Url.substr(0, QuestionMarkIndex);
      }
#ifdef COL_ENABLE_CONDITIONAL_LOGGING
      int AnotherQuestionMarkIndex;
      if (Url.find(&AnotherQuestionMarkIndex, '?', QuestionMarkIndex + 1)) {
         COL_TRC("That URL has more than one question mark... using first one");
         COL_VAR(AnotherQuestionMarkIndex);
      }
#endif
   } else {
      COL_TRC("No question mark");
      QuestionMarkIndex = 0;
      Query.clear();
      // 'Base' has not been initialized! Make note of that
   }
   if ((HasHashParams = Url.find(&HashIndex, '#', QuestionMarkIndex))) {
      COL_TRC("Found a hash");
      Hash = Url.substr(HashIndex);
   } else {
      COL_TRC("No hash");
      HashIndex = 0;
      Hash.clear();
   }
   COL_VAR2(HasQueryParams, HasHashParams);
   COL_VAR2(QuestionMarkIndex, HashIndex);
   // Second, extract the query string already in the URL, if one exists
   if (HasQueryParams) {
      if (HasHashParams) {
         COL_TRC("Has query string and hash parameters - extracting query string");
         COLASSERT(QuestionMarkIndex < HashIndex);
         int QueryLen = (HashIndex - QuestionMarkIndex);
         COL_VAR(QueryLen);
         Query = Url.substr(QuestionMarkIndex, QueryLen);
      } else {
         COL_TRC("Has query string and no hash parameters - query string is at the end");
         Query = Url.substr(QuestionMarkIndex);
      }
   } else {
      // Remember that 'Base' has not been initialized in this case - so do that now
      if (HasHashParams) {
         COL_TRC("Initializing base up to hash position");
         if (HashIndex == 0) {
            // edge-case: hash at the start of the string - leave 'Base' empty
            COL_TRC("Hash is at the front");
            Base.clear();
         } else {
            Base = Url.substr(0, HashIndex);
         }
      } else {
         COL_TRC("No query nor hash - base is the whole URL");
         Base = Url;
         Query.clear();
         Hash.clear();
      }
   }
   COL_VAR3(Base, Query, Hash);
   if (pBase)  {  Base.swap(pBase);  }
   if (pQuery) { Query.swap(pQuery); }
   if (pHash)  {  Hash.swap(pHash);  }
}

// Appends a query string (that can be generated by LNETvarToHttpParameters) to an existing
// URL to form a new URL - if the existing URL already includes a query string, then the
// supplied query string parameters are appended on to the URL with '&'
// e.g. LNEThttpAppendQueryString("http://myurl/abc", "foo=bar")
//      => "http://myurl/abc?foo=bar"
// e.g. LNEThttpAppendQueryString("http://myurl/abc?fizz=buzz", "foo=bar")
//      => "http://myurl/abc?fizz=buzz&foo=bar"
// The logic is based on RFC 3986 (section 4.1)
// http://tools.ietf.org/html/rfc3986#section-4.1
static void LNEThttpAppendQueryString(const COLstring& ExistingUrl, const COLstring& NewQueryString, COLstring* pResultUrl) {
   COL_FUNCTION(LNEThttpAppendQueryString);
   COL_VAR2(ExistingUrl, NewQueryString);
   COLstring& ResultUrl = *pResultUrl;
   if (NewQueryString.size() == 0) {
      COL_TRC("No query string provided... just returning original URL");
      ResultUrl = ExistingUrl;
      return;
   }

   // First, split the URL up so we can follow the RFC
   COLstring Base, Query, Hash;
   LNEThttpSplitUrlQueryHash(ExistingUrl, &Base, &Query, &Hash);

   // Second, figure out how to append onto 'Query', if it exists
   COLstring QueryAppend;
   COLstring QuerySeperatorCharacter;  // this either holds '', '?', or '&', to either use
                                       // an existing query string, or start a new one
   if (Query.size() == 0) {
      COL_TRC("Starting a new query string");
      QuerySeperatorCharacter = COL_T("?");
   } else {
      COLASSERT(Query[0] == '?');   // existing query string must start with '?'
      if (Query.size() > 1) {
         COL_TRC("Seems like query parameters already exist");
         QuerySeperatorCharacter = COL_T("&");
      } else {
         COL_TRC("No actual query parameters, but '?' exists - no need for another '?'");
         QuerySeperatorCharacter.clear();
      }
   }
   QueryAppend = QuerySeperatorCharacter + NewQueryString;
   COL_VAR2(QuerySeperatorCharacter, QueryAppend);

   // Finally, form the final URL
   ResultUrl = Base + Query + QueryAppend + Hash;
   COL_VAR(ResultUrl);
}


// Converts an int to a hex string - Needed for chunk encoding
// cURL likes to work in hex, so this helper converts ints to hex strings instead of base10 strings
static COLstring LNETuint32ToHexString(COLuint32 Value) {
   char Result[32];
#ifdef _WIN32
   return _ultoa((unsigned long)Value, Result, 16);
#else
   snprintf(Result, 32, "%lX", (unsigned long)Value);
   return Result;
#endif
}

// These are custom callback functions that are called by the cURL client when initializing or receiving data
// Comments for their usage are provided in the methods that build the cURL object
// The prototypes of the methods are specified by cURL already, and cannot be modified
typedef X509* (*d2i_X509_t) (X509**, const char**, long);   // Older OpenSSL versions declare the second param as unsigned char**, so use this instead
#define decodeX509  ((d2i_X509_t)d2i_X509)
static CURLcode LNETonCurlSslContextSetup(CURL* pCurl, void* vpSslContext, void* vpSslDataObject) {
   COL_FUNCTION(LNETonCurlSslContextSetup);
   // Extract the parameters from their void pointers
   // NOTE : Reinterpret cast here is acceptable because the original parameters are of the type we are casting to
   // vpSslCtx is always an object of type SSL_CTX*
   SSL_CTX* pSslContext = reinterpret_cast<SSL_CTX*>(vpSslContext);
   // The cURL object was built so that vpSslDataObject is a LNEThttpCurlItem* pointing to the original LNEThttpCurlItem object of the request
   LNEThttpCurlItem* pHttp = reinterpret_cast<LNEThttpCurlItem*>(vpSslDataObject);

   // These variables are set when building the ssl portion of the HTTP cURL item
   const COLstring& CertData = pHttp->m_SslData.m_Cert;
   const COLstring& KeyData = pHttp->m_SslData.m_Key;
   const COLstring& CaData = pHttp->m_SslData.m_Ca;
   COL_VAR3(pHttp->m_SslData.m_Cert, pHttp->m_SslData.m_Key, pHttp->m_SslData.m_Ca);

   try {
      // Each of the _stats fields are already validated, so we can safely assume they are all COLstrings (if set)
      if (!CertData.is_null()) {
         const char* pCertData = CertData.c_str();
         X509* Cert = decodeX509(NULL, &pCertData, CertData.size());
         if (!Cert) {
            throw COLerror("Invalid SSL __cert_data.");
         }
         int OK = SSL_CTX_use_certificate(pSslContext, Cert);
         X509_free(Cert);
         if (!OK) {
            throw COLerror("Failed to bind SSL __cert_data.");
         }
      }
      if (!KeyData.is_null()) {
         const unsigned char* pKeyData = (unsigned char*)KeyData.c_str();
         EVP_PKEY* Key = d2i_PrivateKey(EVP_PKEY_RSA, NULL, &pKeyData, KeyData.size());
         if (!Key) {
            throw COLerror("Invalid SSL __key_data.");
         }
         int OK = SSL_CTX_use_PrivateKey(pSslContext, Key);
         EVP_PKEY_free(Key);
         if (!OK) {
            throw COLerror("Failed to bind SSL __key_data.");
         }
      }
      if (!CertData.is_null() || !KeyData.is_null()) {
         if (!SSL_CTX_check_private_key(pSslContext)) {
            throw COLerror("SSL key/cert mismatch.");
         }
      }

      if (!CaData.is_null()) {
         const char* pCaData = CaData.c_str();
         X509* Cert = decodeX509(NULL, &pCaData, CaData.size());
         if (!Cert) {
            throw COLerror("Invalid SSL __ca_data.");
         }
         X509_STORE* Store = SSL_CTX_get_cert_store(pSslContext);
         int OK = Store && X509_STORE_add_cert(Store, Cert);
         X509_free(Cert);
         if (!OK) {
            throw COLerror("Failed to bind SSL __ca_data.");
         }
      }
      // Managed to do all custom SSL context behaviour, so let cURL know we're done here
      return CURLE_OK;
   } catch(const COLerror& Error) {
      // Attach the error details to the COLvar so it can be retrieved if desired
      pHttp->m_Error = Error.description();
   } catch(...) {
      pHttp->m_Error = "Unknown error during SSL context setup";
   }
   // This will cause cURL to abort and return this CURLcode for the overall request
   return CURLE_ABORTED_BY_CALLBACK;
}

// This is a callback that can be used by cURL when receiving response data
static size_t LNETonReceivedCurlData(void* pBuffer, size_t SizeOfItem, size_t NumberOfItem, void* vpResponseDataObject) {
   COL_FUNCTION(LNETonReceivedCurlData);
   size_t Size = SizeOfItem * NumberOfItem;
   // The cURL object is built so that the user data object is a reference to a COLstring buffer
   reinterpret_cast<COLstring*>(vpResponseDataObject)->append((const char *)pBuffer, Size);

#ifdef COL_ENABLE_CONDITIONAL_LOGGING
   COLstring IncomingData = COLstring((const char*)pBuffer, Size);
   COL_VAR2(IncomingData, Size);
#endif

   return Size;
}

// This is a callback that can be used by cURL when requesting data to send to a server
static size_t LNETonSendCurlData(void* pDest, size_t SizeOfItem, size_t NumberOfItem, void* vpSendDataObject) {
   COL_FUNCTION(LNETonSendCurlData);
   size_t Size = SizeOfItem * NumberOfItem;
   // The cURL object is built so that the user data object is a reference to the LNEThttpCurlItem that started the request
   LNEThttpCurlItem* pHttpCurlItem = reinterpret_cast<LNEThttpCurlItem*>(vpSendDataObject);
   COLstring SendData;
   if (!LNEThttpCurlRequestSendData(pHttpCurlItem, Size, &SendData)) {
      // Error will be set appropriately
      return CURL_READFUNC_ABORT;
   }
   // There are two considerations made for the request size:
   //    1. The user-supplied function may not have provided the requested size of data (so SendData < Size)
   //    2. The amount of data supplied may be more than cURL is ready to handle (so SendData > Size)
   // So explicitly recalculate the amount of data to send and let cURL know how much was actually provided
   Size = COL_MIN(Size, SendData.size());
   memcpy(pDest, SendData.c_str(), Size);

#ifdef COL_ENABLE_CONDITIONAL_LOGGING
   // This var is just used to produce a nicer name in the COL_VAR trace
   const COLstring& OutgoingData = SendData;
   COL_VAR(OutgoingData);
#endif

   return Size;
}

// This is a callback that can be used by cURL when requesting a seek on a stream
static int LNETonSeekCurlData(void* vpSeekObject, curl_off_t Offset, int Origin) {
   // The cURL object is built so that the user data object is a reference to the LNEThttpCurlItem that started the request
   LNEThttpCurlItem* pHttpCurlItem = reinterpret_cast<LNEThttpCurlItem*>(vpSeekObject);

   if (!LNEThttpCurlRequestSeek(pHttpCurlItem, (int)Offset)) {
      // TODO : It may be desirable some day to have this return 2 in certain cases
      //    2 means that the seek failed, but cURL should try to work around the issue
      return 1;
   }
   return 0;
}

LNEThttpCurlItem::LNEThttpCurlItem(LNETcurlCacheItem* pCurl) {
   COL_METHOD(LNEThttpCurlItem::LNEThttpCurlItem);
   // Explicitly set these pointers to NULL so a free is not attempted on random pointers
   m_pHeaders = NULL;
   m_pDebugLog = NULL;
   LNEThttpCurlReset(this);
   m_pCurl = pCurl;
}

LNEThttpCurlItem::LNEThttpCurlItem(LNETcurlCacheItem* pCurl, const COLvar& Params) {
   COL_METHOD(LNEThttpCurlItem::LNEThttpCurlItem-FromParams);
   // Explicitly set these pointers to NULL so a free is not attempted on random pointers
   m_pHeaders = NULL;
   m_pDebugLog = NULL;
   LNEThttpCurlReset(this);
   m_pCurl = pCurl;
   LNEThttpBuildCommonFromParams(this, Params);
}

LNEThttpCurlItem::~LNEThttpCurlItem() {
   COL_METHOD(LNEThttpCurlItem::~LNEThttpCurlItem);
   LNEThttpCurlReset(this);
}

void LNEThttpCurlReset(LNEThttpCurlItem* pItem) {
   COL_FUNCTION(LNEThttpCurlReset);
   // cURL pointers and lua_State pointers are not managed by the LNEThttpCurlItem, so just rid ourselves of the reference
   pItem->m_pCurl = NULL;
   pItem->m_PutData.m_pL = NULL;
   pItem->m_AsyncData.m_pL = NULL;

   if (pItem->m_pHeaders) {
      curl_slist_free_all(pItem->m_pHeaders);
      pItem->m_pHeaders = NULL;
   }

   if (pItem->m_pDebugLog) {
      delete pItem->m_pDebugLog;
      pItem->m_pDebugLog = NULL;
   }

   pItem->m_MessageBody.zero();
   pItem->m_ResponseHeaders.zero();
   pItem->m_Error.zero();
   pItem->m_ResponseCode = 0;
   // clear the PUT specific struct
   pItem->m_PutData.m_IsKnownSize = false;
   pItem->m_PutData.m_BytesRemaining = 0;
   pItem->m_PutData.m_StaticPutData.zero();
   pItem->m_PutData.m_ReaderFunctionIndex = -1;
   pItem->m_PutData.m_SendDataBuffer.zero();

   pItem->m_AsyncData.m_OnCompleteFunctionIndex = -1;
}

// This method is a big of a special case
// It's called from a callback by cURL when using PUT to fill the request data
// TODO : It has to interact with the lua state in order to call a reader function, which is mixing the cURL and lua/C layers
//    Would ideally like to separate them, perhaps by wrapping the lua function call in something more transparent
bool LNEThttpCurlRequestSendData(LNEThttpCurlItem* pItem, size_t RequestSize, COLstring* pDest) {
   COL_FUNCTION(LNEThttpCurlRequestSendData);
   COL_VAR(RequestSize);

   if (pItem->m_PutData.m_IsKnownSize) {
      COL_TRC("Size is known - using it to generate data");
      // cURL will keep asking for data even if we already filled out "size" bytes, so we use this test to stop the transfer
      if (!pItem->m_PutData.m_BytesRemaining) {
         COL_TRC("All the data is already sent... returning nothing");
         *pDest = "";
         return true;
      }
      // Emulate a request size up to a maximum of the remaining size of the transfer
      RequestSize = COL_MIN(RequestSize, pItem->m_PutData.m_BytesRemaining);
      COL_TRC("Changed RequestSize to " << RequestSize);
   } else {
      // Assume this is a Transfer-Encoding: chunked
      // Each block of data sent requires the number of bytes in front, so save some room in the buffer for that
      RequestSize -= 16;   // Arbitrary - there's room for 12 digits in there and the \r\n on either side of the data
                           // cURL won't send requests bigger than 16k (0x400), so there's tons of room to spare here
   }

   if (pItem->m_PutData.m_BufferedSendDataPosition == pItem->m_PutData.m_SendDataBuffer.size()) {
      COL_TRC("No data is buffered for sending, have to generate more");
      // No data is already prepared for sending, so have to generate more
      if (pItem->m_PutData.m_ReaderFunctionIndex > 0) {
         COL_TRC("Running callback function");
         // A lua function was set as the source of data, so call it and read the string on top of the stack
         bool RequestSucceeded = true;
         lua_State* L = pItem->m_PutData.m_pL;
         lua_pushvalue(L, pItem->m_PutData.m_ReaderFunctionIndex);
         lua_pushinteger(L, RequestSize);
         COLstring CurrentOutput;
         // Call the function with a single argument (requested size of return) and expect a single value to be returned
         if (lua_pcall(L, 1, 1, 0)) {
            // Error from the reader function itself
            pItem->m_Error = LUAtoString(L, -1);
            RequestSucceeded = false;
         } else if (!LUAextractString(L, -1, &CurrentOutput)) {
            pItem->m_Error = "Provided function did not return a string.";
            RequestSucceeded = false;
         } else if (pItem->m_PutData.m_SendDataBuffer.is_null()) {
            pItem->m_Error = "Provided function returned an empty string.";
            // This gives the user the benefit of the doubt, sending the empty string to cURL
            // (it's not considered an error unless cURL otherwise encounters issues, like timing out)
         }
         pItem->m_PutData.m_SendDataBuffer.append(CurrentOutput);
         // Error or not, there's something on the top of the stack
         lua_pop(L, 1);

         if (!RequestSucceeded) {
            COL_TRC("The callback failed - going to abort operation");
            return false;
         }
         COL_TRC("Successfully ran callback");
      } else {
         COL_TRC("Using static COLstring data");
         if (!pItem->m_PutData.m_StaticPutData.is_null()) {
            // Request size really doesn't matter - all this static data must be sent eventually
            // A bit of a hack: Set the send buffer to the value and clear the data to be sent
            pItem->m_PutData.m_SendDataBuffer.swap(&(pItem->m_PutData.m_StaticPutData));
         } else {
            // Already sent the data...
            pItem->m_Error = "Not enough data provided for the specified transfer size.";
            return false;
         }
      }
   } else {
      COL_TRC("Buffered data is available - going to use that");
   }

   // NOTE : Using 'shift' and 'clear' for string manipulation so that capacities remain the same
   //    It's likely that if data is requested again, it will be of roughly the same size as a previous call
   //    Also, cURL will never request more than 16k of data at a time, so this won't grow to an absurd size
   size_t BytesAvailable;
   int SendBufferSize = pItem->m_PutData.m_SendDataBuffer.size() - pItem->m_PutData.m_BufferedSendDataPosition;
   if (RequestSize < SendBufferSize) {
      // Can't send all the data at once, so have to save some data in the buffer
      BytesAvailable = RequestSize;
      *pDest = pItem->m_PutData.m_SendDataBuffer.substr(pItem->m_PutData.m_BufferedSendDataPosition, RequestSize);
      pItem->m_PutData.m_BufferedSendDataPosition += RequestSize;
      pItem->m_PutData.m_BytesRemaining -= RequestSize;
   } else {
      BytesAvailable = SendBufferSize;
      *pDest = pItem->m_PutData.m_SendDataBuffer.substr(pItem->m_PutData.m_BufferedSendDataPosition);
      pItem->m_PutData.m_BufferedSendDataPosition = pItem->m_PutData.m_SendDataBuffer.size();
      // Though the send buffer is empty, the transfer isn't necessarily done - A reader function can potentially buffer some more data
      pItem->m_PutData.m_BytesRemaining -= BytesAvailable;
   }

   if (!pItem->m_PutData.m_IsKnownSize) {
      // Chunk encoding requires the size to be prepended in hex
      *pDest = LNETuint32ToHexString(BytesAvailable) + "\r\n" + *pDest + "\r\n";
      if (BytesAvailable == 0) {
         // cURL will STILL KEEP ASKING... kind of absurd, but fine
         pItem->m_PutData.m_IsKnownSize = true;
         pItem->m_PutData.m_BytesRemaining = 0;
      }
   }
   return true;
}

// This method is used as a callback from cURL to seek forwards/backwards in the SendData during upload
bool LNEThttpCurlRequestSeek(LNEThttpCurlItem* pItem, int SeekPosition) {
   COL_FUNCTION(LNEThttpCurlRequestSeek);
   COL_VAR3(SeekPosition, pItem->m_PutData.m_SendDataBuffer.size(), pItem->m_PutData.m_SendDataBuffer);
   if (pItem->m_PutData.m_SendDataBuffer.size() <= SeekPosition) {
      // Can't seek past the data we have...
      return false;
   }
   // It's a valid position, so we can refer back to it when a requestSendData is triggered
   int BytesBacktracked = pItem->m_PutData.m_BufferedSendDataPosition - SeekPosition;
   pItem->m_PutData.m_BufferedSendDataPosition = SeekPosition;
   pItem->m_PutData.m_BytesRemaining += BytesBacktracked;
   return true;
}

void LNEThttpCurlAttachReader(LNEThttpCurlItem* pItem, lua_State* L, int i) {
   COL_FUNCTION(LNEThttpCurlAttachReader);
   pItem->m_PutData.m_pL = L;
   // Need absolute index for the function index
   if (i < 0) {
      i += lua_gettop(L) + i + 1;
   }
   pItem->m_PutData.m_ReaderFunctionIndex = i;
}

void LNEThttpCurlAttachOnCompleteCallback(LNEThttpCurlItem* pItem, lua_State* L, int i) {
   COL_FUNCTION(LNEThttpCurlAttachOnCompleteCallback);
   COL_ERROR_STREAM("Not done async callback yet.", COLerror::PreCondition);
   pItem->m_AsyncData.m_pL = L;
   // Need absolute index for the function index
   if (i < 0) {
      i += lua_gettop(L) + i + 1;
   }
   pItem->m_AsyncData.m_OnCompleteFunctionIndex = i;
}

static void LNETcurlCleanupMulti(LNEThttpCurlItem* pItem, CURLM* pMultiHandle){
   COL_FUNCTION(LNETcurlCleanupMulti);
   curl_multi_remove_handle(pMultiHandle, pItem->m_pCurl->m_pHandle); // Have to remove the curl handle, especially if we want to re-use it
   curl_multi_cleanup(pMultiHandle);   // Clean up the multi handle
}

static void LNETcheckRequestDone(lua_State* L, LNEThttpCurlItem* pItem, CURLM* pMultiHandle){
   COL_FUNCTION(LNETcheckRequestDone);
   int MessagesLeft;
   CURLMsg* pMessage = curl_multi_info_read(pMultiHandle, &MessagesLeft);
   if (pMessage && pMessage->msg == CURLMSG_DONE) { // CURLMSG_DONE means the transfer is complete
      if (pMessage->data.result == CURLE_OK) {
         COL_TRC("Request completed successfully.");
      }
      else                                   {
         COL_TRC("Request failed. Have to get error string via code from data.result");
         auto Code = pMessage->data.result;
         if (Code != CURLE_OK) {
            COLstring Error = LNETcurlGetError(pItem->m_pCurl, Code);
            LNETcurlCleanupMulti(pItem, pMultiHandle); // must remove the curl handle or suffer when trying to re-use this handle on the next run of the script
            LUAraiseLuaError(L, Error);
         }
      }
   }
}

static void LNETcurlGetResponseCode(LNEThttpCurlItem* pItem){
   COL_FUNCTION(LNETcurlGetResponseCode);
   CURLcode Result = curl_easy_getinfo(pItem->m_pCurl->m_pHandle, CURLINFO_RESPONSE_CODE, &(pItem->m_ResponseCode));
   LNETcurlCacheItemCheckError(pItem->m_pCurl, Result);
}

static void LNETcurlMulti(lua_State* L, LNEThttpCurlItem* pItem, TRNannotationState* pState){
   COL_FUNCTION(LNETcurlMulti);
   CURLM* pMultiHandle = curl_multi_init();
   curl_multi_add_handle(pMultiHandle, pItem->m_pCurl->m_pHandle);
   int StillRunning = 0;
   CURLMcode ReturnCode;
   // curl_multi_perform executes the request - any data is written to the pItem->m_MessageBody string
   COL_TRC("About to run multi_perform in loop");
   do {
      int NumFds;
      ReturnCode = curl_multi_perform(pMultiHandle, &StillRunning); // each call to curl_multi_perform reads/writes some data if its there
      if (ReturnCode == CURLM_OK) {
         ReturnCode = curl_multi_poll(pMultiHandle, NULL, 0, 50, &NumFds);
      }
      if (ReturnCode != CURLM_OK) {
         COL_TRC("curl_multi failed, code " << (int)ReturnCode);
         break;
      }
      if (pState->m_Cancelled) { // m_Cancelled would be set if we got another annotation request from the front end that wants to re-use this state
         COL_TRC("Interrupted - exit.");
         break;
      }
   } while (StillRunning);

   if(!pState->m_Cancelled){
      LNETcheckRequestDone(L, pItem, pMultiHandle);
      LNETcurlGetResponseCode(pItem);
   }
   LNETcurlCleanupMulti(pItem, pMultiHandle);
}

static void LNETcurlBlockingCall(LNEThttpCurlItem* pItem){
   COL_FUNCTION(LNETcurlBlockingCall);
   CURLcode Result;
   Result = curl_easy_perform(pItem->m_pCurl->m_pHandle);
   if (Result == CURLE_ABORTED_BY_CALLBACK) { COL_ERROR_STREAM(pItem->m_Error, COLerror::PreCondition); }
   else                                     { LNETcurlCacheItemCheckError(pItem->m_pCurl, Result);      }
   // else, request succeeded, so get the code
   LNETcurlGetResponseCode(pItem);
}

void LNEThttpCurlExecuteRequest(lua_State* L, LNEThttpCurlItem* pItem) {
   COL_FUNCTION(LNEThttpCurlExecuteRequest);
   if (pItem->m_AsyncData.m_OnCompleteFunctionIndex >= 0) {
      COL_ERROR_STREAM("Async support is not yet implemented.", COLerror::PreCondition);
   }
   if (LUALisEnvironmentTest(L)){
      COL_TRC("For translator, do interruptible curl call");
      TRNannotationState* pAnnotationState = TRNgetAnnotationState(L);
      LNETcurlMulti(L, pItem, pAnnotationState);
      if (pAnnotationState->m_Cancelled) { return; }
   } else {
      COL_TRC("Called from BLUA, do blocking curl call"); // TODO - switch to using interruptible version once BLUA is refactored
      LNETcurlBlockingCall(pItem);
   }
   COL_VAR(pItem->m_MessageBody);
}

static void LNEThttpBuildHeadersFromParams(LNEThttpCurlItem* pItem, const COLvar& Params){
   COL_FUNCTION(LNEThttpBuildHeadersFromParams);
   if (!Params.exists("headers")) {
      return;
   } else if (!Params["headers"].isArray()) {
      COL_ERROR_STREAM("Headers must be either a table, an array of strings, or nil.", COLerror::PreCondition);
   }
   /* Setting up headers */
   // A processed header will always be an array of strings, so proceed with that in mind
   // Headers must be built in a specific format - as a struct curl_slist
   // Furthermore, the implementation requires that the initial curl_slist* be NULL, or behaviour is undefined
   if (pItem->m_pHeaders) { curl_slist_free_all(pItem->m_pHeaders); pItem->m_pHeaders = NULL; }
   const COLvar::COLvarVec& Headers = Params["headers"].array();
   for (COLvar::COLvarVec::const_iterator it = Headers.begin(); it != Headers.end(); it++) {
      pItem->m_pHeaders = curl_slist_append(pItem->m_pHeaders, (*it).str().c_str());
   }
   if (pItem->m_pHeaders) {
      setCurlOption(pItem, CURLOPT_HTTPHEADER, pItem->m_pHeaders);
   }
}

static void LNEThttpbuildAuthMethod(LNEThttpCurlItem* pItem, const COLvar& Params){
   COL_FUNCTION(LNEThttpbuildAuthMethod);
   // The validated auth.method is an array of strings, each being an auth method to try
   const COLvar::COLvarVec& AuthMethods = Params["auth"]["method"].array();
   long AuthMethod = 0;
   for (COLvar::COLvarVec::const_iterator it = AuthMethods.begin(); it != AuthMethods.end(); it++) {
      AuthMethod |= LNETgetCurlAuthMethodFromColString(it->str());
   }
   if (AuthMethod != CURLAUTH_NONE) { setCurlOption(pItem, CURLOPT_HTTPAUTH, AuthMethod); }
}

static void LNEThttpBuildAuthFromParams(LNEThttpCurlItem* pItem, const COLvar& Params){
   COL_FUNCTION(LNEThttpBuildAuthFromParams);
   if (Params["auth"].isNull()) {
      COL_TRC("Auth is not set - using CURL defaults");
      return;
   }
   if (Params["auth"].exists("method")) {
      LNEThttpbuildAuthMethod(pItem, Params);
   } // else the default auth methods will be used
   setCurlOption(pItem, CURLOPT_USERNAME, Params["auth"]["username"].str().c_str());
   setCurlOption(pItem, CURLOPT_PASSWORD, Params["auth"]["password"].str().c_str());
   setCurlOption(pItem, CURLOPT_UNRESTRICTED_AUTH, Params["auth"].exists("follow") && Params["auth"]["follow"].asBool() ? 1L : 0L);
}

static void LNEThttpBuildSslFromParams(LNEThttpCurlItem* pItem, const COLvar& Params){
   COL_FUNCTION(LNEThttpBuildSslFromParams);
   const COLvar& Ssl = Params["ssl"];
   // The ssl version must be provided as a long from the CURL header definitions
   // Force TLS1.3 -> TLS1.2 as default.
   long SslVersion = CURL_SSLVERSION_TLSv1_2;
   if (!Ssl.isNull() && Ssl.exists("ssl_version")) {
      COL_TRC("Setting user specified SSL version.");
      SslVersion = LNETgetCurlSslVersionFromColString(Ssl["ssl_version"].str());
      setCurlOption(pItem, CURLOPT_SSLVERSION, SslVersion);
   }
   setCurlOption(pItem, CURLOPT_SSLVERSION, SslVersion);
   // Force our cipher suite list as a default
   if (!Ssl.isNull() && Ssl.exists("cipher_suite_list")) {
      COL_TRC("User specified custom supported cipher suite list.");
      const COLstring& CipherList = Ssl["cipher_suite_list"].str();
      setCurlOption(pItem, CURLOPT_TLS13_CIPHERS, CipherList.c_str());
   } else {
      COL_TRC("Using iNTERFACEWARE recommended cipher suite list.");
      const COLstring& RecommendedCipherList = DBC_SSL_DEFAULT_CIPHER_SUITES;
      setCurlOption(pItem, CURLOPT_TLS13_CIPHERS, RecommendedCipherList.c_str());
   }
   // Force our cipher list as a default
   if (!Ssl.isNull() && Ssl.exists("cipher_list")) {
      COL_TRC("User specified custom supported cipher list.");
      const COLstring& CipherList = Ssl["cipher_list"].str();
      setCurlOption(pItem, CURLOPT_SSL_CIPHER_LIST, CipherList.c_str());
   } else {
      COL_TRC("Using iNTERFACEWARE recommended cipher list.");
      const COLstring& RecommendedCipherList = DBC_SSL_DEFAULT_CIPHERS;
      setCurlOption(pItem, CURLOPT_SSL_CIPHER_LIST, RecommendedCipherList.c_str());
   }
   if ( ! Ssl.isNull() && Ssl.exists("ca_file") ) {
      setCurlOption(pItem, CURLOPT_CAINFO, Ssl["ca_file"].str().c_str());
   } else {
      COLstring DefaultCaPath = CURLdefaultCaBundlePath();
      setCurlOption(pItem, CURLOPT_CAINFO, DefaultCaPath.c_str());
   }
   if (Ssl.isNull()) {
      COL_TRC("Ssl is not set - using CURL defaults for rest of values");
      return;
   }

   setCurlOptionIfExists(pItem, CURLOPT_SSLCERT,     Ssl, "cert");
   setCurlOptionIfExists(pItem, CURLOPT_SSLCERTTYPE, Ssl, "cert_type");
   setCurlOptionIfExists(pItem, CURLOPT_SSLKEY,      Ssl, "key");
   setCurlOptionIfExists(pItem, CURLOPT_SSLKEYTYPE,  Ssl, "key_type");
   setCurlOptionIfExists(pItem, CURLOPT_SSLENGINE,   Ssl, "ssl_engine");
   setCurlOptionIfExists(pItem, CURLOPT_ISSUERCERT,  Ssl, "issuer_cert");
   setCurlOptionIfExists(pItem, CURLOPT_CRLFILE,     Ssl, "crl_file");

   // These options set custom SSL context setup instructions - called before init of SSL connection after processing other SSL options
   // If the _stats extension is no longer relevant, then these custom handler settings can be removed
   if (Ssl.exists("__cert_data") || Ssl.exists("__key_data") || Ssl.exists("__ca_data")) {
      setCurlOption(pItem, CURLOPT_SSL_CTX_FUNCTION, &LNETonCurlSslContextSetup);
      setCurlOption(pItem, CURLOPT_SSL_CTX_DATA, pItem);
      // These parameters are required for the context setup later, so save a copy of them
      if (Ssl.exists("__cert_data")) { pItem->m_SslData.m_Cert = Ssl["__cert_data"]; }
      if (Ssl.exists("__key_data"))  { pItem->m_SslData.m_Key  = Ssl["__key_data"];  }
      if (Ssl.exists("__ca_data"))   { pItem->m_SslData.m_Ca   = Ssl["__ca_data"];   }
   }

   // If key_pass is not set, the the user is prompted on stdin to enter a password, so provide a blank fallback
   if (Ssl.exists("key_pass")) {
      setCurlOptionFromVar(pItem, CURLOPT_SSLKEYPASSWD, Ssl["key_pass"]);
   } else {
      setCurlOption(pItem, CURLOPT_SSLKEYPASSWD, "");
   }

   // The verify peer and verify hosts must be provided as longs, defaulting to "true"
   long VerifyPeer = (!Ssl.exists("verify_peer") || Ssl["verify_peer"].asBool()) ? 1L : 0L;
   long VerifyHost = (!Ssl.exists("verify_host") || Ssl["verify_host"].asBool()) ? 2L : 0L;  // libcurl expects 2
   setCurlOption(pItem, CURLOPT_SSL_VERIFYPEER, VerifyPeer);
   setCurlOption(pItem, CURLOPT_SSL_VERIFYHOST, VerifyHost);
}

static void LNEThttpBuildHttpVersionFromParams(LNEThttpCurlItem* pItem, const COLvar& Params){
   COL_FUNCTION(LNEThttpBuildHttpVersionFromParams);
   if (Params["http_version"].isNull()) {
      COL_TRC("http_version is not set - using CURL defaults");
      return;
   }
   long HttpVersion = LNETgetCurlHttpVersionFromColString(Params["http_version"].str());
   setCurlOption(pItem, CURLOPT_HTTP_VERSION, HttpVersion);
}

// This function builds parts of the cURL object that are common to all the methods
// it calls the three static functions above to fill out individual parts of the cURL object.
void LNEThttpBuildCommonFromParams(LNEThttpCurlItem* pItem, const COLvar& Params) {
   COL_FUNCTION(LNEThttpBuildCommonFromParams);
   COL_VAR(Params);
   if (!pItem->m_pCurl) {
      COL_ERROR_STREAM("cURL handle is not initialized. Need a valid cURL handle to build off.", COLerror::PreCondition);
   }

   if (Params["debug"].asBool()) {
      /* This method attaches callbacks to the cURL handle and logs all activity */
      COL_TRC("cURL debug mode enabled - an extra return value with debug output will be included");
      LNEThttpDebugStartTracing(pItem);
   }

   /* Set up the URL using just the URL parameter
    * GET parameters (query string) is not considered here, but the caller can override the URL later */
   setCurlOption(pItem, CURLOPT_URL, Params["url"].str().c_str());

   /* Timeout value */
   setCurlOption(pItem, CURLOPT_TIMEOUT, Params["timeout"].asInt());

   /* Keep the connection alive, so long as cURL is active
      This is particularly useful for long uploads */
   // Only available in 7.25.0 (current 7.21.6)
   // setCurlOption(this, CURLOPT_TCP_KEEPALIVE, 1L);

   /* Allow a maximum of 50 redirects */
   setCurlOption(pItem, CURLOPT_MAXREDIRS, 50L);

   /* HTTP method to use */
   // NOTE : There is no actual entry point in net.http that uses this method, but it's supported here for extensibility
   if (Params["method"].str() == "HEAD") {
      setCurlOption(pItem, CURLOPT_NOBODY, 1L);
   } else if (!Params["method"].str().is_null()) {
      setCurlOption(pItem, CURLOPT_CUSTOMREQUEST, Params["method"].str().c_str());
   }

   LNEThttpBuildHeadersFromParams(pItem, Params);
   LNEThttpBuildAuthFromParams(pItem, Params);
   LNEThttpBuildSslFromParams(pItem, Params);
   LNEThttpBuildHttpVersionFromParams(pItem, Params);

   // Attach the response handlers and fields so they can be accessed after the cURL request completes
   setCurlOption(pItem, CURLOPT_WRITEFUNCTION, &LNETonReceivedCurlData);
   setCurlOption(pItem, CURLOPT_WRITEDATA, &(pItem->m_MessageBody));
   setCurlOption(pItem, CURLOPT_HEADERFUNCTION, &LNETonReceivedCurlData);
   setCurlOption(pItem, CURLOPT_HEADERDATA, &(pItem->m_ResponseHeaders));

   // Always follow redirects
   setCurlOption(pItem, CURLOPT_FOLLOWLOCATION, 1L);
}

void LNEThttpBuildPOST(LNEThttpCurlItem* pItem, const COLvar& Params){
   COL_FUNCTION(LNEThttpBuildPOST);
   if (!pItem->m_pCurl) {
      COL_ERROR_STREAM("cURL handle is not initialized. Need a valid cURL handle to build off.", COLerror::PreCondition);
   }

   if (Params["method"].str() == "POST") {
      setCurlOption(pItem, CURLOPT_POST, 1L);
   } else {
      // We must be doing TRACE
      setCurlOption(pItem, CURLOPT_CUSTOMREQUEST, Params["method"].str().c_str());
   }

   LNEThttpBuildCommonFromParams(pItem, Params);
   // For POSTing, the get_parameters must be placed in the URL, and the body must be filled with the parameters
   COLstring FormedUrl;
   if (Params.exists("get_parameters")) {
      COLstring QueryString;
      LNETvarToHttpParameters(Params["get_parameters"], &QueryString);
      LNEThttpAppendQueryString(Params["url"].str(), QueryString, &FormedUrl);
   } else {
      FormedUrl = Params["url"].str();
   }
   setCurlOption(pItem, CURLOPT_URL, FormedUrl.c_str());
   // If the body is supplied, that overrides anything set in parameters (i.e. parameters is ignored)
   if (Params.exists("body") && !Params["body"].str().is_null()) {
      const COLstring& Body = Params["body"].str();
      // Setting size before the data is binary safe - cURL will only copy those many characters
      setCurlOption(pItem, CURLOPT_POSTFIELDSIZE, (long)Body.size());
      setCurlOption(pItem, CURLOPT_POSTFIELDS, Body.c_str());
   } else {
      COLstring FormattedParameters;
      LNETvarToHttpParameters(Params["parameters"], &FormattedParameters);
      setCurlOption(pItem, CURLOPT_POSTFIELDSIZE, (long)FormattedParameters.size());
      // Since the formatted params are in a temporary, ask cURL to copy it explicitly
      setCurlOption(pItem, CURLOPT_COPYPOSTFIELDS, FormattedParameters.c_str());
   }
}

void LNEThttpBuildGET(LNEThttpCurlItem* pItem, const COLvar& Params){
   COL_FUNCTION(LNEThttpBuildGET);
   if (!pItem->m_pCurl) {
      COL_ERROR_STREAM("cURL handle is not initialized. Need a valid cURL handle to build off.", COLerror::PreCondition);
   }

   if (Params["method"].str() == "GET") {
      setCurlOption(pItem, CURLOPT_HTTPGET, 1L);
   } else {
      // We are doing HEAD or OPTIONS
      setCurlOption(pItem, CURLOPT_CUSTOMREQUEST, Params["method"].str().c_str());
   }
   LNEThttpBuildCommonFromParams(pItem, Params);

   // For GETing, the parameters must be placed in the URL
   COLstring FormedUrl;
   if (Params.exists("parameters")) {
      COLstring QueryString;
      LNETvarToHttpParameters(Params["parameters"], &QueryString);
      LNEThttpAppendQueryString(Params["url"].str(), QueryString, &FormedUrl);
   } else {
      FormedUrl = Params["url"].str();
   }
   setCurlOption(pItem, CURLOPT_URL, FormedUrl.c_str());
}

void LNEThttpBuildPUT(LNEThttpCurlItem* pItem, const COLvar& Params){
   COL_FUNCTION(LNEThttpBuildPUT);
    if (!pItem->m_pCurl) {
      COL_ERROR_STREAM("cURL handle is not initialized. Need a valid cURL handle to build off.", COLerror::PreCondition);
   }
   setCurlOption(pItem, CURLOPT_UPLOAD, 1L);
   LNEThttpBuildCommonFromParams(pItem, Params);
   // Attach callbacks that actually transfer the data
   setCurlOption(pItem, CURLOPT_READDATA, pItem);
   setCurlOption(pItem, CURLOPT_READFUNCTION, &LNETonSendCurlData);
   setCurlOption(pItem, CURLOPT_SEEKDATA, pItem);
   setCurlOption(pItem, CURLOPT_SEEKFUNCTION, &LNETonSeekCurlData);

   if (Params["method"].str() != "PUT") {
      setCurlOption(pItem, CURLOPT_CUSTOMREQUEST, Params["method"].str().c_str());
   }
   pItem->m_PutData.m_BufferedSendDataPosition = 0;
   if (Params.exists("data") && !Params["data"].str().is_null()) {
      // Copy over the data - It will be needed later in the callback
      pItem->m_PutData.m_StaticPutData = Params["data"].str();
      // Default the size to the length of the string (Params["size"] overrides it below if it's explicitly set)
      int DataSize = Params["data"].str().size();

      // see header for why bytes are counted
      pItem->m_PutData.m_IsKnownSize = true;
      pItem->m_PutData.m_BytesRemaining = DataSize;   // see header for why this is done
      setCurlOption(pItem, CURLOPT_INFILESIZE_LARGE, (curl_off_t)DataSize); // see below for why (curl_off_t) cast is done
   }
   // kind of like setCurlOptionIfExists, but value must be explicitly cast to (curl_off_t) for variable-width reasons
   if (Params.exists("size")) {
      COL_TRC("Explicitly setting cURL send size to " << Params["size"].asInt());
      // see header for why bytes are counted
      pItem->m_PutData.m_IsKnownSize = true;
      pItem->m_PutData.m_BytesRemaining = Params["size"].asInt();
      setCurlOption(pItem, CURLOPT_INFILESIZE_LARGE, (curl_off_t)Params["size"].asInt());
   }
}

void LNEThttpBuildDELETE(LNEThttpCurlItem* pItem, const COLvar& Params){
   COL_FUNCTION(LNEThttpBuildDELETE);
   if (!pItem->m_pCurl) {
      COL_ERROR_STREAM("cURL handle is not initialized. Need a valid cURL handle to build off.", COLerror::PreCondition);
   }
   setCurlOption(pItem, CURLOPT_CUSTOMREQUEST, "DELETE");
   LNEThttpBuildCommonFromParams(pItem, Params);
   // There is nothing extra to do for DELETE
}