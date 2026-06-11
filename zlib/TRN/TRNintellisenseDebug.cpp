//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNintellisenseDebug
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Monday, December 20th, 2010 @ 10:20:28 PM
//---------------------------------------------------------------------------
#include <THTM/THTMprinter.h>
#include "TRNintellisenseDebug.h"
#include "TRNintellisenseContext.h"
#include "TRNintellisenseSearch.h"
#include "TRNconstant.h"
#include "TRNtraverser.h"
#include "TRNintellisenseSearchCriteria.h"
#include "TRNisValidLuaName.h"
#include "TRNhtmlEscapeString.h"
#include "TRNvariableAutocompletion.h"
#include "TRNaddSpecificNodeTypeOptions.h"
#include "TRNautoComplete.h"

#include <CACHE/CACHEfile.h>

#include <LND/LNDutils.h>
#include <LND/LNDdebug.h>
#include <LUA/LUAtimeout.h>
#include <LHLP/LHLPhelp.h>
#include <LUAL/LUALutils.h>
#include <NOX/NOXconstant.h>

#include <LUA/LUAutils.h>

#include <REX/REXmatcher.h>

#include <SFI/SFIhtmlCrop.h>
#include <TXT/TXTutils.h>

#include <COL/COLarray.h>
#include <COL/COLauto.h>
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <FIL/FILutils.h>
#include <HELP/HELPmodel.h>
#include <HELP/HELPserialize.h>
COL_LOG_MODULE;

static bool TRNaddTableNodeTreeVariableOption(lua_State* L, int i, COLstring& Output) {
   if (!lua_getmetatable(L, i)) {
      // table based node tree always have a meta-table.
      return false;
   }

   lua_getfield(L, -1, "_rawdata");       // grab _rawdata from element's meta-table
   COLstring Description = lua_tostring(L, -1);
   COL_VAR(Description);
   lua_pop(L, 1);    // pop _rawdata
   lua_pop(L, 1);    // pop meta-table
   if (Description.is_null()) {
      return false;
   }

   if (Description.size() > TRN_INTELLISENSE_MAX_LENGTH) {
      Description = Description.substr(0, TRN_INTELLISENSE_MAX_LENGTH) + "...";
   }
   Output += "(" + Description + ")";
   return true;
}

static void TRNaddVariableOption(
   lua_State* L,
   int i,
   const COLstring& VariableName,
   bool HighPriority,
   COLarray<const void*> TableStack,
   const REXmatcher& SearchRegex,
   TRNsortedOptionsList& Options
){
   COL_FUNCTION(TRNaddVariableOption);
   COL_VAR3(i, VariableName, HighPriority);

   COLauto<TRNtraverser> pTraverser = TRNcreateTraverser(L, i, false, 0);
   TRNintellisenseOption Option(*pTraverser);
   if (HighPriority) Option.Priority = TRNintellisenseOption::HIGH;
   Option.FullText = VariableName;

   COLstring EscapedVariableName;
   if (VariableName.size() > TRN_INTELLISENSE_MAX_LENGTH){
      SearchRegex.regexReplace(Option.DisplayText, VariableName.substr(0, TRN_INTELLISENSE_MAX_LENGTH), TRN_HIGHLIGHT_SEARCH_MATCH, TRNhtmlEscapeString);
      Option.DisplayText.append(TRN_INTELLISENSE_TRUNCATED);
   } else {
      SearchRegex.regexReplace(Option.DisplayText, VariableName, TRN_HIGHLIGHT_SEARCH_MATCH, TRNhtmlEscapeString);
   }
   Option.DisplayText.append(" ", 1);

   if (!TRNaddTableNodeTreeVariableOption(L, i, Option.DisplayText)) {
      pTraverser->writeTreeValue(Option.DisplayText, &Option.FunctionName);
   }
   COL_VAR(Option.DisplayText);

   //add bracketing for functions, examine help to determine bracket type
   if (lua_isfunction(L,i)){
      COL_TRC("We have a function.");
      lua_pushvalue(L,i);
      HELPobject FunctionHelp;
      COLstring HelpFile;
      if (TRNfetchHelpJsonFromFile(L, &FunctionHelp, &HelpFile)){
         COL_VAR(FunctionHelp.ParameterTable);
         Option.FullText += (FunctionHelp.ParameterTable ? '{' : '(');
      } else {
         COL_TRC("Could not find parameter table, defaulting to '('.");
         Option.FullText += '(';
      };
      lua_pop(L,1);
      COL_VAR(Option.FullText);
   }
   Option.LuaKey = VariableName;
   COL_VAR(Option.LuaKey);

   Options.add(Option);
   COL_TRC("About to exit TRNaddVariableOption");
}

