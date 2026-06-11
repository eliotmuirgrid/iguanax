//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: HTTPutils
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Wednesday, December 6th, 2006 @ 10:38:49 PM
//
//---------------------------------------------------------------------------
#include "HTTPprecomp.h"
#pragma hdrstop

#include <COL/COLarray.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;


#include "HTTPheader.h"
#include "HTTPutils.h"
#include "HTTPrequest.h"
#include "HTTPcookie.h"
#include "HTTPvariables.h"
#include "HTTPmultiPart.h"
#include "HTTPfile.h"
#include "HTTPresponse.h"

#include <COL/COLhashmap.h>
#include <COL/COLmap.h>

typedef COLhashmap<COLstring, COLstring> TStringMap;
typedef COLmap<COLstring, COLstring> TSortedStringMap;

static void sortFromMap(
   const TStringMap& StringMap,
   TSortedStringMap& SortedMap
){
   SortedMap.clear();
   for (auto i = StringMap.cbegin(); i != StringMap.cend(); i++){
      SortedMap.add(i->first, i->second);
   }
}

void HTTPdumpRequest(COLostream& Stream, const HTTPrequest& Request, const void* pBodyData, COLuint32 SizeOfBody)
{
   const COLuint64 ContentLength = HTTPheaderGetContentLength(Request.header());
   Stream << COL_T("GetVariables=") << Request.queryString() << newline
             << COL_T("ContentLength=") << ContentLength << newline
             << COL_T("Method=") << Request.method() << newline
             << COL_T("Version=") << Request.header().version() << newline
             << COL_T("RequestLocation=") << Request.location() << newline
             << COL_T("RequestString=") << Request.requestString() << newline;


   TSortedStringMap GetVariables;
   {
     //we sort it because the output should be consistent between platforms
     //hashtables guarantee nothing in terms of iteration order
     HTTPvarMap GetVariablesUnsorted;
     HTTPextractGetVariables(Request, &GetVariablesUnsorted);
     sortFromMap(GetVariablesUnsorted,GetVariables);
   }

   Stream << COL_T("COUNT_OF_GET_VARS=") << GetVariables.size() << newline;
   for (COLmapPlace GetIndex=GetVariables.first();
        GetIndex != NULL;
        GetIndex=GetVariables.next(GetIndex))
   {
      Stream << GetVariables.key(GetIndex) << '=' << GetVariables[GetIndex] << newline;
   }


   COLarray<HTTPfile> FileList;
   TSortedStringMap PostVariableList;
   {
      HTTPvarMap PostVariableListUnsorted;
      switch (HTTPextractRequestType(Request)) {
         case HTTP_POST:
            HTTPextractPostVariables(pBodyData, SizeOfBody, &PostVariableListUnsorted);
            break;
         case HTTP_POST_MULTIPART:
            HTTPextractMultiPartData(pBodyData,SizeOfBody, HTTPgetBoundary(Request), &PostVariableListUnsorted, &FileList);
            break;
         default:
            break;
      }
      //we sort it because the output should be consistent between platforms
      //hashtables guarantee nothing in terms of iteration order
      sortFromMap(PostVariableListUnsorted, PostVariableList);
   }

   Stream << COL_T("COUNT_OF_POST_VARS=") << PostVariableList.size() << newline;
   for (COLmapPlace PostIndex=PostVariableList.first();
        PostIndex != NULL;
        PostIndex=PostVariableList.next(PostIndex))
   {
      Stream << PostVariableList.key(PostIndex) << '=' << PostVariableList[PostIndex] << newline;
   }

   TSortedStringMap CookieList;
   {
      TStringMap CookieListUnsorted;
      HTTPextractCookies(Request, CookieListUnsorted);
      sortFromMap(CookieListUnsorted,CookieList);
   }
   Stream << COL_T("COUNT_OF_COOKIES=") << CookieList.size() << newline;
   for (COLmapPlace CookieIndex=CookieList.first();
        CookieIndex != NULL;
        CookieIndex=CookieList.next(CookieIndex))
   {
      Stream << CookieList.key(CookieIndex) << '=' << CookieList[CookieIndex] << newline;
   }

   if (FileList.size() > 0)
   {
      Stream << COL_T("COUNT_OF_FILE=") << FileList.size() << newline;
      for (COLuint32 FileIndex=0; FileIndex < FileList.size(); FileIndex++)
      {
         Stream << COL_T("File[") << FileIndex << ']' << newline;
         Stream << COL_T(" Name=") << FileList[FileIndex].name() << newline
                << COL_T(" MimeType=") << FileList[FileIndex].mimeType() << newline
                << COL_T(" Size=") << FileList[FileIndex].size() << newline;
      }
   }
}

