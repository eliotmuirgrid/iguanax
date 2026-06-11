//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GINIdefault.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   22/05/25 10:58 AM
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLsplit.h>
#include <DIR/DIRpath.h>
#include <FIL/FILutils.h>
#include <GINI/GINIdefault.h>
#include <GINI/GINIignore.h>
#include <GINI/GINIsetKeyTracking.h>
COL_LOG_MODULE;

static COLarray<COLstring> GINIdefaultPatterns() {
   COLarray<COLstring> Defaults;
   Defaults.push_back("**/.DS_Store");
   Defaults.push_back("*.log");
   Defaults.push_back("*.tmp");
   Defaults.push_back("Thumbs.db");
   Defaults.push_back(".*.swp");
   Defaults.push_back("*~");
   return Defaults;
}

bool GINIdefault(COLstring* pErr, const bool ForceKeyTracking) {
   COL_FUNCTION(GINIdefault);
   const COLstring            IgnoreFile      = DIRconfigFile(".gitignore");
   const bool                 Exists          = FILfileExists(IgnoreFile);
   const COLarray<COLstring> DefaultPatterns = GINIdefaultPatterns();
   COLarray<COLstring>       IgnoreLines;
   if(Exists) {
      COL_TRC("File exists so only want to make sure defaults are set");
      IgnoreLines = GINIreadIgnoreLines(IgnoreFile);
      if(!GINIfindSection(IgnoreLines, GINI_DEFAULT_SECTION)) { GINIremovePatterns(&IgnoreLines, DefaultPatterns); }
      GINIprocessIgnore(&IgnoreLines, GINI_DEFAULT_SECTION, true, DefaultPatterns);
      const bool Success = ForceKeyTracking ? GINIsetKeyTrackingImpl("admin", true, &IgnoreLines, pErr)
                                            : GINIsaveIgnore(IgnoreFile, IgnoreLines, pErr);
      return Success ? GINIaddAndCommitIgnore("admin", pErr) : false;
   }
   COL_TRC("File did not exist so make sure both sections are added");
   GINIprocessIgnore(&IgnoreLines, GINI_DEFAULT_SECTION, true, DefaultPatterns);
   // GINIsetKeyTrackingImpl saves to disk and performs necessary operations to remove keys from git cache
   const bool Success = GINIsetKeyTrackingImpl("admin", true, &IgnoreLines, pErr);
   COL_VAR2(Success, *pErr);
   return Success;
}

void GINIcheckDefaultExists(COLarray<COLstring>* pIgnoreLines) {
   COL_FUNCTION(GINIcheckDefaultExists);
   if(!GINIfindSection(*pIgnoreLines, GINI_DEFAULT_SECTION)) {
      COL_TRC("Defaults dont exist -- recreate them");
      GINIprocessIgnore(pIgnoreLines, GINI_DEFAULT_SECTION, true, GINIdefaultPatterns());
   }
}