// Note: the order in which options are added here is important.  It is the same as the order in
// which Lua will evaluate a variable name.
// For now we only search the variable names, NOT their values.
static void TRNvariableIntellisense(lua_State* L, TRNintellisenseContext* pContext, lua_Debug* pD){
   COLarray<const void*> TableStack;

   REXmatcher SearchRegex;
   int DummyInt = -1;
   TRNintellisenseSearchCriteria(pContext->ParseContext.SearchString, SearchRegex, DummyInt);
   COLindex MatchStart = 0, MatchEnd;

   // First, local variables.
   int LocalIndex = 0;
   const char* pLocalName;
   // Scan to end of locals, so we can go backwards (see #21870).
   while ((pLocalName = lua_getlocal(L, pD, ++LocalIndex))) {
      lua_pop(L, 1); // Pop local variable value
   }
   while ((pLocalName = lua_getlocal(L, pD, --LocalIndex))){
      // temporary local variables start with '('
      if (pLocalName[0] != '(' && SearchRegex.findMatch(pLocalName, 0, MatchStart, MatchEnd)){
         TRNaddVariableOption(L, lua_gettop(L), pLocalName, MatchStart == 0, TableStack, SearchRegex, pContext->Options);
      }
      lua_pop(L, 1); // Pop local variable value
   }

   // Next, upvalues.
   // Note: "upvalue" means a local variable declared in a scope outside this function and REFERENCED IN THIS FUNCTION.
   int UpValueIndex = 1;
   const char* pUpValueName;
   lua_getinfo(L, "f", pD); // pushes current function onto stack
   while ((pUpValueName = lua_getupvalue(L, -1, UpValueIndex++))){
      if (SearchRegex.findMatch(pUpValueName, 0, MatchStart, MatchEnd)){
         TRNaddVariableOption(L, lua_gettop(L), pUpValueName, MatchStart == 0, TableStack, SearchRegex, pContext->Options);
      }
      lua_pop(L, 1); // pop upvalue
   }
   lua_pop(L, 1); // pop function

   // Next, "potential" upvalues.
   const COLlist<COLstring>& Upvalues = pContext->Upvalues;
   for (COLlistPlace UpvaluePlace = Upvalues.first(); UpvaluePlace; UpvaluePlace = Upvalues.next(UpvaluePlace)){
      const COLstring& UpvalueName = Upvalues[UpvaluePlace];
      if (SearchRegex.findMatch(UpvalueName.c_str(), 0, MatchStart, MatchEnd)){
         TRNintellisenseOption Option(LUAL_LUA_PROTOCOL, LUAL_LUA_UNKNOWN_TYPE);
         SearchRegex.regexReplace(Option.DisplayText, UpvalueName, TRN_HIGHLIGHT_SEARCH_MATCH, TRNhtmlEscapeString);
         Option.FullText = UpvalueName; //note that we don't know if this is a function or not, so we can't do what addSimpleOption does
         Option.LuaKey = UpvalueName;
         if (MatchStart == 0) Option.Priority = TRNintellisenseOption::HIGH;
         pContext->Options.add(Option);
      }
   }

   // Global variables.
   lua_pushnil(L); // first key
   while (lua_next(L, LUA_GLOBALSINDEX)){
      // key at index -2, value at index -1
      if (lua_type(L, -2) == LUA_TSTRING){
         COLstring GlobalName = LUAtoString(L, -2);
         if (TRNisValidLuaName(GlobalName)
            && SearchRegex.findMatch(GlobalName.c_str(), 0, MatchStart, MatchEnd)
            && (GlobalName != TRN_GLOBAL_INTELLISENSE_VARIABLE)){
            TRNaddVariableOption(L, lua_gettop(L), GlobalName, MatchStart == 0, TableStack, SearchRegex, pContext->Options);
         }
      }
      lua_pop(L, 1); // removes value; keeps key for next iteration.
   }
}

