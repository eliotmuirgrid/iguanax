// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: WEBIindexFile
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Tuesday 11 July 2023 - 02:31PM
// ---------------------------------------------------------------------------
#include <WEBI/WEBIindexFile.h>

#include <FIL/FILpathUtils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

bool WEBIcssAnchorExists(COLstring* pData, int* pInsertPos) {
   COL_FUNCTION(WEBIcssAnchorExists);
   return pData->find(pInsertPos, "<!-- CSS anchor (for css auto include) -->");
}

bool WEBIjsAnchorExists(COLstring* pData, int* pInsertPos) {
   COL_FUNCTION(WEBIjsAnchorExists);
   return pData->find(pInsertPos, "<!-- JS anchor (for js auto include) -->");
}

COLstring WEBIcssInclude(const COLstring& CssFile) {
   COL_FUNCTION(WEBIcssInclude);
   return "<link rel=\"stylesheet\" type=\"text/css\" href=\"" + CssFile + "\">\n";
}

COLstring WEBIjsInclude(const COLstring& JsFile) {
   COL_FUNCTION(WEBIjsInclude);
   return "<script type=\"text/javascript\" src=\"" + JsFile + "\"></script>\n";
}

void WEBIinclude(const COLstring& Include, COLstring* pData, int* pInsertPos) {
   COL_FUNCTION(WEBIinclude);
   pData->insert(*pInsertPos, Include);
   *pInsertPos += Include.size();
}

bool WEBIisExternal(const COLstring& JsFile) {
   COL_FUNCTION(WEBIisExternal);
   return JsFile.find("jquery") != npos || JsFile.find("CodeMirror") != npos;
}

// this will modify the string to index.js or index.css if it is a non-external file (see WEBIisExternal)
// returns false if no modification was made
bool WEBIresolveFile(COLstring* pEntry) {
   COL_FUNCTION(WEBIresolveFile);
   COLstring Extension = FILpathExtLast(*pEntry, false);
   COL_VAR2(*pEntry, Extension);
   if(Extension == "js" && !WEBIisExternal(*pEntry)) { *pEntry = "/web/app.js";      return true; }
   if(Extension == "css")                            { *pEntry = "/web/app/app.css"; return true; }
   return false;
}