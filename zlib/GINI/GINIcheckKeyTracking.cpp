//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GINItrackKeys.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   21/05/25 12:01 PM
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <GINI/GINIcheckKeyTracking.h>
#include <GINI/GINIignore.h>
COL_LOG_MODULE;

// /instance/track_keys
void GINIcheckKeyTracking(const COLwebRequest& Request) {
   COL_FUNCTION(GINIcheckKeyTracking);
   COLvar     Result;
   const auto GitIgnoreLines = GINIreadIgnoreLines(DIRconfigFile(".gitignore"));
   Result["ignored"]         = GINIfindSection(GitIgnoreLines, GINI_KEY_SECTION);
   COLrespondSuccess(Request.Address, Result);
}