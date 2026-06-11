//---------------------------------------------------------------------------
// Copyright (C) 1997-2014 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LNEThttp
//
// Description:
//
// Implementation
//
// Authors: Nasron Cheong, Akshay Ganeshen
// Date:    Fri  7 Nov 2014 15:25:40 EST
//
//---------------------------------------------------------------------------
#include "LNEThttp.h"
#include <LNET/LNETnet.h>

// #include "NODLenvironment.h"
#include <NODL/NODLutils.h>
// #include "NODLglobals.h"
#include <LNET/LNETcurl.h>
#include <LNET/LNEThttpCurl.h>
#include <LNET/LNEThttpDebug.h>
#include <LNET/LNEThttpListen.h>
#include <LNET/LNETrespond.h>
#include <LUA/LUAtimeout.h>
#include <LUAL/LUALutils.h>
// #include "NODLtracing.h"

#include <CURL/CURLutils.h>

#include <LUA/LUAerror.h>
#include <LUA/LUAutils.h>
#include <LUA/LUAvar.h>
#include <LUA/LUAstartup.h>
#include <LUA/LUAtest.h>
#include <LUA/LUAcheck.h>
#include <LUA/LUAtable.h>

#include <HTTP/HTTPrequest.h>
#include <HTTP/HTTPrequestParser.h>
#include <HTTP/HTTPbodyParser.h>
#include <HTTP/HTTPresponse.h>
#include <HTTP/HTTPheader.h>
#include <HTTP/HTTPvariables.h>
#include <HTTP/HTTPcookie.h>

#include <COL/COLauto.h>
#include <COL/COLstring.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLhashmap.h>
// #include <COL/COLsimpleBuffer.h>
// #include <COL/COLvarConvert.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <openssl/ssl.h>

/// HTTP handler declarations
#define LNET_HTTP_NAMESPACE "http"
#define LNET_HTTP_GET "get"
#define LNET_HTTP_POST "post"
#define LNET_HTTP_PUT "put"
#define LNET_HTTP_DELETE "delete"
#define LNET_HTTP_PATCH "patch"
#define LNET_HTTP_HEAD "head"
#define LNET_HTTP_OPTIONS "options"
#define LNET_HTTP_TRACE "trace"
#define LNET_HTTP_PARSE_REQUEST "parseRequest"
#define LNET_HTTP_LISTEN "listen"
#define LNET_HTTP_RESPOND "respond"

// These are the potential options for the various net.http calls
// They are used to verify that all the parameters in a call are recognized
// NOTE : Since these are provided as lists of strings, the parameters are checked one-by-one
//    If there were a lot of options, it would be better to use a hashmap, but since there are only a few, this is fine
static const char* s_GetOptions[] = {
   "url", "auth", "debug", "headers", "response_headers_format", "live", "method", "parameters", "ssl",
   "http_version", "timeout", "cache_connection_by_host", NULL
};

static const char* s_PostOptions[] = {
   "url", "auth", "body", "debug", "get_parameters", "headers", "response_headers_format", "live",
   "http_version", "method", "parameters", "ssl", "timeout", "cache_connection_by_host", NULL
};

static const char* s_PutOptions[] = {
   "url", "auth", "data", "debug", "headers", "response_headers_format", "live", "method", "reader", "size",
   "http_version", "ssl", "timeout", "cache_connection_by_host", NULL
};

static const char* s_DeleteOptions[] = {
   "url", "auth", "debug", "headers", "response_headers_format", "live", "method", "ssl", "timeout",
   "http_version", "cache_connection_by_host", NULL
};

static const char* s_PatchOptions[] = {
   "url", "auth", "debug", "headers", "response_headers_format", "live", "reader", "size", "ssl",
   "http_version", "timeout", "cache_connection_by_host", NULL
};

static const char* s_HeadOptions[] = {
   "url", "auth", "debug", "headers", "response_headers_format", "live", "method", "parameters", "ssl",
   "http_version", "timeout", "cache_connection_by_host", NULL
};

static const char* s_OptionsOptions[] = {
   "url", "auth", "debug", "headers", "response_headers_format", "live", "method", "ssl", "timeout",
   "http_version", "cache_connection_by_host", NULL
};

static const char* s_TraceOptions[] ={
   "url", "auth", "body", "debug", "get_parameters", "headers", "response_headers_format", "live",
   "http_version", "parameters", "ssl", "timeout", "cache_connection_by_host", NULL
};

static const char* s_ParseRequestOptions[] = {
   "data", "headers_format", NULL
};

static const char* s_AuthOptions[] = {
   "follow", "method", "password", "username", NULL
};

static const char* s_SslOptions[] = {
   "ca_file", "cert", "cert_type", "crl_file", "issuer_cert", "key", "key_pass",
   "key_type", "ssl_engine", "ssl_version", "verify_host", "verify_peer", "cipher_list", "cipher_suite_list",
   // The following are private, hidden options for the _stats module.
   "__cert_data", "__key_data", "__ca_data", NULL
};

// Splits a single header of the form "key: value" and assigns it to the supplied destination COLstrings
// Both the Key and Value have their whitespace stripped
// If there is no ":" in the supplied Header string, the whole string is assumed to be a key to a value-less header
void LNETsplitHeader(const COLstring& Header, COLstring* pKey, COLstring* pValue) {
   COL_FUNCTION(LNETsplitHeader);

   COLstring Key, Value;
   if (Header.split(Key, Value, ":")) {
      // Strip away any potential whitespace around the :
      Key = COLstripWhiteSpace(Key, true, true);
      Value = COLstripWhiteSpace(Value, true, true);
      // Faster to swap than to assign
      Key.swap(pKey);
      Value.swap(pValue);
   } else {
      // Assume this is a value-less key
      *pKey = Header;
      *pValue = "";
   }
   COL_VAR3(Header, *pKey, *pValue);
}