static void TRNaddShortHelpText(TRNintellisenseOption& Option, const HELPparameter& Param){
   COLostream DisplayTextStream(Option.DisplayText);
   COLstring ShortParamDesc = LHLPhelpTruncateText(Param.Description, 120);

   //note we assume that ShortParamDesc can contain html char
   LHLPhelpAddStyledValue(DisplayTextStream,Param.Description);
}
// Returns the help content and the file from which it was obtained
bool TRNfetchHelpJsonFromFile(lua_State* L, HELPobject* pHelp, COLstring* pHelpFile){
   COL_FUNCTION(TRNfetchHelpJsonFromFile);
   COL_TRC("GETTING HELP");
   COLstring& HelpFile = *pHelpFile;
   const int StartTop = lua_gettop(L);
   COL_VAR(StartTop);
   lua_getglobal(L, "_help");  
   lua_pushvalue(L, -2);
   lua_gettable(L, -2);
   COL_VAR(lua_type(L, -1));
   if (lua_type(L, -1) == LUA_TNIL) { // if function help is not found, return
      COL_TRC("No help found");
      lua_settop(L, StartTop);
      return false;
   }
   HelpFile = LUAtoString(L, -1);
   COL_VAR(HelpFile);
   lua_pop(L, 1);  // pop the filepath string on top of the stack
   lua_pop(L, 1);  // pop the _help table
   COL_VAR(lua_gettop(L));
   COLstring FileContent;
   if(CACHEexists()) { CACHEreadFile(HelpFile, &FileContent); }
   else              { 
      if (!FILfileExists(HelpFile)) { return false; } // need this check, otherwise FILreadFile could throw and kill off the worker thread.
      FILreadFile(HelpFile, &FileContent); 
   }
   COL_VAR(FileContent);
   HELPtoObject(pHelp, FileContent);
   COL_VAR(lua_gettop(L));
   lua_settop(L, StartTop);
   COL_VAR(lua_gettop(L));
   return true;
}


