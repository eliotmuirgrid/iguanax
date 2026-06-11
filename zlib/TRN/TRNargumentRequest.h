#ifndef __TRN_ARGUMENT_REQUEST_H__
#define __TRN_ARGUMENT_REQUEST_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNargumentRequest
//
// Description:
//
// TRNargRequest class.
//
// Author: Eliot Muir
// Date:   Wednesday, November 24th, 2010 @ 01:19:34 PM
//
//---------------------------------------------------------------------------

#include <CHNK/CHNKwriteChunk.h>
#include <CHNK/CHNKviewString.h>
#include <COL/COLhashmap.h>
#include <COL/COLarray.h>
#include <COL/COLvar.h>
#include <NOD/NODplace.h>

#include <BRO/BROenabled.h>  // CONNECTOR BRO code. We need this for BRO_ENABLED. It can be removed later.

class TRNannotationState;

class TRNarg{
public:
   TRNarg();

   COLstring m_OriginalString;        // valid iff !isTree() and the argument is a string
   COLstring m_ViewString;            // valid iff !isTree()
   COLref<NODplace> m_pTree; // valid iff isTree()

   // All strings in m_Chunks are pointers to parts of m_ViewString.
   COLarray<CHNKstringArgChunk> m_Chunks; // TRN becomes new lib name

   COLstring m_Title;
   COLstring m_FileName;
   COLstring m_FunctionName;
   int m_Call;
   int m_Line;
   int m_ProtocolType;
   int m_NodeType;
   CHNKviewMode m_ViewMode; // TRN becomes new lib name
};

bool TRNargIsTree(const TRNarg& Arg);

int TRNargLinesInLastChunk(const TRNarg& Arg);
void TRNargumentRequestClear(TRNarg* pRequest);
// default copy constructor allowed.
class TRNargRequest {
public:
   TRNargRequest();
   COLstring m_Id;
   // The following are the result of splitting the Id.
   COLstring m_FunctionAddress;
   bool m_IsReturn;

   int m_FirstChunk;
   int m_SecondChunk; // chunked if strings get super long
   COLstring m_RootAddressJson;
   COLstring m_ExpansionTreeJson;
   int m_ArgumentIndex; // the parameter or return value number
   CHNKviewMode m_ViewMode;  // HEX encoded, special characters encoded or nothing.
   COLvar m_VarResult; // stores the response for this argument

#ifdef BRO_ENABLED
   COLvar m_BroResult; // stores the response for this argument   // CONNECTOR BRO code.
#endif
};

void TRNargumentRequestSetViewMode(TRNargRequest& Arg, const COLstring& ViewModeString);
// Will parse the three parts of the id: function address, index, and (r|a).
void TRNargumentRequestSetMembersFromId(TRNargRequest& Arg);

void TRNargumentReached        (TRNargRequest& ArgRequest, lua_State* L, int i, const COLstring& FileName, const COLstring& FunctionName, const int Call, const int Line, COLhashmap<COLstring, TRNarg>& ArgCache);
void TRNargumentRequestPopulate(TRNargRequest& ArgRequest, lua_State* L, int i, const COLstring& FileName, const COLstring& FunctionName, const int Call, const int Line, TRNarg& Arg);

COLostream& operator<<(COLostream& Stream, TRNargRequest& ArgRequest);
#endif // end of defensive include