// Converts headers either in an array or in a single long string into a COLvar Map
// Does nothing if provided headers are neither of the two types
// NOTE : The COLvar is not explicitly set to a Map, so if it is already a map, the new keys are appended into it
// When includeMultipleValues is set to true, the map will be returned as key => [values]
//    In this case, a header key may have multiple assigned values
static void LNETconvertHeadersToMap(const COLvar& Headers, COLvar* pVar, bool includeMultipleValues=false) {
   COL_FUNCTION(LNETconvertHeadersToMap);

   if (Headers.isString()) {
      COL_TRC("Converting string to map");
      COLstring Buffer, Rest=Headers.str(), Line;
      while(Buffer = Rest, Buffer.split(Line, Rest, "\n"), !(Line = COLstripWhiteSpace(Line, true, true)).is_null()) {
         COLstring Key, Value;
         LNETsplitHeader(Line, &Key, &Value);
         if (includeMultipleValues) {
            (*pVar)[Key].push_back(Value);
         } else {
            (*pVar)[Key] = Value;   // can potentially overwrite!
         }
         COL_VAR2(Key, (*pVar)[Key]);
      }
   } else if (Headers.isArray()) {
      COL_TRC("Converting array to map");
      COLstring Line;
      for (int i = 0; i < Headers.size(); i++) {
         Line = COLstripWhiteSpace(Headers[i].str(), true, true);
         COLstring Key, Value;
         LNETsplitHeader(Line, &Key, &Value);
         if (includeMultipleValues) {
            (*pVar)[Key].push_back(Value);
         } else {
            (*pVar)[Key] = Value;   // can potentially overwrite!
         }
         COL_VAR2(Key, (*pVar)[Key]);
      }
   } else {
      COL_TRC("Doing nothing");
   }

   COL_VAR3(Headers, *pVar, includeMultipleValues);
}

// Helper function to check if a header of the specified key is defined in the COLvar headers
// Handles parsing headers of types string, map, and array - returns false by default
// If pValue is non-null, the value is assigned with the existing value (if it exists, of course)
bool LNETgetHeader(const COLvar& Headers, const COLstring& Key, COLstring* pValue) {
   COL_FUNCTION(LNETgetHeader);

   COL_VAR2(Headers, Key);

   bool FoundHeader = false;
   COLstring StrippedKey = COLstripWhiteSpace(Key, true, true);
   if (Headers.isString()) {
      COLstring ExistingKey, ExistingValue;
      COLstring Header = COLstripWhiteSpace(Headers.str(), true, true);
      LNETsplitHeader(Header, &ExistingKey, &ExistingValue);
      if (ExistingKey == StrippedKey) {
         if (pValue) {
            *pValue = ExistingValue;
         }
         FoundHeader = true;
      }
   } else if (Headers.isArray()) {
      for (int i = 0; i < Headers.size(); i++) {
         COLstring ExistingKey, ExistingValue;
         LNETsplitHeader(Headers[i].str(), &ExistingKey, &ExistingValue);
         if (StrippedKey == ExistingKey) {
            if (pValue) {
               *pValue = ExistingValue;
            }
            FoundHeader = true;
         }
      }
   } else if (Headers.isMap()) {
      if (Headers.exists(StrippedKey)) {
         if (pValue) {
            *pValue = Headers[StrippedKey].str();
         }
         FoundHeader = true;
      }
   }

   COL_VAR(FoundHeader);
   if (FoundHeader) {
      COL_VAR(*pValue);
   }

   return FoundHeader;
}

// Adds a default-value header to a COLvar representing the current state of the headers
//    Passed by reference instead of pointer to be consistent with how NODLsetDefaultValue works
// If the header exists, no value is set
// However, if the header does not exist, the default value is added as a header of the provided key
// This method will try to preserve the format of the header
//    NOTE : If header is of type string, and the keys aren't equal, it will be converted to an array with the new default header
// If headers is not one of a string, array, or map, nothing is done
void LNETsetDefaultHeader(COLvar& Headers, const COLstring& Key, const COLstring& Value) {
   COL_FUNCTION(LNETsetDefaultHeader);

   if (LNETgetHeader(Headers, Key)) {
      COL_TRC("Header is already present - not overwriting");
      return;
   }
   COLstring StrippedKey = COLstripWhiteSpace(Key, true, true);
   COLstring StrippedValue = COLstripWhiteSpace(Value, true, true);

   COL_VAR2(StrippedKey, StrippedValue);
   // Otherwise, have to figure out how to add the header depending on the current header type
   if (Headers.isNull()) {
      // Set it up to be an array of strings when in doubt
      Headers.setArrayType();
      Headers.push_back(StrippedKey + ": " + StrippedValue);
   } else if (Headers.isString()) {
      // Convert the headers to an array and add the new Key:Value pair
      COLstring Header = COLstripWhiteSpace(Headers.str(), true, true);
      COLvar HeaderArray = COLvar().setArrayType();
      HeaderArray.push_back(Header);
      HeaderArray.push_back(StrippedKey + ": " + StrippedValue);
      HeaderArray.swap(&Headers);
   } else if (Headers.isArray()) {
      Headers.push_back(StrippedKey + ": " + StrippedValue);
   } else if (Headers.isMap()) {
      Headers[StrippedKey] = StrippedValue;
   }

   COL_VAR(Headers);
}

// This method is called immediately after generating the COLvar from lua
// Currently, the usage of this method is to allow some leniency in the call of a net.http request as a function (with a single argument)
//    In this case, the argument is implicitly converted to the URL parameter of the request, and the COLvar is converted to a map to reflect this change
static void LNETpreprocessVar(COLvar* pVar) {
   COL_FUNCTION(LNETpreprocessVar);
   COL_VAR(*pVar);
   if (pVar->isString()) {
      COL_TRC("Converting single string argument to map argument with key 'url'");
      COLvar ProcessedVar = COLvar().setMapType();
      ProcessedVar["url"] = pVar->str();
      *pVar = ProcessedVar;
      COL_VAR(*pVar);
   } else {
      COL_TRC("Nothing changed");
   }
   COL_VAR(*pVar);
}

// This method sets some sane values that are common to all request types.
// Although L and i are passed in (the lua table used to generate the pVar), they are only being used for the environment test
static void LNETsetDefaultValuesCommon(lua_State* L, int i, COLvar *pVar) {
   COL_FUNCTION(LNETsetDefaultValuesCommon);

   COLsetDefault(pVar, "url", "");
   COLsetDefault(pVar, "timeout", LNET_CURL_DEFAULT_TIMEOUT);
   COLsetDefaultArray(pVar, "headers");
   COLsetDefault(pVar, "response_headers_format", "default");
   COLsetDefault(pVar, "method", "");
   COLsetDefault(pVar, "debug", false);
   COLsetDefault(pVar, "cache_connection_by_host", true);
   if (!LUALisEnvironmentTest(L)) {
      (*pVar).erase("live");
   }
   COLsetDefault(pVar, "live", !LUALisEnvironmentTest(L));
   if (LUALisEnvironmentStartup(L)) { // set both default and current value to false
      COLsetDefault(pVar, "live", false);
      (*pVar)["live"] = false;
   }
   COL_VAR(*pVar);
}