static void TRNluaHandleFunctionHelp(lua_State* L, TRNintellisenseContext* pContext){
   COL_FUNCTION(TRNluaHandleFunctionHelp);
   if (!lua_isfunction(L,-1)) return;
   HELPobject FunctionHelp;
   COLstring HelpFile;
   if (TRNfetchHelpJsonFromFile(L, &FunctionHelp, &HelpFile)){
      pContext->HelpSummary = FunctionHelp.SummaryLine;
      pContext->HelpFile = HelpFile;
      if (FunctionHelp.Parameters.size()){
         //using named parameters + at least one param

         REXmatcher SearchRegex;
         if (pContext->ParseContext.SearchString.size()){
            int SearchIndex = -1;
            TRNintellisenseSearchCriteria(pContext->ParseContext.SearchString, SearchRegex, SearchIndex);
         }

         COLuint32 ParamNameMaxLength = 0;
         {
            for (COLuint32 ParamIndex = 0; ParamIndex != FunctionHelp.Parameters.size(); ParamIndex++){
               COLuint32 ThisSize = FunctionHelp.Parameters[ParamIndex].Name.size();
               if (ThisSize > ParamNameMaxLength) ParamNameMaxLength = ThisSize;
            }
         }
         for (COLuint32 ParamIndex = 0; ParamIndex != FunctionHelp.Parameters.size(); ParamIndex++){
            const HELPparameter& Param = FunctionHelp.Parameters[ParamIndex];
            if (!pContext->ParseContext.SearchString.size() ||
               SearchRegex.doesMatch(Param.Name.c_str())){
                  TRNintellisenseOption Option(TRN_LUA_HELP_PARAM_PROTOCOL, (Param.Required ? TRN_LUA_HELP_PARAM_TYPE_REQUIRED : TRN_LUA_HELP_PARAM_TYPE_OPTIONAL) ,0, TRNintellisenseOption::MAX);
                  Option.FullText = pContext->ParseContext.FullExpression.substr(0, pContext->ParseContext.FullExpression.size() - pContext->ParseContext.SearchString.size()) + Param.Name + "=";
                  Option.Index = ParamIndex;
                  Option.DisplayText = Param.Name;
                  if (pContext->ParseContext.SearchString.size()){
                     SearchRegex.regexReplace(Option.DisplayText, TRN_HIGHLIGHT_SEARCH_MATCH, TRNhtmlEscapeString);
                  }
                  COLuint32 SpaceCount = ParamNameMaxLength-Param.Name.size();
                  while(SpaceCount--){
                     //pad out with some spaces
                     Option.DisplayText += COL_T("&nbsp;");
                  }
                  TRNaddShortHelpText(Option,Param);
                  Option.FunctionName = Param.Name;
                  pContext->Options.add(Option);
            }
         }
      }

      //if we are searching, and no parameters were found
      //don't output help text
      if (pContext->ParseContext.SearchString.size() && pContext->Options.size() == 0){
         pContext->HelpSummary.clear();
      }
   }
}

// Get a copy of the table represented by __node from the private Lua
// registry, with functions filtered out based on pPlace's node/protocol type.
static void TRNgetFilteredNodeFunctions(lua_State* L, const NODplace* pPlace){
   lua_newtable(L); // copy
   int CopyIndex = lua_gettop(L);
   lua_getfield(L, LUA_REGISTRYINDEX, "__node");

   lua_pushnil(L); // first key
   while (lua_next(L, -2)){
      // Key is at -2, value is at -1
      if (LNDincludeFunction(pPlace, LUAtoString(L, -2))){
         lua_setfield(L, CopyIndex, lua_tostring(L, -2)); // puts value in copy, and pops it
      } else {
         lua_pop(L, 1); // pops value, leaves key on stack
      }
   }

   lua_pop(L, 1); // pop original
}

static void TRNgenerateDisplayFullTextOption(COLstring& FullText, TRNintellisenseOption& Option){
   COL_FUNCTION(TRNgenerateDisplayFullTextOption);
   COLstring Result;
   COLostream Stream(Result);
   Option.DisplayText = "Text";
   Option.DisplayText.write(TRN_BEGIN_TREE_VALUE.c_str(), TRN_BEGIN_TREE_VALUE.size());
   Option.DisplayText += "<span class=\'clrz_text\'>";
   COL_VAR(Option.DisplayText);
   //Escapes the text so all the characters show up in the intellisense.
   THTMprintEscaped(Stream, FullText.c_str(), FullText.size());
   COL_VAR2(FullText, Result);
   //Crops if it's too long to display.
   SFIhtmlCrop(Result.c_str(), Result.size(), TRN_INTELLISENSE_TRUNCATED, TRN_INTELLISENSE_MAX_LENGTH, Option.DisplayText);
   COL_VAR(Option.DisplayText);
   Option.DisplayText.write(TRN_END_HIGHLIGHT_SEARCH_MATCH, 7);
   Option.DisplayText.write(TRN_END_TREE_VALUE.c_str(), TRN_END_TREE_VALUE.size());
   COL_VAR(Option.DisplayText);
   //This assures we get the Text icon beside the option in front end.
   Option.NodeType = NOX_TEXT;
}

