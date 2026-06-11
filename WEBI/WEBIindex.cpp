//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: WEBindex.cpp
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Sunday 21 May 2023 - 06:02PM
// ---------------------------------------------------------------------------

#include <WEBI/WEBIindex.h>
#include <WEBI/WEBIindexFile.h>

#include <FIL/FILdirEnumerator.h>

#include <FIL/FILutils.h>
#include <FIL/FILpathUtils.h>

#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static void WEBenuDir(const COLstring& CurrDir,COLvar* pAllCss, COLvar* pAllJs){
   FILdirEnumerator DirEnumerator(CurrDir + "/*", false, true);
   COLstring        CurrentDirName;
   bool             GotNewDir = DirEnumerator.getFile(CurrentDirName);
   while (GotNewDir) {
      COLstring CurrFullPath = FILpathAppend(CurrDir, CurrentDirName);
      COLstring Extension    = FILpathExtLast(CurrentDirName, false);
      if(Extension == "css")                { pAllCss->push_back(CurrFullPath); }
      else if(Extension == "js")            { pAllJs->push_back(CurrFullPath); }
      else if(FILisDirectory(CurrFullPath)) { WEBenuDir(CurrFullPath, pAllCss, pAllJs); }
      GotNewDir = DirEnumerator.getFile(CurrentDirName);
   }
}

void WEBIindexGetCssAndJs(const COLstring& WebDir, COLstring* pData){
   COL_FUNCTION(WEBIindexGetCssAndJs);
   COLvar AllCss, AllJs;
   AllCss.setArrayType();
   AllJs.setArrayType();
   WEBenuDir(WebDir, &AllCss, &AllJs);
   int InsertPos;
   COL_TRC("Adding CSS files");
   if (!WEBIcssAnchorExists(pData, &InsertPos)) { COL_ERR("Could not find CSS anchor"); return; }
   InsertPos += 43;  // because the css anchor string is 43 characters long
   for (int i = 0; i < AllCss.size(); i ++){
      COLstring CssFile = AllCss[i];
      CssFile.replace(WebDir, "");
      WEBIinclude(WEBIcssInclude(CssFile), pData, &InsertPos);
   }
   COL_TRC("Adding JS files");
   if (!WEBIjsAnchorExists(pData, &InsertPos)) { COL_ERR("Could not find JS anchor"); return; }
   InsertPos += 41;  // because the js anchor string is 41 characters long
   for (int i = 0; i < AllJs.size(); i ++){
      COLstring JsFile = AllJs[i];
      int FoundJs;
      JsFile.replace(WebDir, "");
#ifdef _WIN32
      JsFile.replace(FIL_DIR_SEPARATOR, "/");
#endif
      if(WEBIisExternal(JsFile) || pData->find(&FoundJs, JsFile)) { continue; } // file was manually included already
      WEBIinclude(WEBIjsInclude(JsFile), pData, &InsertPos);
   }
   COL_VAR(*pData);
}

void WEBIindexPack(const COLstring& JsFile, const COLstring& CssFile, COLstring* pData) {
   COL_FUNCTION(WEBIindexPack);
   int InsertPos;
   if(WEBIcssAnchorExists(pData, &InsertPos)) {
      COL_TRC("Found CSS anchor");
      InsertPos += 43;
      WEBIinclude(WEBIcssInclude(CssFile), pData, &InsertPos);
   }

   if(WEBIjsAnchorExists(pData, &InsertPos)) {
      COL_TRC("Found JS anchor");
      InsertPos += 41;
      WEBIinclude(WEBIjsInclude(JsFile), pData, &InsertPos);
   }
   COL_VAR(*pData);
}