// Helper methods for LNETprocessValuesCommon (See the comment for that method)
// Forward declared for code-readability
static void LNETprocessUrl(COLvar* pVar);
// static void LNETprocessHeaders(COLvar* pVar);
static void LNETprocessAuth(COLvar* pVar);
static void LNETprocessSsl(COLvar* pVar);

// Common processing.
// This function has two components to it
// First, it validates all the fields in the COLvar so they may be used readily when building the HTTP request client object (e.g. cURL)
//    The primary considerations here are that fields are of certain types (e.g. Strings, bools, ints, etc.)
//    There are also checks to ensure that certain API restrictions match (e.g. Cannot specify both 'body' and 'parameters' in POST)
// Second, it converts fields to a consistent type when the parameter may be specified in multiple forms:
//    Urls are prefixed with 'http://' if it is unspecified whether it is http or https
//    Headers are converted to an array of strings (e.g. {foo='bar'} => ["foo: bar"])
//    Auth methods is converted to an array of strings, each being a method to try when authenticating
// Note that this is idempotent so as to not override any values if they are already in the standardized form
static void LNETprocessValuesCommon(COLvar* pVar) {
   COL_FUNCTION(LNETprocessValuesCommon);

   // This assignment is used so that the COLassert macros will produce nicer error messages
   const COLvar& Argument = *pVar;

   // NOTE : Checking for known parameters is impossible here
   // it must be done by the individual request handler (one level up from here)
   // e.g. If this is a POST request, do: LUAparamsAreKnown(Params, s_PostOptions)

   COLassertParamIsString(Argument, "method");
   const COLstring& Method = (*pVar)["method"].str();
   if (Method != "GET" && Method != "POST" && Method != "PUT" && Method != "DELETE" && Method != "PATCH" && Method != "HEAD" && Method != "OPTIONS" && Method != "TRACE") {
      COL_ERROR_STREAM("Method must be one of GET, POST, PUT, DELETE, PATCH, HEAD, OPTIONS or TRACE.", COLerror::PreCondition);
   }

   LNETprocessUrl(pVar);
   LNETprocessHeaders(pVar);
   LNETprocessAuth(pVar);
   LNETprocessSsl(pVar);

   COLassertParamIsString(Argument, "response_headers_format");
   const COLstring& ResponseHeadersFormat = (*pVar)["response_headers_format"].str();
   if (ResponseHeadersFormat != "default" && ResponseHeadersFormat != "raw" && ResponseHeadersFormat != "multiple") {
      COL_ERROR_STREAM("Response headers format must be one of 'default', 'raw', or 'multiple'", COLerror::PreCondition);
   }
   COLassertParamIsStringIfExists(Argument, "body");
   COLassertParamIsStringIfExists(Argument, "data");
   COLassertParamIsIntIfExists(Argument, "size");
   if (pVar->exists("size") && (*pVar)["size"].asInt() <= 0) {
      COL_ERROR_STREAM("Size must be greater than 0.", COLerror::PreCondition);
   }
   // TODO : Assert 'reader' is function if exists
   COLassertParamIsTableIfExists(Argument, "parameters");
   COLassertParamIsTableIfExists(Argument, "get_parameters");

   COLassertParamIsBoolIfExists(Argument, "cache_connection_by_host");

   COLassertParamIsInt(Argument, "timeout");

   /* Certain combinations of parameters are not allowed
    * This is generally because the parameters are mutually exclusive, but are provided in the net.http API in different forms for user convenience
    */
   if (Argument.exists("body") && !Argument["body"].str().is_null() && Argument.exists("parameters") && !Argument["parameters"].map().empty()) {
      COL_ERROR_STREAM("Arguments 'body' and 'parameters' cannot be used simultaneously.", COLerror::PreCondition);
   }
   if (Argument.exists("data") && !Argument["data"].str().is_null() && Argument.exists("reader") && !Argument["reader"].isNull()) {
      COL_ERROR_STREAM("Arguments 'data' and 'reader' cannot be used simultaneously.", COLerror::PreCondition);
   }
}

static void LNETprocessUrl(COLvar* pVar) {
   COL_FUNCTION(LNETprocessUrl);

   if (!(*pVar)["url"].isString()) {
      COL_ERROR_STREAM("URL must be a string.", COLerror::PreCondition);
   }

   //only allow urls of form
   //http://abc
   //https://abc
   //abc - assume this is http:// prefixed
   const char* Http = "http://";
   const char* Https = "https://";
   const COLstring& UrlString = (*pVar)["url"].str();
   if (UrlString.find(Http) != 0 && UrlString.find(Https) != 0){
      // Doesn't start with http or https
      if (UrlString.find("://") != npos) {
         // But it does specify a protocol which is neither http nor https...
         COL_ERROR_STREAM("Invalid URL " << UrlString << ". URLs should begin with http:// or https://.\nIf neither is provided, http:// is assumed.", COLerror::PreCondition);
      } else {
         // Just assume they want http and prefix with that
         (*pVar)["url"] = Http + UrlString;
      }
   }
   // Otherwise, it's already valid
   COL_VAR((*pVar)["url"]);
}

void LNETprocessHeaders(COLvar* pVar) {
   COL_FUNCTION(LNETprocessHeaders);
   COL_VAR((*pVar)["headers"]);

   if (!(*pVar)["headers"].isMap() && !COLvarIsVector((*pVar)["headers"]) && !(*pVar)["headers"].isString() && !(*pVar)["headers"].isNull()) {
      COL_ERROR_STREAM("Headers must be either a table, an array, a string, or nil.", COLerror::PreCondition);
   }

   if (COLvarIsVector((*pVar)["headers"])) {
      // This is already in the form we want
      COL_TRC("Headers is already an array of strings");
      return;
   } else if ((*pVar)["headers"].isNone()) {
      // Convert to an empty array
      COL_TRC("Converting null headers to an empty array");
      (*pVar)["headers"].setArrayType();
   } else if ((*pVar)["headers"].isString()) {
      // Convert the single string to an array of strings
      // This may be invalid according to the implementation:
      // (*pVar)["headers"] = COLvar().push_back((*pVar)["headers"]);
      // So make an explicit copy and the set the value to the copy
      COL_TRC("Converting headers from string to array of strings");
      COLvar HeaderCopy = (*pVar)["headers"];
      (*pVar)["headers"] = COLvar().push_back(HeaderCopy);
   } else if ((*pVar)["headers"].isMap()) {
      // Convert key, value pairs to strings formatted as "key: value"
      COL_TRC("Converting headers from table to array");
      COLvar ProcessedHeaders = COLvar().setArrayType();
      COLvar::COLvarMap::const_iterator it = (*pVar)["headers"].map().cbegin();
      for (; it != (*pVar)["headers"].map().cend(); it++) {
         ProcessedHeaders.push_back(it->first + ": " + it->second.asString());
      }
      (*pVar)["headers"] = ProcessedHeaders;
   } else {
      COL_ERROR_STREAM("Header is of an unknown type.", COLerror::PreCondition);
   }

   COL_VAR((*pVar)["headers"]);
}