static void TRNaddChildOptions(TRNtraverser& Traverser, const COLstring& RootVar, TRNsortedOptionsList& Options, lua_State* L){
   COL_FUNCTION(TRNaddChildOptions);

   COLstring FullText;
   bool HasText(false);
   while (Traverser.stepNext()) {
      TRNintellisenseOption Option(Traverser, 1);
      COL_VAR3(Option.NodeType, Option.ProtocolType, Option.IsSimple);
      COL_VAR(Traverser.optionDisplayText());
      // Collecting all text nodes of the node, in the case of XML
      if (Option.ProtocolType == NOX_XML && Option.NodeType == NOX_TEXT && Traverser.traverserType() == TRN_NODE_TRAVERSER_TYPE){
         COL_TRC("Is XML. Will merge text nodes.");
         COL_VAR(Traverser.childValue());
         FullText += Traverser.childValue();
         HasText = true;
      } else {
         // Other protocols should keep text nodes distinct.
         COL_TRC("Will keep text nodes separate.");
         Option.DisplayText = Traverser.optionDisplayText(&Option.FunctionName);
         if (Option.ProtocolType == NOX_XML) {
            // In XML we need to check for node names that collide with our methods
            Option.FullText = Traverser.optionFullText(RootVar, L);
         } else {
            Option.FullText = Traverser.optionFullText(RootVar);
         }
         if (!Traverser.isLowPriority()) Option.Priority = TRNintellisenseOption::HIGH;
         Options.add(Option);
      }
   }
   COL_VAR(HasText);
   if (!HasText) return; //There is no text node for this node (i.e. node methods)

   TRNintellisenseOption TextOption(Traverser, 1);
   TextOption.FullText = RootVar + ":nodeText()";
   TRNgenerateDisplayFullTextOption(FullText, TextOption);
   Options.add(TextOption);
}

static void TRNindexedIntellisenseHandleNode(lua_State* L, TRNintellisenseContext* pContext, const NODplace* pNodeData, COLauto<TRNtraverser>* pTraverser){
   COL_FUNCTION(TRNindexedIntellisenseHandleNode);
   COLPRECONDITION(pNodeData);
   if (pContext->ParseContext.IndexChar == ':'){
      lua_pop(L, 1); // pop node
      COL_VAR(pContext->Options.size());
      TRNgetFilteredNodeFunctions(L, pNodeData); // replace node with filtered node namespace
      COL_VAR(pContext->Options.size());
      *pTraverser = TRNcreateTraverser(L, lua_gettop(L), false, pContext->ParseContext.IndexChar);
   } else {
      *pTraverser = TRNcreateTraverser(*pNodeData);
      if (pContext->ParseContext.SearchString.is_null()){
         COL_VAR(pContext->Options.size());
         TRNaddSpecificNodeTypeOptions(*pNodeData, pContext->ParseContext.TrimmedExpression, pContext->Options);
         COL_VAR(pContext->Options.size());
      }
   }
}

