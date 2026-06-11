//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNargumentFormat
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Saturday, November 20th, 2010 @ 02:38:37 PM
//---------------------------------------------------------------------------
#include "TRNargumentFormat.h"
#include "TRNconstant.h"
#include <THTM/THTMprinter.h>
#include <LND/LNDutils.h>
#include <LUA/LUAutils.h>
#include <LUAL/LUALprint.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

const int TRN_MAX_STRLEN = 90;

static void TRNstreamId(int i, const COLstring& FunctionAddress, bool IsReturn, COLostream& Stream){
   Stream << FunctionAddress << '-' << i << (IsReturn ? "-r" : "-a");
}

// Find the next UTF-8 character from a given point. If s points
// to the end of a string, it is returned instead.
static const char* TRNnextChar(const char* s){
   // Examine subsequent characters until we find an ASCII
   // character or the beginning of a multi-byte.
   if(*s) while((*++s & 0xC0) == 0x80) { continue; }
   return s;
}

static inline void TRNopenArgSpan(const char* pClass, int DisplayIndex, const COLstring& FunctionAddress, bool IsReturn, COLostream& Stream){
   Stream << COL_T("<span class=\'") << pClass << COL_T("\' id=\'");
   TRNstreamId(DisplayIndex, FunctionAddress, IsReturn, Stream);
   Stream << COL_T("\'>");
}

static inline void TRNcloseArgSpan(COLostream& Stream){
   Stream << COL_T("</span>");
}

static void TRNformatLongString(lua_State* L, int i, int DisplayIndex, const COLstring& FunctionAddress, bool IsReturn, COLostream& Stream){
   COL_FUNCTION(TRNformatLongString);
   // beware lua_tostring converts numbers into strings.
   size_t len = 0; 
   const char* pData = lua_tolstring(L, i, &len);
   TRNopenArgSpan("pv lt", DisplayIndex, FunctionAddress, IsReturn, Stream);
   Stream << '\'';
   const char* pEnd = TRNnextChar(pData + COL_MIN(len, TRN_MAX_STRLEN));
   COL_DUMP("String:", pData, pEnd-pData);
   THTMprintEscapedLiteral(Stream, pData, pEnd-pData);
   if (len > TRN_MAX_STRLEN){
      COL_TRC("It was a long string so putting in ellisis");
      Stream << COL_T("<span class=\'el\'>&hellip;</span>");
   }
   Stream << '\'';
   TRNcloseArgSpan(Stream);
}

static void TRNformatNumber(lua_State* L, int i, int DisplayIndex, const COLstring& FunctionAddress, bool IsReturn, COLostream& Stream){
   TRNopenArgSpan("pv nm", DisplayIndex, FunctionAddress, IsReturn, Stream);
   LUAformatNumber(L, i, Stream);
   TRNcloseArgSpan(Stream);
}

static void TRNformatKeyword(const char* pKeyword, int DisplayIndex, const COLstring& FunctionAddress, bool IsReturn, COLostream& Stream){
   TRNopenArgSpan("pv kw", DisplayIndex, FunctionAddress, IsReturn, Stream);
   Stream << pKeyword;
   TRNcloseArgSpan(Stream);
}

static void TRNformatMisc(const char* pText, int DisplayIndex, const COLstring& FunctionAddress, bool IsReturn, COLostream& Stream){
   TRNopenArgSpan("pv", DisplayIndex, FunctionAddress, IsReturn, Stream);
   Stream << pText;
   TRNcloseArgSpan(Stream);
}

static void TRNformatUserData(lua_State* L, int i, int DisplayIndex, const COLstring& FunctionAddress, bool IsReturn, COLostream& Stream){
   COL_FUNCTION(TRNformatUserData);

   COLstring UserdataLabel;
   int ProtocolType, NodeType;

   if (LNDisNode(L, i)){
      const NODplace* pNode = LNDgetNode(L, i);
      COLPRECONDITION(pNode);
      ProtocolType = pNode->protocolType();
      NodeType = pNode->nodeType();
      COLostream UserdataLabelStream(UserdataLabel);
      THTMprintEscaped(UserdataLabelStream, pNode->name().c_str(), pNode->name().size());
   } else {
      LUALdisplayComplexItem(L, i, &UserdataLabel, NULL, ProtocolType, NodeType, NULL, true);
   }

   Stream << COL_T("<a class=\'ANNnodeTreeValue nt_") << ProtocolType << '_' << NodeType << COL_T("\' id=\'");
   TRNstreamId(DisplayIndex, FunctionAddress, IsReturn, Stream);
   Stream << COL_T("\'>") << UserdataLabel << COL_T("</a>");
}

static void TRNformatTable(lua_State* L, int i, int DisplayIndex, const COLstring& FunctionAddress, bool IsReturn, COLostream& Stream){
   COL_FUNCTION(TRNformatTable);
   COLstring ShortLabel;
   int ProtocolType, NodeType;
   LUALdisplayComplexItem(L, i, &ShortLabel, NULL, ProtocolType, NodeType, NULL, true);
   Stream << COL_T("<a class=\'ANNnodeTreeValue nt_") << ProtocolType << '_' << NodeType << COL_T("\' id=\'");
   TRNstreamId(DisplayIndex, FunctionAddress, IsReturn, Stream);
   Stream << COL_T("\'>") << ShortLabel << COL_T("</a>");
}

static void TRNformatVar(lua_State* L, int i, int DisplayIndex, const COLstring& FunctionAddress, bool IsReturn, COLostream& Stream){
   COL_FUNCTION(TRNformatVar);
   COL_VAR(i);
   int t = lua_type(L,i);
   COL_VAR(lua_typename(L, t));
   switch(t){
      case LUA_TSTRING:        TRNformatLongString (L, i,                           DisplayIndex, FunctionAddress, IsReturn, Stream); break;
      case LUA_TNIL:           TRNformatKeyword    ("nil",                          DisplayIndex, FunctionAddress, IsReturn, Stream); break;
      case LUA_TBOOLEAN:       TRNformatKeyword(lua_toboolean(L, i)?"true":"false", DisplayIndex, FunctionAddress, IsReturn, Stream); break;
      case LUA_TFUNCTION:      TRNformatKeyword("function",                         DisplayIndex, FunctionAddress, IsReturn, Stream); break;
      case LUA_TUSERDATA:      TRNformatUserData   (L, i,                           DisplayIndex, FunctionAddress, IsReturn, Stream); break;
      case LUA_TLIGHTUSERDATA: TRNformatUserData   (L, i,                           DisplayIndex, FunctionAddress, IsReturn, Stream); break;
      case LUA_TTHREAD:        TRNformatMisc       ("thread",                       DisplayIndex, FunctionAddress, IsReturn, Stream); break;
      case LUA_TTABLE:         TRNformatTable      (L, i,                           DisplayIndex, FunctionAddress, IsReturn, Stream); break;
      case LUA_TNUMBER:        TRNformatNumber     (L, i,                           DisplayIndex, FunctionAddress, IsReturn, Stream); break;
      default:
         COLPRECONDITION(1==0);
   }
}

void TRNargumentsFormat(lua_State* L, int Start, int End, const COLstring& FunctionAddress, bool IsReturn, COLostream& Stream){
   COL_FUNCTION(TRNargumentsFormat);
   for (int i=Start;i <= End; i++){
      TRNformatVar(L, i, i-Start+1, FunctionAddress, IsReturn, Stream);
      if (i != End) Stream << ','; 
   }
}