static void LNETprocessAuth(COLvar* pVar) {
   COL_FUNCTION(LNETprocessAuth);
   COL_VAR((*pVar)["auth"]);

   if ((*pVar)["auth"].isNull()) {
      // This will end up using cURL (or whichever client) default auth values down the line
      COL_TRC("Auth is null - will use client defaults for auth");
      return;
   } else if (!(*pVar)["auth"].isMap()) {
      COL_ERROR_STREAM("Auth must be either a table or nil.", COLerror::PreCondition);
   }

   // This assignment is used so that the COLassert macros will produce nicer error messages
   const COLvar& Auth = (*pVar)["auth"];

   COLstring UnknownParameterDetails;
   if (!LUAparamsAreKnown(Auth, s_AuthOptions, &UnknownParameterDetails)) {
      COL_ERROR_STREAM("Auth parameter '" << UnknownParameterDetails << "' is not recognized.", COLerror::PreCondition);
   }

   COLassertParamIsString(Auth, "username");
   COLassertParamIsString(Auth, "password");
   COLassertParamIsBoolIfExists(Auth, "follow");

   COLassertParamIsArrayOrStringIfExists(Auth, "method");
   if (Auth.exists("method")) {
      if (Auth["method"].isString()) {
         // Convert it to an array of strings for easier conversion later
         COLvar MethodArray = COLvar().setArrayType();
         MethodArray.push_back(Auth["method"]);
         (*pVar)["auth"]["method"] = MethodArray;
      } else if (!COLvarIsVector(Auth["method"])) {
         COL_ERROR_STREAM("Auth method must be an array or a string.", COLerror::PreCondition);
      }
      // Then ensure each method is actually known - this throws if any auth method is invalid (according to cURL, at least)
      // NOTE : There is a tradeoff here in that the strings are being compared to other strings
      // An alternative is to check if it is a key in a hashmap, but that uses more IO (and less CPU)
      const COLvar::COLvarVec& AuthMethods = Auth["method"].array();
      for (COLvar::COLvarVec::const_iterator it = AuthMethods.begin(); it != AuthMethods.end(); it++) {
         LNETgetCurlAuthMethodFromColString(it->str());
      }
   }

   COL_VAR((*pVar)["auth"]);
}

static void LNETprocessSsl(COLvar* pVar) {
   COL_FUNCTION(LNETprocessSsl);

   if ((*pVar)["ssl"].isNull()) {
      // This will end up using cURL (or whichever client) default ssl values down the line
      COL_TRC("Ssl is null - will use client defaults for ssl");
      return;
   }

   // This assignment is used so that the COLassert macros will produce nicer error messages
   const COLvar& Ssl = (*pVar)["ssl"];

   COLstring UnknownParameterDetails;
   if (!LUAparamsAreKnown(Ssl, s_SslOptions, &UnknownParameterDetails)) {
      COL_ERROR_STREAM("Ssl parameter '" << UnknownParameterDetails << "' is not recognized.", COLerror::PreCondition);
   }

   COLassertParamIsStringIfExists(Ssl, "cert");
   COLassertParamIsStringIfExists(Ssl, "cert_type");
   COLassertParamIsStringIfExists(Ssl, "key");
   COLassertParamIsStringIfExists(Ssl, "key_type");
   COLassertParamIsStringIfExists(Ssl, "ssl_engine");
   COLassertParamIsStringIfExists(Ssl, "ca_file");
   COLassertParamIsStringIfExists(Ssl, "issuer_cert");
   COLassertParamIsStringIfExists(Ssl, "crl_file");
   COLassertParamIsStringIfExists(Ssl, "key_pass");
   COLassertParamIsBoolIfExists(Ssl, "verify_peer");
   COLassertParamIsBoolIfExists(Ssl, "verify_host");
   COLassertParamIsStringIfExists(Ssl, "ssl_version");
   COLassertParamIsStringIfExists(Ssl, "cipher_list");
   COLassertParamIsStringIfExists(Ssl, "cipher_suite_list");

   if (Ssl.exists("ssl_version")) {
      // Ensure that the version is actually known - this throws if the version string is invalid (according to cURL, at least)
      LNETgetCurlSslVersionFromColString(Ssl["ssl_version"]);
   }

   // According to the comments:
   // "These are private options used by the _stats extension."
   // Is it relevant any more? It's still being supported, and has the same behaviour as before for the time being
   COLassertParamIsStringIfExists(Ssl, "__cert_data");
   COLassertParamIsStringIfExists(Ssl, "__key_data");
   COLassertParamIsStringIfExists(Ssl, "__ca_data");

   COL_VAR((*pVar)["ssl"]);
}

// This is a helper method for LNETheaderBlocksToColVar
// Handles the 'raw' formatting option for headers, which just adds the header "key: value" pairs to the Headers field of the destination COLvar
// No splitting is done on the headers
static void LNETheaderBlocksToColVarRaw(const COLarray<COLstring>& HeaderBlocks, COLvar* pVar, bool hasResponseLine=false) {
   COL_FUNCTION(LNETheaderBlocksToColVarRaw);
   COLstring Line, Rest, Buffer;

   (*pVar) = COLvar().setArrayType();
   for (int i = 0; i < HeaderBlocks.size(); i++) {
      COL_VAR2(i, HeaderBlocks[i]);

      COLvar CurrentHeader = COLvar().setMapType();
      if (hasResponseLine) {
         // Response/Status line
         Buffer = HeaderBlocks[i];
         Buffer.split(Line, Rest, "\r\n");
         Line = COLstripWhiteSpace(Line, true, true);

         if (!Line.is_null()) {
            CurrentHeader["Response"] = Line;
            // Backwards compatibility:
            CurrentHeader["RequestOrStatusLine"] = Line;
         }
      } else {
         Rest = HeaderBlocks[i];
      }

      while(Buffer = Rest, Buffer.split(Line, Rest, "\r\n"), !Line.is_null()) {
         Line = COLstripWhiteSpace(Line, true, true);
         // No processing, just push the header as-is into the return
         CurrentHeader["Headers"].push_back(Line);
      }

      (*pVar).push_back(CurrentHeader);
   }
}

