//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: TRNCfieldGrammar
//
// Author: Matthew Sobkowski
//
// Implementation
//---------------------------------------------------------------------------
#include <CFG/CFGfieldGrammar.h>
#include <CFG/CFGmap.h>
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
#include <TRNC/TRNCfieldGrammar.h>
COL_LOG_MODULE;

// TODO - one day one should not have to have config.json in every project for custom fields but today we do

// /component/field/grammar
void TRNCgetFieldGrammar(const COLwebRequest& Request) {
   COL_FUNCTION(TRNCgetFieldGrammar);
   COL_VAR(Request.Data);
   COLstring ComponentId, Error;
   const bool      IsDev = COLvarRequiredBool(Request.Data, "development");
   if (!CFGcomponentId(Request.Data, &ComponentId, &Error)){
      return COLrespondError(Request.Address, Error);
   }
   COL_VAR2(ComponentId, IsDev);
   if(!PRMcheckRoleAccess(Request.FromTranslator, Request.User, ComponentId, ROLid::EDIT_FIELDS, &Error)) {
      return COLrespondError(Request.Address, PRMsetError(ROLid::EDIT_FIELDS));
   }
   COLarray<CFGfield> Grammar;
   const COLstring    GrammarPath = DIRcomponentProjectDir(ComponentId, IsDev) + "config.json";
   if(!CFGfieldLoadGrammar(GrammarPath, &Grammar, &Error)) { return COLrespondError(Request.Address, Error); }
   COLvar GrammarWebVar;
   for(const auto& Field : Grammar) {
      COLvar FieldVar;
      FieldVar["name"]    = Field.Name;
      FieldVar["default"] = Field.Default;
      FieldVar["type"]    = CFGfieldTypeToString(Field.FieldType);
      FieldVar["options"] = Field.Options;
      if(!Field.Description.is_null()) { FieldVar["description"] = Field.Description; }
      GrammarWebVar.push_back(FieldVar);
   }
   COL_VAR(GrammarWebVar);
   COLrespondSuccess(Request.Address, GrammarWebVar);
}
