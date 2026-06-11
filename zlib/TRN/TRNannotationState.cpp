//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNannotationState
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Thursday, November 18th, 2010 @ 02:17:15 PM
//---------------------------------------------------------------------------
#include <TRN/TRNannotationState.h>
#include <TRN/TRNannotationLuaFunction.h>
#include <TRN/TRNannotationCFunction.h>
#include <TRN/TRNcfunction.h>
#include <TRN/TRNargumentFormat.h>
#include <TRN/TRNfile.h>
#include <TRN/TRNargumentMatchIterator.h>
#include <TRN/TRNargumentPopulateCache.h>
#include <TRN/TRNfileFind.h>
#include <TRN/TRNconstant.h>
#include <TRN/TRNhtmlCreateFunctionCall.h>

#include <SFI/SFImd5.h>

#include <SFI/SFIhtmlEscape.h>

#include <LUA/LUAutils.h>
#include <LUA/LUAerror.h>

#include <LUAL/LUALutils.h>
#include <REX/REXfind.h>

#include <COL/COLsinkString.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static const COLstring TRN_ANONYMOUS_FUNCTION_ADDRESS = "(*)";

TRNannotationState::TRNannotationState()
 : m_LastCallStackDepth(0),
   m_ErrorLine(-1),
   m_IgnoreNextReturn(false),
   m_DevMode(true),
   m_IsIntellisense(false), 
   m_Cancelled(false),
   m_AnnotationsEnabled(true)
{
   COL_METHOD(TRNannotationState);
   m_pEvent = std::make_shared<COLevent>();
}

TRNannotationState::~TRNannotationState(){
   COL_METHOD(~TRNannotationState);
   TRNannotationResetMaps(this);
}

void TRNannotationResetMaps(TRNannotationState* pState){
   COL_FUNCTION(TRNannotationResetMaps);
   TRNcleanUpFiles(pState->m_LuaFileList);
   TRNcleanUpCFunctions(pState->m_CFunctionList);
   pState->m_LuaFileList.clear();
   pState->m_CFunctionList.clear();
   pState->m_LFunctionNameMap.clear();
}

void TRNannotationStateSet(lua_State* L, TRNannotationState* pState){
   COL_FUNCTION(TRNannotationStateSet);
   LUApushString(L,TRN_ANNOTATION_STATE);
   lua_pushlightuserdata(L, (void*)pState);
   lua_settable(L,LUA_REGISTRYINDEX);
}

TRNannotationState* TRNgetAnnotationState(lua_State* L){
   COL_FUNCTION(TRNgetAnnotationState);
   LUApushString(L, TRN_ANNOTATION_STATE);
   lua_gettable(L, LUA_REGISTRYINDEX);
   TRNannotationState* pState = (TRNannotationState*)lua_touserdata(L, -1);
   lua_pop(L,1);  // reset the stack back to normal
   COLPRECONDITION(pState != NULL);
   return pState;
}

void TRNannotationStateSetLuacom(TRNannotationState* pState){
   COL_FUNCTION(TRNannotationStateSetLuacom);
#ifdef _WIN32
   pState->m_UsesLuacom = true;
   COL_TRC("Luacom in use, check when closing to make sure resources are freed.");
#else 
   pState->m_UsesLuacom = false;
   COL_TRC("Not on Windows - set to false.");
#endif
}

void TRNeventOnLine(COLlist<TRNframe>& m_Stack, int Line){
   COL_FUNCTION(TRNeventOnLine);
   COLPRECONDITION(m_Stack.size() > 0);
   
   TRNframe& CurrentFrame = m_Stack[m_Stack.last()];
   CurrentFrame.m_CurrentLine = Line;
   COL_TRC(CurrentFrame.m_Current.m_Name << ':' << Line);
   TRNfunctionCall& CurrentCall = CurrentFrame.m_Current;
   if (CurrentCall.m_pFunction && TRNfunctionCallIsTargetCall(CurrentCall)){
      COL_DBG("TRNfunction " << ((void*)CurrentCall.m_pFunction) << " executing line " << Line);
      TRNfunctionOnLine(CurrentCall.m_pFunction,Line);
   }
}

