//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: HTTPvariables
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Saturday, December 9th, 2006 @ 11:22:38 AM
//
//---------------------------------------------------------------------------
#include "HTTPprecomp.h"
#pragma hdrstop

#include "HTTPvariables.h"
#include "HTTPrequest.h"
#include "HTTPrequestParser.h"
#include "HTTPmultiPart.h"

#include <COL/COLvar.h>
#include <COL/COLhashmap.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void HTTPparseVariables(const COLstring& RawData, HTTPvarMap* pVariableList, bool NormalizeKeys){
   COL_FUNCTION(HTTPparseVariables);
   COL_VAR(RawData);
   COL_VAR(NormalizeKeys);

   const char* pInput = RawData.c_str();
   do {
      int Length = strcspn(pInput, "&");
      int KeyLength = strcspn(pInput, "&=");
      COL_VAR2(Length, KeyLength);

      if(KeyLength < Length) {
         int ValueLength = Length - KeyLength - 1;
         COL_VAR(ValueLength);
         COLstring Key, Value;
         Key.append(pInput,KeyLength);
         HTTPunescapeCharacters(Key);

         if(ValueLength > 0) {
            Value.append(pInput + KeyLength + 1, ValueLength);
            HTTPunescapeCharacters(Value);
         }

         COL_VAR2(Key, Value);
         pVariableList->add(NormalizeKeys ? Key.toLowerCase() : Key, Value);
      }
      pInput += Length;
   } while(*pInput++);
}

void HTTPparseVariables(const COLstring& RawData, COLvar* pVariableList){
   COL_FUNCTION(HTTPparseVariables);
   COL_VAR(RawData);

   COLvar& Vars = *pVariableList;
   const char* pInput = RawData.c_str();
   do {
      int Length = strcspn(pInput, "&");
      int KeyLength = strcspn(pInput, "&=");
      COL_VAR2(Length, KeyLength);

      if(KeyLength < Length) {
         int ValueLength = Length - KeyLength - 1;
         COL_VAR(ValueLength);
         COLstring Key, Value;
         Key.append(pInput, KeyLength);
         HTTPunescapeCharacters(Key);

         if(ValueLength > 0) {
            Value.append(pInput + KeyLength + 1, ValueLength);
            HTTPunescapeCharacters(Value);
         }

         COL_VAR2(Key, Value);

         // When we see a key for the second time, turn the matching string COLvar
         // into an array, and start stuffing it with strings.
         if (Vars.exists(Key)) {
            if (! Vars[Key].isArray()) {
               COLvar Tmp;
               Tmp.swap(&Vars[Key]);
               Vars[Key].setArrayType();
               Vars[Key].push_back(Tmp);
            }
            Vars[Key].push_back(COLvar(Value));
         } else {
            Vars[Key] = Value;
         }
      }
      pInput += Length;
   } while(*pInput++);
   COL_VAR(Vars);
}

void HTTPextractGetVariables(const HTTPrequest& Request, HTTPvarMap* pVariableList, bool NormalizeKeys) {
   COL_FUNCTION(HTTPextractGetVariables);
   if (!Request.queryString().is_null()) {
      HTTPparseVariables(Request.queryString(), pVariableList, NormalizeKeys);         
   }
}

void HTTPextractGetVariables(const HTTPrequest& Request, COLvar* pVariableList) {
   COL_FUNCTION(HTTPextractGetVariables);
   if (!Request.queryString().is_null()) {
      HTTPparseVariables(Request.queryString(), pVariableList);         
   }
}

void HTTPextractPostVariables(const void* pBody, COLuint32 SizeOfBody, HTTPvarMap* pVariableList, bool NormalizeKeys){
   COL_FUNCTION(HTTPextractPostVariables);
   COLstring BodyString((const char*)pBody, SizeOfBody);
   HTTPparseVariables(BodyString, pVariableList, NormalizeKeys);
}

void HTTPextractPostVariables(const void* pBody, int SizeOfBody, COLvar* pVariableList){
   COL_FUNCTION(HTTPextractPostVariables);
   COLstring BodyString((const char*)pBody, SizeOfBody);
   HTTPparseVariables(BodyString, pVariableList);
}
// TODO - I didn't bother creating a COLvar version of HTTPextractPostVariablesFast


void HTTPextractPostVariablesFast(const void* pBody, COLuint32 SizeOfBody, HTTPvarMap* pVariableList, bool NormalizeKeys) {
   COL_FUNCTION(HTTPextractPostVariablesFast);
   if(SizeOfBody) {
      const char* pBodyStr = (const char*)pBody;
      COLASSERT(pBodyStr[SizeOfBody] == '\0');  // Not a bug.  See header.
      COLstring BodyString(pBodyStr, SizeOfBody);
      HTTPparseVariables(BodyString, pVariableList, NormalizeKeys);
   }
}

HTTPrequestType HTTPextractRequestType(const HTTPrequest& Request){
   COL_FUNCTION(HTTPextractRequestType);
   if (Request.method() == "POST"){
      // must check for and handle the special case where we 
      // have a post with multi-part data (this is the case when 
      // we have a post that contains some file data - support for file
      // upload see: RFC1867)
      const COLstring Boundary(HTTPgetBoundary(Request));
      if (!Boundary.is_null()) {
         COL_TRC("MULTIPART");
         return HTTP_POST_MULTIPART;
      }
      else {
         COL_TRC("POST");
         return HTTP_POST;
      }
   }

   if (Request.method() == "GET") {
      COL_TRC("GET");
      return HTTP_GET;
   }
   if (Request.method() == "HEAD") {
      COL_TRC("HEAD");
      return HTTP_HEAD;
   }
   COL_TRC("OTHER");
   return HTTP_OTHER;
}

void HTTPparseVariables(const HTTPrequest& Request, const void* pBody, int BodySize, COLvar* pVariables){
   COL_FUNCTION(HTTPparseVariables);
   COL_VAR(Request.queryString());
   if (Request.method() == "POST") {
      COL_TRC("Extracting POST variables from request");
      HTTPextractPostVariables(pBody, BodySize, pVariables);
   }
   else if (Request.method() == "GET") {
      COL_TRC("Extracting GET variables from request");
      HTTPextractGetVariables(Request, pVariables);
   }   
}