// This is a helper method for LNETheaderBlocksToColVar
// Handles the 'default' formatting option for headers, which builds a map using header keys and values
static void LNETheaderBlocksToColVarDefault(const COLarray<COLstring>& HeaderBlocks, COLvar* pVar, bool hasResponseLine=false) {
   COL_FUNCTION(LNETheaderBlocksToColVarDefault);
   COLstring Line, Rest, Buffer;

   // Use the last/latest header (after all redirects have occurred)
   (*pVar) = COLvar().setMapType();
   if (hasResponseLine) {
      // Response/Status line
      Buffer = HeaderBlocks.back();
      Buffer.split(Line, Rest, "\r\n");
      Line = COLstripWhiteSpace(Line, true, true);
      if (!Line.is_null()) {
         (*pVar)["Response"] = Line;
         // Backwards compatibility:
         (*pVar)["RequestOrStatusLine"] = Line;
      }
   } else {
      Rest = HeaderBlocks.back();
   }

   // This helper function can work on COLstrings, so long as the response/request line is not present
   LNETconvertHeadersToMap(Rest, pVar);
}

// This is a helper method for LNETheaderBlocksToColVar
// Handles the 'multiple' formatting option which is a bit of a beast...
// The COLvar is built so that the header key is an index into a map
//    The 'Value' field is the last value, similar to 'default'
//    The 'Values' field is all the values, which is useful when the header key appears multiple times
static void LNETheaderBlocksToColVarMultiple(const COLarray<COLstring>& HeaderBlocks, COLvar* pVar, bool hasResponseLine=false) {// Build key/{value, [values]} in backwards order
   COL_FUNCTION(LNETheaderBlocksToColVarMultiple);
   COLstring Line, Rest, Buffer;

   // NOTE : This is built backwards for backwards compatibility
   //   Previous behavior was to only have arrays of values from the latest block (after all redirects)
   // Have to make two passes
   // First pass will figure out the union of all keys of all headers - needs to be a hashmap to test existence
   //    Values may keep getting overwritten... doesn't really matter, just using the key list for .exists
   COL_TRC("Building key list");
   COLvar KeyList = COLvar().setMapType();
   for (int i = 0; i < HeaderBlocks.size(); i++) {
      if (hasResponseLine) {
         // Response/Status line
         Buffer = HeaderBlocks[i];
         Buffer.split(Line, Rest, "\r\n");
         // Not using Line it at this point, so just ignore it
      } else {
         Rest = HeaderBlocks[i];
      }
      LNETconvertHeadersToMap(Rest, &KeyList);  // this strictly appends keys, though the values may get overwritten
   }
   COL_VAR(KeyList);
   COL_TRC("Done building key list - building return map now");

   // Second pass will start collating the values in the format described above, and assign nil for headers without that key
   (*pVar) = COLvar().setMapType();
   // First index is by the reverse header order, and then by key name (once the headers are parsed)
   for (int i = HeaderBlocks.size() - 1; i >= 0; i--) {
      // Do the special case for i == size-1 to set "Value" fields within this iteration
      // This order is probably better for branch prediction (probably can't unroll any of this)
      bool IsLatestHeader = i == (HeaderBlocks.size() - 1);
      COL_VAR3(i, IsLatestHeader, HeaderBlocks[i]);
      if (hasResponseLine) {
         // Response/Status line
         Buffer = HeaderBlocks[i];
         Buffer.split(Line, Rest, "\r\n");
         Line = COLstripWhiteSpace(Line);
         // Have a default value just in case
         COLvar Response = COLvar().reset();
         if (!Line.is_null()) {
            Response.set(Line);
         }
         if (IsLatestHeader) {
            (*pVar)["Response"]["Value"] = Response;
            (*pVar)["RequestOrStatusLine"]["Value"] = Response;
         }
         (*pVar)["Response"]["Values"].push_back(Response);
         (*pVar)["RequestOrStatusLine"]["Values"].push_back(Response);
      } else {
         Rest = HeaderBlocks[i];
      }
      COLvar CurrentHeader = COLvar().setMapType();
      LNETconvertHeadersToMap(Rest, &CurrentHeader, true /* Include multiple values */);
      COL_VAR(CurrentHeader);
      // Now iterate through the keys, explicitly assigning nil when keys aren't in this header set
      for (COLvar::COLvarMap::const_iterator it = KeyList.map().cbegin(); it != KeyList.map().cend(); it++) {
         COLvar Value = COLvar().setArrayType(); // default to empty set of values
         if (CurrentHeader.exists(it->first)) {
            Value = CurrentHeader[it->first];
         }

         if (IsLatestHeader) {
            // Want to extract the latest set of values
            if (Value.array().size() > 0) {
               (*pVar)[it->first]["Value"] = Value.array().back();
            } else {
               COL_TRC("There is no value for " << it->first << " in the latest set of headers");
               (*pVar)[it->first]["Value"] = COLvar().reset();
            }
         }
         // Flatten the multiple values in the current header - It's easiest to just append them one by one
         // Again... have to build it backwards
         for (int j = Value.array().size() - 1; j >= 0; j--) {
            (*pVar)[it->first]["Values"].push_back(Value[j]);
         }
      }
   }
}

// This method converts header blocks of a specific format to a COLvar with a supplied HeaderFormat
// The expected header block format is as follows:
//    list of header blocks each separated by \r\n
//       list of headers each separated by \r\n
//          [optional 'Response' header] - always the first line in a block
//          key: value OR key
static void LNETheaderBlocksToColVar(const COLstring& HeaderBlockList, const COLstring& HeaderFormat, COLvar* pVar, bool hasResponseLine=false) {
   COL_FUNCTION(LNETheaderBlocksToColVar);
   COL_VAR2(HeaderBlockList, HeaderFormat);
   COLstring Line, Rest = HeaderBlockList, Buffer;
   // The implementation of COLstring doesn't allow splicing as such, so need a temporary string as well
   // while (Rest.split(&Line, &Rest))

   // First, split up the header blocks which are delimited by empty lines (\r\n)
   COLarray<COLstring> HeaderBlocks;
   while(Buffer = Rest, Buffer.split(Line, Rest, "\r\n\r\n"), !Line.is_null()) {
      // TODO : This explicitly copies lines - would be better to work with references instead
      HeaderBlocks.push_back(Line);
   }
   if (HeaderBlocks.size() == 0) {
      COL_TRC("Header blocks could not be split into individual blocks");
      return;
   }

   // Seems well formatted, so begin pushing data into the supplied COLvar
   if (HeaderFormat == "raw") {
      LNETheaderBlocksToColVarRaw(HeaderBlocks, pVar, hasResponseLine);
   } else if (HeaderFormat == "default") {
      LNETheaderBlocksToColVarDefault(HeaderBlocks, pVar, hasResponseLine);
   } else if (HeaderFormat == "multiple") {
      LNETheaderBlocksToColVarMultiple(HeaderBlocks, pVar, hasResponseLine);
   } else {
      COL_ERROR_STREAM("Unknown format: '" << HeaderFormat << "'", COLerror::PreCondition);
   }
}

