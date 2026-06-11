//---------------------------------------------------------------------------
// Copyright (C) 1997-2019 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: WEBfetch
//
// Author: Eliot Muir
//
// Description:
//
// Implementation
//---------------------------------------------------------------------------

#include "WEButils.h"

#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

enum WEBhttpStatusCode{
   HTTP_STATUS_OK= 200,
   HTTP_STATUS_CREATED = 201,
   HTTP_STATUS_ACCEPTED = 202,
   HTTP_STATUS_NO_CONTENT = 204,
   HTTP_STATUS_MOVED_PERM = 301,
   HTTP_STATUS_FOUND = 302,
   HTTP_STATUS_NOT_MODIFIED = 304,
   HTTP_STATUS_BAD_REQUEST = 400,
   HTTP_STATUS_UNAUTHORIZED = 401,
   HTTP_STATUS_FORBIDDEN = 403,
   HTTP_STATUS_NOT_FOUND = 404,
   HTTP_STATUS_INTERNAL_ERROR = 500,
   HTTP_STATUS_NOT_IMPLEMENTED = 501,
   HTTP_STATUS_BAD_GATEWAY = 502,
   HTTP_STATUS_SERVICE_UNAVAILABLE = 503
};

COLstring WEBlookupReason(int Code){
   COL_FUNCTION(WEBlookupReason);
   switch (Code){
      case HTTP_STATUS_OK:                   return "OK";
      case HTTP_STATUS_CREATED:              return "Created";
      case HTTP_STATUS_ACCEPTED:             return "Accepted";
      case HTTP_STATUS_NO_CONTENT:           return "No Content";
      case HTTP_STATUS_MOVED_PERM:           return "Moved Permanently";
      case HTTP_STATUS_FOUND:                return "Found";
      case HTTP_STATUS_NOT_MODIFIED:         return "Not Modified";
      case HTTP_STATUS_BAD_REQUEST:          return "Bad Request";
      case HTTP_STATUS_UNAUTHORIZED:         return "Unauthorized";
      case HTTP_STATUS_FORBIDDEN:            return "Forbidden";
      case HTTP_STATUS_NOT_FOUND:            return "Not Found";
      case HTTP_STATUS_INTERNAL_ERROR:       return "Internal Server Error";
      case HTTP_STATUS_NOT_IMPLEMENTED:      return "Not Implemented";
      case HTTP_STATUS_BAD_GATEWAY:          return "Bad Gateway";
      case HTTP_STATUS_SERVICE_UNAVAILABLE:  return "Service Unavailable";
      default:                               return "Extension Code";
   }
}

bool WEBisRedirectCode(int Code){
   COL_FUNCTION(WEBisRedirectCode);
   COL_VAR(Code);
   return (Code > 299) && (Code < 309);
}

unsigned int WEBconvertHexString(const char* pValue, int Length){
   COL_FUNCTION(WEBconvertHexString);
   unsigned int Result = 0;
   // COL_VAR(Result);
   const char* pEnd = pValue + Length;
   for (const char* i=pValue; i !=pEnd; i++){
      COL_VAR(*i);
      if (*i >= '0' && *i <= '9'){
         Result += (*i - '0');                                                                                                                 
      } else if (*i >= 'A' && *i <= 'F'){
         Result +=  *i - 'A' + 10;
      } else if (*i >= 'a' && *i <= 'f'){
         Result += *i - 'a' + 10;
      }
      //COL_VAR(Result);
      Result = Result << 4;
      //COL_VAR(Result);
   }
   Result = Result >> 4;
   COL_VAR(Result);
   return Result;
}

// COLstring WEBconvertInttoHex(const COLstring& Input) {

//    int Size = Input.size();

//    // char *  itoa ( int value, char * str, int base );
//    char OutputBuffer[33];
//    itoa(Size, OutputBuffer, 10);

//    COL_TRC(OutputBuffer);

//    COLstring Output(OutputBuffer);
//    COL_VAR(Output);

//    return Output;
// }