void HTTPextractAllVariables(
   HTTPvarMap* pVariableTable,
   const HTTPrequest& Request,
   const void* pBodyData,
   COLuint32 SizeOfBody,
   bool NormalizeKeys
)
{
   switch (HTTPextractRequestType(Request)) {
      case HTTP_POST:
         HTTPextractPostVariables(pBodyData, SizeOfBody, pVariableTable, NormalizeKeys);
         break;
      case HTTP_POST_MULTIPART:
         COL_ERROR_STREAM_PLAIN("This is multi-part form request.  This is meant to be a plain form.", COLerror::PreCondition);
         break;
      default:
         break;
   }
   HTTPextractGetVariables(Request, pVariableTable, NormalizeKeys);
}


// This routine doesn't work very well for showing parsing problems involving special characters
// like \r and \n
// If that gets important then we should probably take the care to enumerate the error numbers coming from
// HTTPheaderParser and write specialized code to display the errors.  To be honest it's not that important.
void HTTPshowParseErrorLocation(const char* pData, COLuint32 SizeOfData, COLuint32 ErrorLocation, COLostream& Stream)
{
   // TODO haven't make any attempt to optimize this code
   COLuint32 LastNewLine = 0;
   COLuint32 CharIndex;
   for (CharIndex=0; CharIndex < ErrorLocation; CharIndex++)
   {
      Stream << pData[CharIndex];
      if (pData[CharIndex] == '\r')
      {
         LastNewLine = CharIndex;
      }
   }
   // print remainder of line
   while (CharIndex < SizeOfData && pData[CharIndex] != '\r' && pData[CharIndex] != '\n')
   {
      Stream << pData[CharIndex];
      CharIndex++;
   }
   Stream << newline;
   for (CharIndex=LastNewLine; CharIndex < ErrorLocation; CharIndex++)
   {
      Stream << ' ';
   }
   Stream << '^' << newline;
}


// Generates a plain error response - can be a handy utility function
COLstring HTTPgeneratePlainErrorResponse(const COLstring& ErrorMessage)
{
   COLstring ResponseData;
   HTTPresponse Response;
   HTTPsetupStringEntity(Response, ErrorMessage, "text/plain");
   COLostream ResponseStream(ResponseData);

   HTTPgenerateResponse(ResponseStream, Response);
   ResponseStream << ErrorMessage;
   return ResponseData;
}

COLstring HTTPserializeRequestToForm
(
   const COLstring& Location,
   const COLhashmap<COLstring, COLstring, COLhash<COLstring> >& VariableList
)
{
   COLstring Result;
   for (COLhashmapPlace VariableIndex=VariableList.first();
        VariableIndex != NULL;
        VariableIndex = VariableList.next(VariableIndex))
   {
      Result += "<input type='hidden' name='var:";
      Result += VariableList.key(VariableIndex);
      Result += "' value='";
      Result += VariableList[VariableIndex];
      Result += "'>\n";
   }
   Result += "<input name='location' type='hidden' value='";
   Result += Location;
   Result += "'>\n";
   // TODO we could put in the all the HTTP headers if we needed to - we probably don't need to
   return Result;
}

// This is the reverse of the above function
void HTTPdeserializeRequestFromForm
(
   const COLhashmap<COLstring, COLstring>& RequestVariables,
   COLstring& Location,
   COLhashmap<COLstring, COLstring, COLhash<COLstring> >& VariableList
)
{
   if (RequestVariables.find("location"))
   {
      Location = RequestVariables["location"];
   }
   COLstring Prefix;
   COLstring VariableName;
   for (COLhashmapPlace VariableIndex=RequestVariables.first();
        VariableIndex != NULL;
        VariableIndex = RequestVariables.next(VariableIndex))
   {
      if (RequestVariables.key(VariableIndex).split(Prefix, VariableName, ":"))
      {
         if (Prefix.compare("var")==0)
         {
            VariableList[VariableName] = RequestVariables[VariableIndex];
         }
      }
   }
}

bool HTTPparseUrl(const COLstring& UrlIn, bool* pIsHTTPS, COLstring* pHost, COLstring* pPath, int* pPort){
   COL_FUNCTION(HTTPparseUrl);
   COL_VAR(UrlIn);
   COLstring Url = UrlIn.strip(COLstring::Both);
   COL_VAR(Url);
   if (Url.size() < 5){
      *pHost = "URL is too short";
      return false;
   }
   if (Url.substr(0,4) != "http"){
      *pHost = "URL is should start with http";
      return false;
   }
   *pIsHTTPS = Url.substr(0,5) == "https";
   *pPort = *pIsHTTPS ? 443 : 80;

   COL_VAR(*pIsHTTPS);
   Url = Url.substr(*pIsHTTPS ? 8 : 7);
   COL_VAR(Url);
   COLstring Host;
   Url.split(Host, *pPath, "/");
   COL_VAR2(Host, *pPath);
   *pPath = "/" + *pPath;
   COLstring PortString;
   Host.split(*pHost, PortString, ":");
   COL_VAR2(*pHost, PortString);
   if (PortString.size() > 0){
      COL_TRC("Found port");
      *pPort = ::atoi(PortString.c_str());
   }
   return true;
}
