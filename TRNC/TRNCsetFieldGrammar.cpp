//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: TRNCsetFieldGrammar
//
// Author: John Q
//
// Implementation
//---------------------------------------------------------------------------
#include <CFG/CFGfieldGrammar.h>
#include <CFG/CFGmap.h>
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
#include <TRNC/TRNCsetFieldGrammar.h>
COL_LOG_MODULE;

// in case the error is null
static COLstring TRNCprintFieldGrammarErr(const COLstring& Error) {
   COL_FUNCTION(TRNCprintFieldGrammarErr);
   return Error.is_null() ? "Unknown error saving field grammar" : Error;
}

// /component/field/grammar/set
void TRNCsetFieldGrammar(const COLwebRequest& Request) {
   COL_FUNCTION(TRNCsetFieldGrammar);
   COL_VAR(Request.Data);
   COLstring ComponentId, Error;
   const COLstring Name          = COLvarRequiredString(Request.Data, "name");
   const COLvar    GrammarWebVar = COLvarRequiredObject(Request.Data, "fields");
   const bool      IsDev         = COLvarRequiredBool(Request.Data, "development");
   if (!CFGcomponentId(Request.Data, &ComponentId, &Error)){
      return COLrespondError(Request.Address, Error);
   }
   COL_VAR3(ComponentId, GrammarWebVar, IsDev);
   if(!PRMcheckRoleAccess(Request.FromTranslator, Request.User, ComponentId, ROLid::EDIT_FIELDS, &Error)) {
      return COLrespondError(Request.Address, PRMsetError(ROLid::EDIT_FIELDS));
   }
   COLarray<CFGfield> Grammar;
   if(!CFGfieldCreateGrammar(GrammarWebVar, &Grammar, &Error)) {
      return COLrespondError(Request.Address, TRNCprintFieldGrammarErr(Error));
   }
   if(!CFGfieldSaveGrammar(ComponentId, Grammar, &Error, IsDev)) {
      return COLrespondError(Request.Address, TRNCprintFieldGrammarErr(Error));
   }
   COLrespondSuccess(Request.Address);
}