static int LNETpushHttpResponseToLua(lua_State* L, const LNEThttpCurlItem& HttpCurlItem, const COLstring& RespHeadFormat) {
   COL_FUNCTION(LNETpushHttpResponseToLua);
   COL_VAR3(HttpCurlItem.m_ResponseCode, HttpCurlItem.m_MessageBody, HttpCurlItem.m_ResponseHeaders);

   // Convert ResponseHeaders string to a table for the user
   COLvar HeaderTable;
   LNETheaderBlocksToColVar(HttpCurlItem.m_ResponseHeaders, RespHeadFormat, &HeaderTable, true);
   COL_VAR(HeaderTable);
   LUApushVar(L, HttpCurlItem.m_MessageBody);
   LUApushVar(L, (int)HttpCurlItem.m_ResponseCode);
   LUApushVar(L, HeaderTable);
   if (LNEThttpDebugIsTracing(&HttpCurlItem)) {
      LUApushVar(L, LNEThttpDebugReadOutput(&HttpCurlItem));
      // An extra value, so returning 4 items instead of 3
      return 4;
   }
   return 3;
}

// NOTE : By default, cURL will send the "Expect: 100-continue" header with POST/PUT requests
//        This will cause a 1-2s delay on our end if the server does not implement it...
//        Use this helper to manually set the header to an empty string to prevent this default behaviour
static void LNETsetDefaultExpectsHeader(COLvar* pHeaders) {
   COL_FUNCTION(LNETsetDefaultExpectsHeader);
   LNETsetDefaultHeader(*pHeaders, "Expect", "");
}

static void LNEThttpBegin(lua_State* L, COLvar* pParams, const COLstring& Method) {
   COL_FUNCTION(LNEThttpBegin);
   if (!lua_gettop(L)) {
      LUA_ERROR_STREAM("No arguments provided.");
   } else {
      LUAcheckArgCount(L, 1);
   }
   pParams->fromLua(L);
   LNETpreprocessVar(pParams);
   COLsetDefault(pParams, "method", Method);
}

static int LNEThttpDoCurlRequest(lua_State* L, LNEThttpCurlItem* pHttpCurlItem){
   COL_FUNCTION(LNEThttpDoCurlRequest);
   COL_TRC("live true, doing request");
   try {
      try {
         LNEThttpCurlExecuteRequest(L, pHttpCurlItem);
      } catch (const COLerror& Error) {
         // This rethrows - adds debug information if there is any available
         LNEThttpDebugAppendError(pHttpCurlItem, Error);
      }
   } LUA_CATCH();
   return 0;
}

static int LNEThttpPostComplete(lua_State* L, COLvar* pParams) {
   COL_FUNCTION(LNEThttpPostComplete);
   COLvar& Params = *pParams;
   COLsetDefault(pParams, "body", "");
   COLsetDefaultMap(pParams, "parameters");
   COLsetDefaultMap(pParams, "get_parameters");
   LNETsetDefaultValuesCommon(L, 1, pParams);

   // Set up all the values to be directly input into cURL, doing preprocessing and validation as required - also idempotent
   // The setup performed here ensures that every field is in an expected form (e.g. headers is converted and asserted to be an array of strings)
   // This is a check specifically for POST
   COLstring UnknownParameterDetails;
   if (!(LUAparamsAreKnown(Params, s_PostOptions, &UnknownParameterDetails))) {
      LUA_ERROR_STREAM("Parameter '" << UnknownParameterDetails << "' is not recognized.");
   }
   try {
      LNETprocessValuesCommon(&Params);
      LNETsetDefaultExpectsHeader(&Params["headers"]);
   } LUA_CATCH();

   LNETcurlCacheItem* pCurl;
   try {
      pCurl = LNETcurlGetCachedHandle(L, Params["url"], Params["cache_connection_by_host"].asBool());
   } LUA_CATCH();
   LNEThttpCurlItem HttpCurlItem = LNEThttpCurlItem(pCurl);
   try {
      LNEThttpBuildPOST(&HttpCurlItem, Params);
   } LUA_CATCH();

   if (Params["live"].asBool()) { LNEThttpDoCurlRequest(L, &HttpCurlItem); }
   else                         { COL_TRC("live is false, so not executing the request"); }

   return LNETpushHttpResponseToLua(L, HttpCurlItem, Params["response_headers_format"].str());
}

static int LNEThttpGetComplete(lua_State* L, COLvar* pParams) {
   COL_FUNCTION(LNEThttpGetComplete);
   COLvar& Params = *pParams;
   COLsetDefaultMap(&Params, "parameters");
   LNETsetDefaultValuesCommon(L, 1, &Params);

   COLstring UnknownParameterDetails;
   if (!(LUAparamsAreKnown(Params, s_GetOptions, &UnknownParameterDetails))) {
      LUA_ERROR_STREAM("Parameter '" << UnknownParameterDetails << "' is not recognized.");
   }
   try {
      LNETprocessValuesCommon(&Params);
   } LUA_CATCH();

   LNETcurlCacheItem* pCurl;
   try {
      pCurl = LNETcurlGetCachedHandle(L, Params["url"], Params["cache_connection_by_host"].asBool());
   } LUA_CATCH();
   LNEThttpCurlItem HttpCurlItem = LNEThttpCurlItem(pCurl);
   try {
      LNEThttpBuildGET(&HttpCurlItem, Params);
   } LUA_CATCH();

   if (Params["live"].asBool()) { LNEThttpDoCurlRequest(L, &HttpCurlItem); }
   else                         { COL_TRC("live is false, so not executing the request"); }

   return LNETpushHttpResponseToLua(L, HttpCurlItem, Params["response_headers_format"].str());
}

