// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: WEBhandle
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Friday 19 May 2023 - 02:21PM
// ---------------------------------------------------------------------------
#include <WEB/WEBhandle.h>
#include <WEB/WEBrequest.h>
#include <WEB/WEBrequestData.h>

#include <SFI/SFIuriUnescapeFilter.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

static void WEBhandleAddArg(const COLstring& Key, const COLstring& Value, WEBrequestData* pHandlePath) {
   COL_FUNCTION(WEBhandleAddArg);
   COL_VAR2(Key, Value);
   COLstring UnescapedKey(Key);
   COLstring UnescapedValue(Value);
   // Unescaping in two separate try as either SFIuriUnescape call could throw and we want to take the unescaped value if possible 
   try {
      // It's best to normalize web headers to lower case.
      UnescapedKey = SFIuriUnescape(Key).toLowerCase();
   } catch (const COLerror& Error){
      COL_TRC("Invalid \%xx sequence in key, taking the raw key");
      UnescapedKey = UnescapedKey.toLowerCase();
   }
   try {
      UnescapedValue = SFIuriUnescape(Value);
   } catch (const COLerror& Error){
      COL_TRC("Invalid \%xx sequence in value, taking the raw value");
      UnescapedValue = Value;
   }
   pHandlePath->Args.add(UnescapedKey, UnescapedValue);
}

static void WEBhandleParsePathArgs(const COLstring& ArgString, WEBrequestData* pHandlePath) {
   COL_FUNCTION(WEBhandleParsePathArgs);
   COLstring Arg;
   COLstring Rest = ArgString;
   COLstring Key, Value;
   while(Rest.split(Arg, Rest, "&")) {
      Arg.split(Key, Value, "=");
      WEBhandleAddArg(Key, Value, pHandlePath);
   }
   if(Arg.split(Key, Value, "=")) { WEBhandleAddArg(Key, Value, pHandlePath); }
}

static void WEBhandleParseRequestLine(const COLstring& RequestLine, WEBrequestData* pHandlePath) {
   COL_FUNCTION(WEBhandleParseRequestLine);
   COLstring Method, Rest, Path, Version, ArgList, Temp;
   RequestLine.split(Method, Rest, " ");
   COL_VAR(Rest);
   Rest.rsplit(Temp, Version, " ");
   Temp.split(Path, ArgList, "?");
   COL_VAR2(Path, ArgList);
   pHandlePath->Path = Path;
   WEBhandleParsePathArgs(ArgList, pHandlePath);
}

void WEBrequestParse(const COLstring& Input, WEBrequestData* pOut){
   COL_FUNCTION(WEBrequestParse);
   COLstring RequestLine, Headers;
   Input.split(RequestLine, Headers, "\n");
   RequestLine.setSize(RequestLine.size()-1);
   COL_VAR(RequestLine);
   pOut->Args.clear();
   WEBhandleParseRequestLine(RequestLine, pOut);
   COL_VAR(Headers);
   COL_DUMP("Headers:", Headers.c_str(), Headers.size());
   int Pos;
   Headers.find(&Pos,"\r\n\r\n");
   pOut->Headers.clear();
   WEBrequestParseHeaders(Headers.c_str(), Headers.c_str() + Pos+2, &pOut->Headers);
   COL_VAR(pOut->Headers);
}