TRNfunctionCall* TRNcurrentFunctionCall(COLlist<TRNframe>& Stack){
   COL_FUNCTION(TRNcurrentFunctionCall);
   return (Stack.size() > 0) ? &Stack[Stack.last()].m_Current : NULL;
}

int TRNcurrentLine(COLlist<TRNframe>& Stack){
   COL_FUNCTION(TRNcurrentLine);
   return Stack.size() > 0 ? Stack[Stack.last()].m_CurrentLine : -1;
}

static COLstring TRNgetLine(const COLstring& Source, int LineNumber){
   COL_FUNCTION(TRNgetLine);
   int CurrentLine = 1;
   int CurrentLineStart = 0;
   while (CurrentLine++ < LineNumber) {
      if (!Source.find(&CurrentLineStart, '\n', CurrentLineStart)){
         return "";
      }
      CurrentLineStart++;
   }

   int CurrentLineEnd = Source.size();
   Source.find(&CurrentLineEnd, '\n', CurrentLineStart);
   return Source.substr(CurrentLineStart, CurrentLineEnd - CurrentLineStart);
}

// Currently only handles function definitions in the following format:
//  [local] function [tablename.]functionname
static bool TRNparseFunctionNameFromSource(lua_State* L, lua_Debug* pD, const COLhashmap<COLstring, TRNfile*>& LuaFileList, COLstring* pFunctionName){
   COL_FUNCTION(TRNparseFunctionNameFromSource);
   if (pD->source && pD->linedefined > 0){
      const TRNfile* pFile = TRNfileFind(LuaFileList, pD->source);
      if (pFile){
         COLstring Line = TRNgetLine(pFile->m_Source, pD->linedefined);
         COL_VAR2(pD->linedefined, Line);
         COLstring FinderRegex = 
            "[ \\t]*(local[ \\t]+)?function[ \\t]+(\\w+\\.)?(\\w+)";
         REXfind Finder;
         Finder.init(FinderRegex.c_str());
         Finder.matchBegin(Line.c_str(), Line.size());
         if (Finder.matchNext()){
            REXresult Match = Finder.match(3);
            *pFunctionName = COLstring(Match.pData, Match.Size);
            return true;
         }
      }
   }
   return false;
}

void TRNgetFunctionNameAndAddress(const COLhashmap<COLstring, TRNfile*>& LuaFileList, COLhashmap<const void*, COLstring, COLhashPointer<const void*> >& LFunctionNameMap,
                                  lua_State* L, lua_Debug* pD, COLstring& FunctionName, COLstring& FunctionAddress){
   COL_FUNCTION(TRNgetFunctionNameAndAddress);
   if (!pD->name || strlen(pD->name) == 0 || strcmp(pD->name, "?") == 0){
      COLhashmapPlace FunctionPlace =  TRNfindLuaFunctionName(LFunctionNameMap, lua_topointer(L, -1));
      if (FunctionPlace){
         FunctionName = TRNluaFunctionName(LFunctionNameMap, FunctionPlace);
         FunctionAddress = FunctionName;
      } else if (TRNparseFunctionNameFromSource(L, pD, LuaFileList, &FunctionName)){
         FunctionAddress = FunctionName;
      } else {
         COLostream FunctionNameStream(FunctionName);
         FunctionNameStream << pD->source << ':' << pD->linedefined;
         FunctionAddress = TRN_ANONYMOUS_FUNCTION_ADDRESS;
      }
   } else {
      FunctionName = pD->name;
      FunctionAddress = FunctionName;
   }
}

