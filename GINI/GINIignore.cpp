//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GINIignore.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   22/05/25 10:37 AM
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLsplit.h>
#include <DIR/DIRpath.h>
#include <FIL/FILutils.h>
#include <GINI/GINIignore.h>
#include <GIT/GITadd.h>
#include <GIT/GITcommit.h>
COL_LOG_MODULE;

const char* GINI_KEY_SECTION     = "# Private Key Files";
const char* GINI_DEFAULT_SECTION = "# iNTERFACEWARE Defaults";

bool GINIsaveIgnore(const COLstring& Path, const COLarray<COLstring>& Lines, COLstring* pErr) {
   COL_FUNCTION(GINIsaveIgnore);
   COLstring Content;
   COLjoin(&Content, Lines, "\n");
   COL_VAR(Content);
   return FILsave(Path, Content, pErr);
}

bool GINIfindSection(const COLarray<COLstring>& IgnoreLines, const COLstring& Section) {
   COL_FUNCTION(GINIfindSection);
   const COLstring SectionLower = COLlowerCase(Section);
   for(const auto& it : IgnoreLines) {
      if(COLlowerCase(it) == SectionLower) return true;
   }
   return false;
}

bool GINIaddAndCommitIgnore(const COLstring& User, COLstring* pErr) {
   COL_FUNCTION(GINIaddAndCommitIgnore);
   const COLstring ConfigDir = DIRconfigDir();
   if(GITadd(ConfigDir, ".gitignore", true, pErr) && GITcommit(User, ConfigDir, "", "Updated .gitignore", pErr)) {
      return true;
   }
   COL_TRC(*pErr);
   return false;
}

void GINIremovePatterns(COLarray<COLstring>* pIgnoreLines, const COLarray<COLstring>& PatternsToRemove) {
   COL_FUNCTION(GINIremovePatterns);
   COLarray<COLstring> NewLines;
   for(const auto& line : *pIgnoreLines) {
      bool remove = false;
      for(const auto& pattern : PatternsToRemove) {
         if(line == pattern) {
            remove = true;
            break;
         }
      }
      if(!remove) { NewLines.push_back(line); }
   }
   *pIgnoreLines = NewLines;
}

COLarray<COLstring> GINIreadIgnoreLines(const COLstring& GitIgnoreFile) {
   COL_FUNCTION(GINIreadIgnoreLines);
   COLarray<COLstring> Lines;
   if(FILfileExists(GitIgnoreFile)) {
      try {
         COLstring Content;
         FILreadFile(GitIgnoreFile, &Content);
         COLsplit(&Lines, Content, "\n");
      } catch(const COLerror& E) { COL_TRC(E.description()); }
   }
   return Lines;
}

void GINIprocessIgnore(COLarray<COLstring>* pGitIgnoreLines, const COLstring& Section, const bool AddSection,
                       const COLarray<COLstring>& Patterns) {
   COL_FUNCTION(GINIprocessIgnore);
   const COLstring SectionLower = COLlowerCase(Section);
   bool SectionFound = false;
   bool InSection    = false;
   COLarray<COLstring> NewLines;

   for(const auto& line : *pGitIgnoreLines) {
      if(COLlowerCase(line) == SectionLower) {
         SectionFound = true;
         InSection    = true;
         if(AddSection) NewLines.push_back(line);
         continue;
      }

      if(InSection) {
         bool isPattern = false;
         for(const auto& pattern : Patterns) {
            if(line == pattern) {
               isPattern = true;
               break;
            }
         }

         // End of section
         if(line.is_null() || (!isPattern && !line.is_null())) { InSection = false; }

         // If we're adding section, skip patterns. otherwise, keep them
         if(!isPattern || AddSection) { NewLines.push_back(line); }
      }
      else {
         NewLines.push_back(line);
      }
   }

   // If we're adding section and didn't find it, add it
   if(AddSection && !SectionFound) {
      if(NewLines.size() > 0 && !NewLines.back().is_null()) {
         NewLines.push_back("");  // Add empty line before section
      }
      NewLines.push_back(Section);
      for(const auto& pattern : Patterns) { NewLines.push_back(pattern); }
   }
   *pGitIgnoreLines = NewLines;
}