// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DEPdeprecated
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Tuesday 08 April 2025 - 07:01AM
// ---------------------------------------------------------------------------

#include <DEP/DEPlist.h>
#include <HELP/HELPutils.h>
#include <COL/COLweb.h>
#include <COL/COLlog.h>
#include <COL/COLsplit.h>
#include <FIL/FILatomic.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <CACHE/CACHEfile.h>
COL_LOG_MODULE;

COLstring DEPfileName(){
   COL_FUNCTION(DEPfileName);
   COLstring FileName;
   FileName = FILpathAppend(FILexecutableDir(), "..");
   FileName = FILpathAppend(FileName, COL_T("help") + FIL_DIR_SEPARATOR);
   FileName = FileName + COL_T("deprecated.txt");
   return FileName;
}

void DEPreadLua(COLarray<COLstring> *pLines){
   COL_FUNCTION(DEPreadLua);
   COLstring Content;
   if(CACHEexists()) {
      CACHEgetDeprecated(&Content);
   } else {
      const COLstring FileName = DEPfileName();
      COL_VAR2(FileName, FILfileExists(FileName));
      if(!FILfileExists(FileName)) {
         COL_TRC("Deprecated file not found, no deprecated Lua api's to worry about.");
         return;
      }
      FILreadFile(FileName, &Content);
   }
   COLarray<COLstring> &Lines = *pLines;
   COLsplit(&Lines, Content, "\n");
   for(int i=0; i < Lines.size(); i++) {
      Lines[i] = COLstripWhiteSpace(Lines[i]);
   }
}

void DEPlistOutput(const COLarray<COLstring> &Lines, COLvar *pOut){
   COL_FUNCTION(DEPlistOutput);
   for(int i=0; i < Lines.size(); i++){
      COLstring Line = Lines[i];
      if(Line.is_null()) { continue; }
      (*pOut)[Line] = "";
   }
}

void DEPlua(const COLwebRequest& Request) {
   COL_FUNCTION(DEPlua);
   COLarray<COLstring> Lines;
   COLvar Out;
   DEPreadLua(&Lines);
   DEPlistOutput(Lines, &Out);
   return COLrespondSuccess(Request.Address, Out);
}  