// TODO ?  Change into two functions ?  
TRNfunctionCall* TRNfindCaller(COLlist<TRNframe>& Stack, int* pCallerLine){
   COL_FUNCTION(TRNfindCaller);
   TRNfunctionCall* pCaller = NULL;
   *pCallerLine = -1;
   if (Stack.size() > 1){
      COLlistPlace SecondLastPlace = Stack.previous(Stack.last());
      TRNframe& Caller = Stack[SecondLastPlace];
      pCaller = &Caller.m_Current;
      *pCallerLine = Caller.m_CurrentLine;
   }
   return pCaller;
}

const COLstring& TRNcurrentAddress(const COLlist<COLstring>& HashStack){
   COL_FUNCTION(TRNcurrentAddress);
   COLPRECONDITION(HashStack.size() > 0);
   return HashStack[HashStack.last()];
}

bool TRNinTroublesomeState(const COLlist<TRNframe>& Stack){
   COL_FUNCTION(TRNinTroublesomeState);
   return Stack.size() == 1 && !Stack[Stack.last()].m_ReceivedParams;
}

void TRNdumpCallStackVar(COLvar* pCallStack, const COLlist<TRNframe>& Stack, const COLlist<COLstring>& HashStack){
   COL_FUNCTION(TRNdumpCallStackVar);
   pCallStack->setArrayType();

   COLlistPlace TargetAddressPlace = HashStack.first();
   if (TargetAddressPlace) TargetAddressPlace = HashStack.next(TargetAddressPlace);
   // TargetAddressPlace is the address of the next function call in the stack frame.

   for (COLlistPlace i = Stack.first(); i; i = Stack.next(i)){   
      const TRNframe& Frame = Stack[i];
      COLvar FrameTrace;

      const TRNfunction* pFunc = Frame.m_Current.m_pFunction;
      if (pFunc) {
         FrameTrace["f"] = pFunc->m_pFile->m_Label;
      }
      FrameTrace["fd"] = (pFunc ? pFunc->m_pFile->m_Label : Frame.m_pSource);
      FrameTrace["s"] = (pFunc ? pFunc->m_Start : 0);
      FrameTrace["c"] = Frame.m_Current.m_Call;
      FrameTrace["l"] = Frame.m_CurrentLine;
      FrameTrace["n"] = Frame.m_Current.m_Name;
      if (TargetAddressPlace){
         FrameTrace["t"] = HashStack[TargetAddressPlace];
         TargetAddressPlace = HashStack.next(TargetAddressPlace);
      } else {
         // We can use 'last' here instead of a real target address because if the target is not found
         // (as 'last' will not be) then we display the last iteration of every loop (which will, in
         // theory, be the iteration state in which the error was encountered).
         FrameTrace["t"] = "last";
      }
      pCallStack->push_back(FrameTrace);
   }
}

void TRNonErrorCall(COLstring& ErrorFile, int& ErrorLine, COLlist<TRNframe>& Stack, int Level){
   COL_FUNCTION(TRNonErrorCall);
   COLlistPlace Place = Stack.last();
   while (Place && --Level){
      Place = Stack.previous(Place);
   }
   if (Place){
      const TRNframe& ErrorFrame = Stack[Place];
      const TRNfunction* pFunc = ErrorFrame.m_Current.m_pFunction;
      ErrorFile = (pFunc ? pFunc->m_pFile->m_Label : "");
      ErrorLine = ErrorFrame.m_CurrentLine;
   }
}

void TRNclearErrorCall(COLstring* pErrorFile, int& ErrorLine){
   COL_FUNCTION(TRNclearErrorCall);
   pErrorFile->clear();
   ErrorLine = -1;
}

