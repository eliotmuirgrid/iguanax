//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SRCHreplaceAll.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   30/01/25 4:43 PM
//  ---------------------------------------------------------------------------
#include <CFG/CFGconfigApi.h>
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <FIL/FILutils.h>
#include <PRM/PRMcallbacks.h>
#include <SRCH/SRCHcache.h>
#include <SRCH/SRCHreplaceAll.h>
#include <SRCH/SRCHsearch.h>
#include <SRCH/SRCHutils.h>
COL_LOG_MODULE;

static void SRCHreplaceAllWrite(const COLmap<COLstring, COLstring>& FileContents) {
   COL_FUNCTION(SRCHreplaceAllWrite);
      for(const auto& it : FileContents) {
      try                      { FILwriteFile(it.first, it.second); }
      catch(const COLerror& E) { COL_TRC(E.description()); }
   }
}

static size_t SRCHcomputeLineStart(const COLstring& content, int lineNumber) {
   COL_FUNCTION(SRCHcomputeLineStart);
   size_t lineStart = 0;
   for(int currentLine = 1; currentLine < lineNumber; ++currentLine) { lineStart = content.find('\n', lineStart) + 1; }
   return lineStart;
}

void SRCHreplaceAllImpl(const COLvar& Matches, const COLstring& Query, const COLstring& Replacement,
                        COLmap<COLstring, COLstring>* pFileContents) {
   COL_FUNCTION(SRCHreplaceAllImpl);
   if(!Matches.isArray()) { return; }

   // First pass: read all files that need modification
   for(const auto& Match : Matches.array()) {
      const COLstring& FilePath = Match["file"];
      if(pFileContents->count(FilePath) == 0) { SRCHreadIntoCache(pFileContents, FilePath); }
   }

   // Second pass: perform replacements from end to start
   for(int i = Matches.size() - 1; i >= 0; --i) {
      const auto&      Match      = Matches[i];
      const COLstring& FilePath   = Match["file"];
      const int        LineNumber = Match["line_number"];
      const int        Position   = Match["position"];
      auto&            Content    = (*pFileContents)[FilePath];
      const size_t     LineStart  = SRCHcomputeLineStart(Content, LineNumber);
      const size_t     ReplacePos = LineStart + (Position - 1);
      Content.replace(ReplacePos, Query.size(), Replacement);
   }
}

void SRCHreplaceAllInit(const COLstring& Path, const COLstring& Query, const COLstring& Replacement,
                        const bool MatchCase, const bool MatchFull, const COLvar& Selections) {
   COL_FUNCTION(SRCHreplaceAllInit);
   COL_VAR3(Path, Query, Replacement);
   COLvar Result;
   SRCHsearch(Path, Query, MatchCase, MatchFull, &Result);
   COLmap<COLstring, COLstring> UpdatedFileMap;
   COLvar& Matches = Result["matches"];
   SRCHfilterMatches(Selections, &Matches);
   COL_VAR2(Selections, Matches);
   SRCHreplaceAllImpl(Matches, Query, Replacement, &UpdatedFileMap);
   SRCHreplaceAllWrite(UpdatedFileMap);
}

// /ide/search/replace
void SRCHreplaceAll(const COLwebRequest& Request) {
   COL_FUNCTION(SRCHreplaceAll);
   const COLstring Component   = COLvarRequiredString(Request.Data, "component");
   const bool      Development = COLvarRequiredBool(Request.Data, "development");
   if(!Development) { return COLrespondError(Request.Address, "Cannot modify file structure of a run repository."); }
   COLstring Error;
   if(!PRMcheckRoleAccess(Request.FromTranslator, Request.User, Component, ROLid::EDIT_SCRIPT, &Error)) {
      return COLrespondError(Request.Address, PRMsetError(ROLid::EDIT_SCRIPT));
   }
   const COLstring Path        = CFGresolveRepoFile(Component, COLvarGetString(Request.Data, "path", ""), Development);
   const COLstring Replacement = COLvarRequiredString(Request.Data, "replace");
   const COLstring Query       = COLvarRequiredString(Request.Data, "query");
   const bool      MatchFull   = COLvarRequiredBool(Request.Data, "match_full_word");
   const bool      MatchCase   = COLvarRequiredBool(Request.Data, "match_case");
   const COLvar    Selections  = COLvarGetObject(Request.Data, "matches", COLvar());
   if(Path.is_null()) { return COLrespondError(Request.Address, "Attempting to access above component root"); }
   SRCHreplaceAllInit(Path, Query, Replacement, MatchCase, MatchFull, Selections);
   COLrespondSuccess(Request.Address);
}