static int LNEThttpPutComplete(lua_State* L, COLvar* pParams) {
   COL_FUNCTION(LNEThttpPutComplete);
   COLvar& Params = *pParams;
   LNETsetDefaultValuesCommon(L, 1, &Params);

   COLstring UnknownParameterDetails;
   if (!(LUAparamsAreKnown(Params, s_PutOptions, &UnknownParameterDetails))) {
      LUA_ERROR_STREAM("Parameter '" << UnknownParameterDetails << "' is not recognized.");
   }
   try {
      LNETprocessValuesCommon(&Params);
      if (!Params.exists("data") && !Params.exists("reader")) {
         COL_ERROR_STREAM("Either 'data' or 'reader' must be provided.", COLerror::PreCondition);
      }

      // The "headers" field is validated now, so modify it a bit further for PUT:
      // First, set the default "Expect" header to be empty (i.e. no "Expect: 100-continue")
      LNETsetDefaultExpectsHeader(&Params["headers"]);
      if (!Params.exists("size") && Params.exists("reader")) {
         // The provided 'reader' will be providing data as it goes, so set a default header for this:
         LNETsetDefaultHeader(Params["headers"], "Transfer-Encoding", "chunked");
      }
   } LUA_CATCH();

   LNETcurlCacheItem* pCurl;
   try {
      pCurl = LNETcurlGetCachedHandle(L, Params["url"], Params["cache_connection_by_host"].asBool());
   } LUA_CATCH();
   LNEThttpCurlItem HttpCurlItem = LNEThttpCurlItem(pCurl);
   try {
      LNEThttpBuildPUT(&HttpCurlItem, Params);
   } LUA_CATCH();
   // TODO : Because COLvar doesn't bind the reader function, it must be attached manually - Find a better way
   // This is bad because it's now mixing layers of interaction - want to ideally separate lua/C, HTTP, and cURL...
   int ReaderIndex = -1;
   if (LUAoptionalFuncParam(L, 1, "reader")) {
      ReaderIndex = lua_gettop(L);
      LNEThttpCurlAttachReader(&HttpCurlItem, L, ReaderIndex);
   }

   if (Params["live"].asBool()) { LNEThttpDoCurlRequest(L, &HttpCurlItem); }
   else                         { COL_TRC("live is false, so not executing the request"); }

   // TODO : This bit of cleanup goes with the reader function issue above
   if (ReaderIndex >= 0) {
      lua_remove(L, ReaderIndex);
   }
   return LNETpushHttpResponseToLua(L, HttpCurlItem, Params["response_headers_format"].str());
}

// These are the methods provided in net.http.
int LNEThttpPost(lua_State* L){
   COL_FUNCTION(LNEThttpPost);
   COLvar Params;
   LNEThttpBegin(L, &Params, "POST");
   return LNEThttpPostComplete(L, &Params);
}

int LNEThttpGet(lua_State* L) {
   COL_FUNCTION(LNEThttpGet);
   COLvar Params;
   LNEThttpBegin(L, &Params, "GET");
   return LNEThttpGetComplete(L, &Params);
}

int LNEThttpPut(lua_State* L) {
   COL_FUNCTION(LNEThttpPut);
   COLvar Params;
   LNEThttpBegin(L, &Params, "PUT");
   return LNEThttpPutComplete(L, &Params);
}

int LNEThttpPatch(lua_State* L) {
   COL_FUNCTION(LNEThttpPatch);
   COLvar Params;
   LNEThttpBegin(L, &Params, "PATCH");
   return LNEThttpPutComplete(L, &Params);
}

int LNEThttpHead(lua_State* L) {
   COL_FUNCTION(LNEThttpHead);
   COLvar Params;
   LNEThttpBegin(L, &Params, "HEAD");
   return LNEThttpGetComplete(L, &Params);
}

int LNEThttpOptions(lua_State* L) {
   COL_FUNCTION(LNEThttpOptions);
   COLvar Params;
   LNEThttpBegin(L, &Params, "OPTIONS");
   return LNEThttpGetComplete(L, &Params);
}

int LNEThttpTrace(lua_State* L) {
   COL_FUNCTION(LNEThttpTrace);
   COLvar Params;
   LNEThttpBegin(L, &Params, "TRACE");
   return LNEThttpPostComplete(L, &Params);
}

int LNEThttpDelete(lua_State* L) {
   COL_FUNCTION(LNEThttpDelete);
   COLvar Params;
   LNEThttpBegin(L, &Params, "DELETE");
   LNETsetDefaultValuesCommon(L, 1, &Params);

   COLstring UnknownParameterDetails;
   if (!(LUAparamsAreKnown(Params, s_DeleteOptions, &UnknownParameterDetails))) {
      LUA_ERROR_STREAM("Parameter '" << UnknownParameterDetails << "' is not recognized.");
   }
   try {
      LNETprocessValuesCommon(&Params);
   } LUA_CATCH();

   LNETcurlCacheItem* pCurl;
   try {
      pCurl = LNETcurlGetCachedHandle(L, Params["url"], Params["cache_connection_by_host"].asBool());
   } LUA_CATCH();
   LNEThttpCurlItem HttpCurlItem = LNEThttpCurlItem(pCurl);
   try {
      LNEThttpBuildDELETE(&HttpCurlItem, Params);
   } LUA_CATCH();

   if (Params["live"].asBool()) {
      try {
         try {
            // NOTE : The execution of the request can be async here - Otherwise, it blocks and waits here
            LNEThttpCurlExecuteRequest(L, &HttpCurlItem);
         } catch (const COLerror& Error) {
            // This rethrows - adds debug information if there is any available
            LNEThttpDebugAppendError(&HttpCurlItem, Error);
         }
      } LUA_CATCH();
   } else {
      COL_TRC("live is false, so not executing the request");
   }
   return LNETpushHttpResponseToLua(L, HttpCurlItem, Params["response_headers_format"].str());
}

