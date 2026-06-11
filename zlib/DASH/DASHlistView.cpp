//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DASHlistView
//
// Label:
//
// Implementation.
//
// Author: Tyler Brown
// Date:   Oct 7 2021
//---------------------------------------------------------------------------
#include <CFG/CFGmap.h>
#include <CFG/CFGmapApi.h>
#include <COL/COLlog.h>
#include <COL/COLsplit.h>
#include <COL/COLstring.h>
#include <COL/COLvarUtils.h>
#include <DASH/DASHlistView.h>
#include <HASH/HASHutils.h>
#include <MATCH/MATCHsearch.h>
COL_LOG_MODULE;

COLarray<COLstring> DASHvarToArray(const COLvar& In) {
   COL_FUNCTION(DASHvarToArray);
   COLarray<COLstring> Array;
   if(!In.isArray()) { return Array; }
   for(const auto& it : In.array()) { Array.push_back(it.asString()); }
   return Array;
}

void DASHglobMatch(const COLstring& GlobTerms, CFGmap* pMap, const bool ExactMatch, COLarray<COLstring>* pComponents) {
   COL_FUNCTION(DASHglobMatchComponents);
   COL_VAR(GlobTerms);
   pComponents->clear();
   for(const auto& it : pMap->ConfigMap) {
      const COLstring SearchString = it.second.CoreConfig.Name + " " + HASHarrToString(it.second.CoreConfig.Tags);
      COL_VAR(SearchString);
      if(MATCHsearch(GlobTerms, SearchString, ExactMatch)) { pComponents->push_back(it.second.CoreConfig.Guid); }
   }
   COL_TRC("Found " << pComponents->size() << " components");
}

void DASHcombineLists(const COLarray<COLstring>& Selected, CFGmap* pMap, COLarray<COLstring>* pComponents) {
   COL_FUNCTION(DASHcombineLists);
   COLvar InputMap;
   InputMap.setMapType();
   for(const auto& it : Selected) { pComponents->push_back(it); }
   int length = pComponents->size() - 1;
   while(length >= 0) {
      const COLstring& Guid = (*pComponents)[length];
      if(InputMap.exists(Guid) || !CFGmapNode(pMap, Guid) || Guid.is_null() || Guid.isWhitespace()) {
         COL_TRC("Removing guid " << Guid);
         pComponents->remove(length);
      } else {
         InputMap[Guid] = 1;
      }
      length--;
   }
}
