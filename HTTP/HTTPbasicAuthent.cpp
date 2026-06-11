//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: HTTPbasicAuthent
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Monday, January 14th, 2013 @ 02:35:39 PM
//
//---------------------------------------------------------------------------
#include "HTTPprecomp.h"
#pragma hdrstop

#include "HTTPbasicAuthent.h"
#include "HTTPheader.h"

#include <SFI/SFIbase64DecodeFilter.h>
#include <SFI/SFIbase64EncodeFilter.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

static const COLstring HTTP_AUTHORIZATION("Authorization");
static const COLstring HTTP_BASIC("Basic");

bool HTTPparseBasicAuthent(const HTTPheader& Headers, COLstring* pUserName, COLstring* pPassword){
   COL_FUNCTION(HTTPparseBasicAuthent);
   int HeaderIndex = Headers.findHeader(HTTP_AUTHORIZATION);
   if (HeaderIndex == -1){
      COL_TRC("No " << HTTP_AUTHORIZATION << " header found.");
      return false;
   }
   const COLstring& HeaderValue = Headers.headerValue(HeaderIndex);
   COL_VAR(HeaderValue);
   if (HeaderValue.find(HTTP_BASIC) != 0){
      COL_TRC("Not basic authentication.");
      return false;
   }        
   COLstring EncodedData = HeaderValue.substr(6);  // strip off Basic and ' '   
   COL_VAR(EncodedData);
   EncodedData.stripAll(' ');
   COL_VAR(EncodedData);
   COLstring DecodedData;
   try{
      SFIbase64Decode(EncodedData, &DecodedData);  // TODO some time check if SFIbase64Decode can throw.
   }catch(const COLerror& Error){
      COL_TRC(Error);
      return false;
   }
   COL_VAR(DecodedData);
   if (!DecodedData.split(*pUserName, *pPassword, ":")){
      COL_TRC("Could not split on : character");
      return false;
   }
   return true;
}

void HTTPsetBasicAuthent(HTTPheader* pHeaders, const COLstring& UserName, const COLstring& Password){
   COL_FUNCTION(HTTPsetBasicAuthent);
   pHeaders->addHeader(HTTP_AUTHORIZATION, HTTP_BASIC + " " + SFIbase64Encode(UserName+":"+Password));
}