static void TRNindexedIntellisense(lua_State* L, TRNintellisenseContext* pContext){
   COL_FUNCTION(TRNindexedIntellisense);
   lua_getfenv(L, -1);
   lua_getfield(L, -1, TRN_GLOBAL_INTELLISENSE_VARIABLE);
   lua_replace(L, -2); // replace env with TRN_GLOBAL_INTELLISENSE_VARIABLE

   COL_TRC("Got the intellisense object.");
   try{
      COLauto<TRNtraverser> pTraverser;
      COL_VAR(pContext->ParseContext.IndexChar);
      if (pContext->ParseContext.IndexChar == '{' || pContext->ParseContext.IndexChar == '('){
         TRNluaHandleFunctionHelp(L,pContext);
      } else {
         // The node represented by pContext->TrimmedExpression should be on top of the stack.
         if (LNDisNode(L, -1)){
            COL_TRC("Found a node");
            const NODplace* pNodeData = LNDgetNode(L, -1);  // safe cast - uses meta table
            TRNindexedIntellisenseHandleNode(L, pContext, pNodeData, &pTraverser); // make sure to grab the pointer to the COLauto as the underlying object pointer is assigned in this helper function
            COLPRECONDITION(pTraverser.get());
         } else if (lua_type(L, -1) == LUA_TSTRING && pContext->ParseContext.IndexChar == ':'){
            COL_TRC("Found a string method call");
            lua_pop(L, 1); // pop string
            lua_getglobal(L, LUA_STRLIBNAME); // replace with string namespace
            pTraverser = TRNcreateTraverser(L, lua_gettop(L), false, pContext->ParseContext.IndexChar); //maybe?
            COLPRECONDITION(pTraverser.get());
         } else if (lua_istable(L, -1) || lua_isuserdata(L, -1)){
            COL_TRC("We have a table or user data...");
            pTraverser = TRNcreateTraverser(L, lua_gettop(L), false, pContext->ParseContext.IndexChar);
            COLPRECONDITION(pTraverser.get());
         }
      }

      if (pTraverser.get()){
         if (pContext->ParseContext.SearchString.is_null()){
            TRNaddChildOptions(*pTraverser, pContext->ParseContext.TrimmedExpression, pContext->Options, L);
            TRNautoCompleteChildOptions(L, &pContext->NewCompleteInfo);
         }else{
            COL_VAR(pContext->ParseContext.SearchString);
            TRNaddSearchMatchNodes(*pTraverser, pContext->ParseContext.TrimmedExpression, pContext->ParseContext.SearchString, pContext->Options, L);
         }
      }
   }
   catch (const COLerror& Error){
      COL_TRC(Error.description());
      luaL_error(L, "%s", Error.description().c_str());
   }
   lua_pop(L,1);
}

void TRNintellisenseHook(lua_State* L, lua_Debug* pD){
   COL_FUNCTION(TRNintellisenseHook);
   int TopIn = lua_gettop(L);
   COL_VAR(TopIn);
   // Get the intellisense context from the Lua registry.
   TRNintellisenseContext* pContext = TRNintellisenseContextGet(L);
   lua_pop(L, 1);
   lua_getinfo(L, "Sf", pD); // pushes the function we're debugging onto the stack
   COL_VAR3(pContext->CurrentFile, pD->source, pD->currentline);
   COLstring pDFile(pD->source); // For windows, pContext->CurrentFile has windows separators, but pD->source will have posix style separators.
   FILcorrectPathSeparators(pDFile); 
   if (pD->currentline == pContext->LineNumber + 1 && pContext->CurrentFile == pDFile){
      COL_TRC("We hit our line and file!");
      pContext->TargetReached = true;
      COL_VAR(lua_gettop(L));

      if (TRNshouldDoVariableAutocompletion(pContext->ParseContext)){
         // Provide auto-completion for variable names (i.e. nothing is being indexed).
         // We should find all the valid variables in this context.
         pContext->ParseContext.FullExpression = pContext->ParseContext.SearchString; // everything before and including the index character now doesn't matter. Fixes #19643
         COL_TRC("Encountered a variable");
         TRNvariableIntellisense(L, pContext, pD);
      } else {
         // User is indexing an object.  We should find all valid children.
         COL_TRC("Encountered an object");
         TRNindexedIntellisense(L, pContext);
      }

      lua_sethook(L, &TRNintellisenseHook, 0 ,0); // disable hook
      COL_TRC("Disabled the debug hook.");
      luaL_error(L, "%s", "Auto-completion target reached."); // bail out
   }
   lua_pop(L, 1); // pop function we're debugging
   LUAcheckTimeout(L);
   COLPRECONDITION(TopIn == lua_gettop(L));
}

void TRNintellisenseHookSet(lua_State* L){
   COL_FUNCTION(TRNintellisenseHookSet);
   // It's important we set the debug hook after we have compiled the code! #16165
   lua_sethook(L, &TRNintellisenseHook, LUA_MASKLINE, 0);
   COL_TRC("debug hook setup complete");
}
