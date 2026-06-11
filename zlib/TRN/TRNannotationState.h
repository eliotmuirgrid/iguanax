#ifndef __TRN_ANNOTATION_STATE_H__
#define __TRN_ANNOTATION_STATE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNannotationState
//
// Description:
//
// TRNannotationState - receives callbacks back from Lua.
//
// Author: Eliot Muir
// Date:   Thursday, November 18th, 2010 @ 02:17:15 PM
//---------------------------------------------------------------------------
#include <memory> // shared ptr
#include <LUA/LUAheader.h>

#include <COL/COLdateTime.h>
#include <COL/COLevent.h>
#include <COL/COLhashmap.h>
#include <COL/COLlist.h>
#include <COL/COLvar.h>

#include <TRN/TRNargumentRequest.h>
#include <TRN/TRNframe.h>
#include <TRN/TRNfunction.h>
#include <TRN/TRNannotationCleanup.h>
#include <TRN/TRNreturnInfo.h>
#include <NOD/NODaddress.h>

class TRNfunction;
class TRNfunctionCall;

class TRNannotationState{
public:
   TRNannotationState();
   ~TRNannotationState();

   COLstring m_CurrentFile;

   COLhashmap<COLstring, TRNarg> m_FullArgumentCache;
   COLarray<TRNargRequest> m_FullArgumentRequests;
   NODaddress m_LastDestination;
   NODaddress m_LastSource;

   COLlist<TRNframe> m_Stack;

   // Stack of function address hashes.  We have to use this
   // instead of the m_Stack, because we need stack levels for
   // Lua functions and C functions (m_Stack is only for Lua functions).
   COLlist<COLstring> m_HashStack;

   TRNreturnInfo m_MostRecentReturnInfo;

   bool m_IsIntellisense;
   // In the format (regex-style): (/funcname)+(#callcount)?
   // The callcount will be omitted if it is 1.
   COLstring m_CurrentAddress;
   // Used for loop support
   bool m_IgnoreNextReturn;
   COLstring m_ErrorFile;
   int m_ErrorLine;
   // these used to be TRNprogram members
   COLstring m_ProjectPath;
   COLstring m_TranslatorGuid;
   COLhashmap<COLstring, TRNfile*>      m_LuaFileList;
   COLhashmap<COLstring, TRNcfunction*> m_CFunctionList;
   COLhashmap<const void*, COLstring, COLhashPointer<const void*> > m_LFunctionNameMap;
   // end TRNprogram members

   COLhashmap<COLstring, COLstring> m_HelpMap;  // Came from TRNannotationJob
   bool m_DevMode; 

   COLstring m_SampleData;
   COLstring m_SampleFunction;

   // The stack depth of the last call event.  If this doesn't match m_Stack's size,
   // this is a sign that an onLFunctionParams event should be ignored.
   int m_LastCallStackDepth;

   // Used by the connection check logic and lua script timeout logic
   COLtimeStamp m_TimeStamp;
   COLuint64    m_WebConnectionId;
   std::shared_ptr<COLevent> m_pEvent;
   // COLevent     m_Event;
   bool         m_Cancelled;
   bool         m_AnnotationsEnabled;  // Switch to false to turn off annotations.
   // To keep track of whether we need to clean up luacom
   bool         m_UsesLuacom = false;
private:

   TRNannotationState(const TRNannotationState& Orig); // not allowed
   TRNannotationState& operator=(const TRNannotationState& Orig); // not allowed
};

void TRNannotationStateSet(lua_State* L, TRNannotationState* pState);
TRNannotationState* TRNgetAnnotationState(lua_State* L);

void TRNannotationStateSetLuacom(TRNannotationState* pState);

TRNfunctionCall* TRNfindCaller(COLlist<TRNframe>& Stack, int* pCallerLine);
void TRNhtmlCreateFunctionCall(lua_State* L, int CountOfParam, TRNfunctionCall* pCaller, TRNfunctionCall* pCallee, int CallerLine, const COLstring& CalleeAddress);

// Returns true if the associated Lua state is in a troublesome state.  This means
// we've stepped inside the first function, but have not received the parameters
// for it.  We encounter problems (#18259) if we reuse a Lua state like this.
// 
// This isn't used anywhere for now, but once we start caching lua states, we need to call this function
// when determining whether we need to compile the lua state again. If true, a compile is needed.
// See Iguana 6: ANNannotationJob::compileProgram() and how it does the check. - Vismay
bool TRNinTroublesomeState(const COLlist<TRNframe>& Stack);
void TRNdumpCallStackVar(COLvar* pCallStack, const COLlist<TRNframe>& Stack, const COLlist<COLstring>& HashStack);

// Callback for when Lua's error() function is called with a specified level.
// After a pcall has returned, call clearErrorCall(), since pcall handles it.
void TRNonErrorCall(COLstring& ErrorFile, int& ErrorLine, COLlist<TRNframe>& Stack, int Level);
void TRNclearErrorCall(COLstring* pErrorFile, int& ErrorLine);

// The file/line in which an error should be reported.
// If a location was stored by calling onErrorCall(), LineNumber and FileName
// will be set to that.
// Otherwise we will go up the stack to find the last stack frame which is in
// a file known by the TRNprogram.  If this is not the top of the stack, the
// error was thrown in an external file.  In this case, we will return false.
// Returns true in all other cases.
bool TRNgetErrorFileAndLine(const COLstring& ErrorFile, int ErrorLine, const COLlist<TRNframe>& Stack, int& LineNumber, COLstring& FileName, bool CompileError);

void TRNpopFrame(COLstring* pCurrentAddress, COLlist<TRNframe>& Stack, COLlist<COLstring>& HashStack);

void TRNeventOnLine(COLlist<TRNframe>& m_Stack, int Line);
void TRNeventAppendAddress(const COLstring& Address, COLstring* pCurrentAddress, COLlist<TRNframe>& Stack, COLlist<COLstring>& HashStack);
void TRNeventRemoveLastAddress(COLstring* pCurrentAddress, COLlist<COLstring>& HashStack);

void TRNgetFunctionNameAndAddress(const COLhashmap<COLstring, TRNfile*>& LuaFileList, COLhashmap<const void*, COLstring, COLhashPointer<const void*> >& LFunctionNameMap,
                                  lua_State* L, lua_Debug* pD, COLstring& FunctionName, COLstring& FunctionAddress);


int  TRNcurrentTopStackLevel(COLlist<TRNframe>& Stack);

TRNfunctionCall* TRNcurrentFunctionCall(COLlist<TRNframe>& Stack);
int TRNcurrentLine(COLlist<TRNframe>& Stack);

const COLstring& TRNcurrentAddress(const COLlist<COLstring>& HashStack);

void TRNreduceCallStackDepth(unsigned int Depth, COLlist<TRNframe>* pStack, COLlist<COLstring>* pHashStack, COLstring* pCurrentAddress);

void TRNresetAnnotationState(TRNannotationState* pHandler);
void TRNannotationResetMaps (TRNannotationState* pState);

void TRNstreamFcout(
   COLostream& Stream,
   const COLstring& DisplayText,
   const char* ElementClass, // "fc" for function calls, "fr" for function returns.
   TRNfunctionCall* pCaller,
   TRNfunctionCall* pCallee,
   int CallerLine,
   const COLstring& FunctionAddress
);

// returns true if we had targets for functions which were unreachable.
bool TRNprogramCheckUnreachableBlocks(COLhashmap<COLstring, TRNfile*>& LuaFileList);

COLostream& operator<<(COLostream& Stream, const COLhashmap<COLstring, TRNfile*>& LuaFiles);
#endif // end of defensive include