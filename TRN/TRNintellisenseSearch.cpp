//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNintellisenseSearch
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Tuesday, December 21st, 2010 @ 06:47:21 AM
//---------------------------------------------------------------------------
#include <TRN/TRNintellisenseSearch.h>
#include <TRN/TRNtraverser.h>
#include <TRN/TRNconstant.h>
#include <TRN/TRNintellisenseSearchCriteria.h>

#include <REX/REXmatcher.h>

#include <COL/COLhashmap.h>
#include <COL/COLauto.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static void TRNaddSearchMatchNodesImpl(TRNtraverser& Traverser, const COLstring& RootVar, const COLstring& RootText, const REXmatcher& SearchRegex, int SearchIndex, int Level, TRNsortedOptionsList& Options, lua_State* L);

static void TRNaddSearchMatchNodesIterationImpl
(
   TRNtraverser& Traverser,
   const COLstring& RootVar,
   const COLstring& RootDisplayText,
   const REXmatcher& SearchRegex,
   int SearchIndex, // -1 means N/A
   int Level,
   TRNsortedOptionsList& Options,
   lua_State* L
){
	COL_FUNCTION(TRNaddSearchMatchNodesIterationImpl);
	COL_VAR3(Level, SearchRegex, SearchIndex); 
   TRNtraverser::eSearchResult SearchResult = Traverser.findMatch(SearchRegex, SearchIndex);
   COL_VAR(TRNsearchResult(SearchResult));
   if (SearchResult != TRNtraverser::NO_MATCH || !Traverser.isLeaf()){
      // Prepare string data that will be used both if the match is found, and when recursively
      // searching through the child place.
      COLstring DisplayText = Traverser.deepOptionDisplayText(SearchRegex, RootDisplayText, false);
      COLstring FullText    = Traverser.optionFullText(RootVar, L);
      if (SearchResult != TRNtraverser::NO_MATCH) {
         TRNintellisenseOption Option(Traverser, Level, TRNintellisenseOption::HIGH);
         if (SearchResult == TRNtraverser::INDEX_MATCH){
            Option.DisplayText = Traverser.deepOptionDisplayText(SearchRegex, RootDisplayText, true);
            Option.Priority = TRNintellisenseOption::MAX;
         } else{
            Option.DisplayText = DisplayText;
            if (Traverser.isLowPriority()) Option.Priority = TRNintellisenseOption::NORMAL;
         }
         Traverser.writeTreeValue(Option.DisplayText, &Option.FunctionName, &SearchRegex);
         Option.FullText = FullText;
         Options.add(Option);
      }
      if (!Traverser.isLeaf()){
         COLauto<TRNtraverser> pNewTraverser = Traverser.traverser();
         TRNaddSearchMatchNodesImpl(*pNewTraverser, FullText, DisplayText, SearchRegex, -1, Level+1, Options, L);
      }
   }
}

static void TRNaddSearchMatchNodesImpl(TRNtraverser& Traverser, const COLstring& RootVar, const COLstring& RootText, const REXmatcher& SearchRegex, int SearchIndex, int Level, TRNsortedOptionsList& Options, lua_State* L) {
   COL_FUNCTION(TRNaddSearchMatchNodesImpl);
   COL_VAR4(SearchRegex, RootVar, RootText, Level);
	if(Level == TRN_MAX_RECURSION_LEVEL){  
      return;  
	}
	while (Traverser.stepNext()) {
      TRNaddSearchMatchNodesIterationImpl(Traverser, RootVar, RootText, SearchRegex, SearchIndex, Level, Options, L);
   }
}

void TRNaddSearchMatchNodes(TRNtraverser& Traverser, const COLstring& RootVar, const COLstring& SearchString, TRNsortedOptionsList& Options, lua_State* L){
   COL_FUNCTION(TRNaddSearchMatchNodes);
	REXmatcher SearchRegex;
   int SearchIndex = -1;
   TRNintellisenseSearchCriteria(SearchString, SearchRegex, SearchIndex);
   TRNaddSearchMatchNodesImpl(Traverser, RootVar, "", SearchRegex, SearchIndex, 0, Options, L);
}