bool TRNgetErrorFileAndLine(const COLstring& ErrorFile, int ErrorLine, const COLlist<TRNframe>& Stack, int& LineNumber, COLstring& FileName, bool CompileError){
   COL_FUNCTION(TRNgetErrorFileAndLine);
   if(CompileError){
      COL_TRC("Compile Error");
      LineNumber = -1;
      FileName   = "";
      return true;
   }
   if (!ErrorFile.is_null() && ErrorLine != -1){
      LineNumber = ErrorLine;
      FileName   = ErrorFile;
      return true;
   } else {
      for (COLlistPlace Place = Stack.last(); Place; Place = Stack.previous(Place)){
         if (const TRNfunction* pFunc = Stack[Place].m_Current.m_pFunction){
            LineNumber = Stack[Place].m_CurrentLine;
            FileName   = pFunc->m_pFile->m_Label;
            return Place == Stack.last();
         }
      }
      // We haven't found any "known" location on the entire stack.
      LineNumber = -1;
      FileName = "";
      return true;
   }
}

void TRNeventAppendAddress(const COLstring& Address, COLstring* pCurrentAddress, COLlist<TRNframe>& Stack, COLlist<COLstring>& HashStack){
   COL_FUNCTION(TRNeventAppendAddress);
   pCurrentAddress->append("/", 1);
   pCurrentAddress->append(Address);
   // Add #<count> if this is not the first time the function has been called in this scope.
   if (Stack.size() > 0){
      COLhashmap<COLstring, int>& CallCountMap = Stack[Stack.last()].m_CallCountMap;
      COLhashmapPlace CallCountPlace = CallCountMap.find(Address);
      if (CallCountPlace){
         pCurrentAddress->append("#", 1);
         pCurrentAddress->append(COLintToString(++CallCountMap[CallCountPlace]));
      } else {
         CallCountMap.add(Address, 1);
      }
   }
   HashStack[HashStack.add()] = SFImd5(*pCurrentAddress);
}

void TRNpopFrame(COLstring* pCurrentAddress, COLlist<TRNframe>& Stack, COLlist<COLstring>& HashStack){
   COL_FUNCTION(TRNpopFrame);
   COL_TRC("Popping frame " << Stack.size() <<", function name: " << Stack.back().m_Current.m_Name);
   Stack.remove(Stack.last());
   TRNeventRemoveLastAddress(pCurrentAddress, HashStack);
}

void TRNreduceCallStackDepth(unsigned int Depth, COLlist<TRNframe>* pStack, COLlist<COLstring>* pHashStack, COLstring* pCurrentAddress){
   COL_FUNCTION(TRNreduceCallStackDepth);
   COLPRECONDITION(Depth >= 0);
   while (pStack->size() > Depth){
      TRNpopFrame(pCurrentAddress, *pStack, *pHashStack);
   }
}

void TRNeventRemoveLastAddress(COLstring* pCurrentAddress, COLlist<COLstring>& HashStack){
   COL_FUNCTION(TRNeventRemoveLastAddress);
   COL_VAR(*pCurrentAddress);
   size_t LastSep = pCurrentAddress->rfind("/");
   COLPRECONDITION(LastSep != npos);
   pCurrentAddress->setSize(LastSep);
   COL_VAR(*pCurrentAddress);

   COLPRECONDITION(HashStack.size() > 0);
   COL_VAR2(HashStack.size(), HashStack.last());
   HashStack.remove(HashStack.last());
}

int TRNcurrentTopStackLevel(COLlist<TRNframe>& Stack){
   COL_FUNCTION(TRNcurrentTopStackLevel);
   for (COLlistPlace P = Stack.last(); P; P = Stack.previous(P)){
      int InternalLevel = Stack[P].m_InternalLevel;
      if (InternalLevel != -1){
         return InternalLevel;
      }
   }
   return -1; // likely the stack is empty
}

bool TRNprogramCheckUnreachableBlocks(COLhashmap<COLstring, TRNfile*>& LuaFileList){
   COL_FUNCTION(TRNprogramCheckUnreachableBlocks);
   bool UnreachableBlocks = false;
   for(auto i = LuaFileList.cbegin(); i; ++i){
      if (TRNcheckUnreachableBlocks(i->second)){
         COL_TRC("One or more unreachable blocks exist");
         UnreachableBlocks = true;
      }
   }
   return UnreachableBlocks;
}