int LNEThttpParseRequest(lua_State* L) {
   COL_FUNCTION(LNEThttpParseRequest);
   if (!lua_gettop(L)) {
      LUA_ERROR_STREAM("No arguments provided.");
   } else {
      LUAcheckArgCount(L, 1);
   }

   COLvar Params;
   Params.fromLua(L);
   // Preprocess to allow a single argument to be considered the 'data' argument
   if (Params.isString()) {
      COLvar ParamMap = COLvar().setMapType();
      ParamMap["data"] = Params.str();
      ParamMap.swap(&Params);
   }

   // Set up some default values
   COLsetDefault(&Params, "headers_format", "default");

   COLstring UnknownParameterDetails;
   if (!(LUAparamsAreKnown(Params, s_ParseRequestOptions, &UnknownParameterDetails))) {
      LUA_ERROR_STREAM("Parameter '" << UnknownParameterDetails << "' is not recognized.");
   }

   try {
      // The common processing only applies to POST, GET, PUT, DELETE, so do the checks and processing manually here
      const COLvar& Argument = Params; // For better error messages from COLassert macros
      COLassertParamIsString(Argument, "data");
      COLassertParamIsString(Argument, "headers_format");

      const COLstring& HeadersFormat = Params["headers_format"].str();
      if (HeadersFormat != "default" && HeadersFormat != "raw" && HeadersFormat != "multiple") {
         COL_ERROR_STREAM("Headers format must be one of 'default', 'raw', or 'multiple'", COLerror::PreCondition);
      }
   }  LUA_CATCH();

   const COLstring& InputData = Params["data"].str();
   // Create the destination for the request information and then parse into it
   HTTPrequest ParsedRequest;
   HTTPrequestParser Parser(ParsedRequest);
   HTTPparseResult ParseResult = Parser.parse(InputData.c_str(), InputData.size());
   if (ParseResult == HTTP_PARSE_ERROR) {
      LUA_ERROR_STREAM(Parser.lastParseError());
   } else if (ParseResult == HTTP_PARSE_NEED_MORE_DATA) {
      LUA_ERROR_STREAM("Input data is incomplete.");
   } else if (ParseResult != HTTP_PARSE_COMPLETE) {
      LUA_ERROR_STREAM("Parsing failed unexpectedly.");
   }

   // Start building up the request information, now that the method and location are available
   COLvar RequestInformation = COLvar().setMapType();

   RequestInformation["method"] = ParsedRequest.method();
   RequestInformation["location"] = ParsedRequest.location();
   // Convert the parsed headers back into a string so it can be used in the conversion from header blocks to a requested format
   // It is roundabout, but it's very important for consistency
   COLstring HeaderBlock = "";
   for (int i = 0; i < ParsedRequest.header().countOfHeader(); i++) {
      HeaderBlock += ParsedRequest.header().headerKey(i) + ": ";
      HeaderBlock += ParsedRequest.header().headerValue(i) + "\r\n";
   }
   HeaderBlock = COLstripWhiteSpace(HeaderBlock, true, true);
   LNETheaderBlocksToColVar(HeaderBlock, Params["headers_format"].str(), &(RequestInformation["headers"]),
                            false /* There is no 'Response' field in this header block */);
   // HTTPcookieJar is a hashmap: COLstring => COLstring
   // Build it and then copy over the keys and values
   HTTPcookieJar Cookies;
   HTTPextractCookies(ParsedRequest, Cookies);
   RequestInformation["cookies"] = COLvar().setMapType();
   for (HTTPcookieJar::iterator it = Cookies.begin(); it != Cookies.end(); it++) {
      // Speed things up with some swapping
      RequestInformation["cookies"][it->first].setType(COLvar::String).swap(&(it->second));
   }

   // After the header, expect a body
   COLPRECONDITION(Parser.endOfHeader() <= InputData.size());
   COLstring Body = InputData.substr(Parser.endOfHeader());

   RequestInformation["body"] = Body;

   // HTTPvarMap is a hashmap: COLstring => COLstring
   HTTPvarMap GetVars, PostVars;
   HTTPextractGetVariables(ParsedRequest, &GetVars);
   // Extracting POST is a bit trickier - have to first confirm that the body is form-like
   COLuint32 ContentTypeHeaderIndex = ParsedRequest.header().findHeader("Content-Type");
   if (ContentTypeHeaderIndex != -1 && ParsedRequest.header().headerValue(ContentTypeHeaderIndex).find("form") != npos) {
      // Seems form-like, so it can be parsed into a buffer for now
      HTTPbodyParser BodyParser;
      COLsinkString BodyBuffer;
      ParseResult = BodyParser.parse(Body.c_str(), Body.size(), ParsedRequest.header(), BodyBuffer);
      if (ParseResult == HTTP_PARSE_ERROR) {
         LUA_ERROR_STREAM(BodyParser.lastError());
      } else if (ParseResult == HTTP_PARSE_NEED_MORE_DATA) {
         LUA_ERROR_STREAM("HTTP response body is incomplete.");
      } else if (ParseResult != HTTP_PARSE_COMPLETE && ParseResult != HTTP_PARSE_CHUNK_COMPLETE) {
         LUA_ERROR_STREAM("Parsing failed unexpectedly.");
      }
      HTTPextractPostVariables((const char*)BodyBuffer.string().data(), BodyBuffer.string().size(), &PostVars);
   }

   // Parameters are extracted, so start reading the information
   RequestInformation["params"] = COLvar().setMapType();
   RequestInformation["post_params"] = COLvar().setMapType();
   RequestInformation["get_params"] = COLvar().setMapType();
   for (HTTPvarMap::iterator it = GetVars.begin(); it != GetVars.end(); it++) {
      // Speed things up with a swap in one of the assignments
      // Need to make a copy for the other assignment though
      RequestInformation["get_params"][it->first].set(it->second);
      RequestInformation["params"][it->first].setType(COLvar::String).swap(&(it->second));
   }
   for (HTTPvarMap::iterator it = PostVars.begin(); it != PostVars.end(); it++) {
      RequestInformation["post_params"][it->first].set(it->second);
      RequestInformation["params"][it->first].setType(COLvar::String).swap(&(it->second));
   }

   COL_VAR(RequestInformation);

   LUApushVar(L, RequestInformation);

   return 1;
}

void LNETaddHttpModule(lua_State* L) {
   COL_FUNCTION(LNETaddHttpModule);
   lua_newtable(L); //http
   LUAaddMethod(L, LNET_HTTP_GET,           &LNEThttpGet);
   LUAaddMethod(L, LNET_HTTP_POST,          &LNEThttpPost);
   LUAaddMethod(L, LNET_HTTP_PUT,           &LNEThttpPut);
   LUAaddMethod(L, LNET_HTTP_DELETE,        &LNEThttpDelete);
   LUAaddMethod(L, LNET_HTTP_PATCH,         &LNEThttpPatch);
   LUAaddMethod(L, LNET_HTTP_HEAD,          &LNEThttpHead);
   LUAaddMethod(L, LNET_HTTP_OPTIONS,       &LNEThttpOptions);
   LUAaddMethod(L, LNET_HTTP_TRACE,         &LNEThttpTrace);
   LUAaddMethod(L, LNET_HTTP_PARSE_REQUEST, &LNEThttpParseRequest);
   LUAaddMethod(L, LNET_HTTP_LISTEN,        &LNEThttpListen);
   LUAaddMethod(L, LNET_HTTP_RESPOND,       &LNEThttpSendResponse);

   lua_setfield(L, -2, LNET_HTTP_NAMESPACE);
}
