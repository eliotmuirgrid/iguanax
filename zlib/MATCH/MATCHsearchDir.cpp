// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
// Module: MATCHsearchDir
// ---------------------------------------------------------------------------

#include <MATCH/MATCHsearchDir.h>

#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <FIL/FILdirEnumerator.h>
#include <MATCH/MATCHsearch.h>

#include <COL/COLlog.h>
#include <COL/COLstack.h>
#include <COL/COLvar.h>
COL_LOG_MODULE;

const int MATCH_MAX_FILE_SIZE = 100 * 1024 * 1024;

bool MATCHdirExists(const COLstring& Path) {
   COL_FUNCTION(MATCHdirExists);
   if (!FILfileExists(Path)) { COL_TRC(Path << " not found"); return false; }
   return true;
}

void MATCHinitResults(COLvar* pResults) {
   COL_FUNCTION(MATCHinitResults);
   if (!pResults) return;
   *pResults = COLvar();
   pResults->setArrayType();
}

bool MATCHprocessFile(const COLstring& Query, const COLstring& FileName, const COLstring& FullPath, bool ExactMatch, COLvar* pResults, bool* AnyFound){
   COL_FUNCTION(MATCHprocessFile);
   if (FILfileSize(FullPath) >= MATCH_MAX_FILE_SIZE) {
      COL_TRC(FullPath << " greater than 100mb");
      if (pResults) (*pResults)["too_large"].push_back(FileName);
      return false;
   }
   COLstring Data;
   FILreadFile(FullPath, &Data);
   Data.replace("\r\n", "\n");
   if (MATCHsearch(Query, Data, ExactMatch)) {
      COL_TRC("Match found: " << FullPath);
      if (pResults) (*pResults)["matches"].push_back(FullPath);
      if (AnyFound) *AnyFound = true;
      return true;
   }
   return false;
}

bool MATCHtraverse(const COLstring& Query, const COLstring& RootDir, bool ExactMatch, COLvar* pResults){
   COL_FUNCTION(MATCHtraverse);
   COLstack<COLstring> Stack;
   Stack.push(RootDir);
   bool AnyFound = false;
   while (!Stack.empty()) {
      COLstring Dir = Stack.top(); Stack.pop();
      FILdirEnumerator e(FILpathAppend(Dir, FIL_ANYFILE));
      COLstring Name;
      while (e.getFile(Name)) {
         if (e.isHidden()) { COL_TRC("Hidden entry: " << Name << " — skipping"); continue; }
         const COLstring Full = FILpathAppend(Dir, Name);
         COL_VAR(Full);
         if (e.isDirectory()) {
            if (Name == ".git") { COL_TRC("Skipping Git folder: " << Full); continue; }
            Stack.push(Full);
         } else {
            MATCHprocessFile(Query, Name, Full, ExactMatch, pResults, &AnyFound);
         }
      }
   }
   return AnyFound;
}

bool MATCHsearchDir(const COLstring& Query, const COLstring& Dir, bool ExactMatch, COLvar* pResults){
   COL_FUNCTION(MATCHsearchDir);
   if (!MATCHdirExists(Dir)) return false;
   MATCHinitResults(pResults);
   return MATCHtraverse(Query, Dir, ExactMatch, pResults);
}
