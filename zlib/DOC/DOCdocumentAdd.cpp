// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DOCdocumentAdd
//
// Description:
//
// Implementation
//
// Author: Stephen Piccone 
// Date:   Friday 26 September 2025 - 12:06PM
// ---------------------------------------------------------------------------

#include <DOC/DOCdocumentAdd.h>
#include <DOC/DOCdocument.h>
#include <DOC/DOChex.h>
#include <DOC/DOCserialize.h>
#include <DOC/DOCgrammar.h>
#include <DOC/DOCdocumentGrammar.h>
#include <DOC/DOCdir.h>

#include <LUA/LUAutils.h>
#include <LUA/LUAerror.h>
#include <LUA/LUAopen.h>

#include <FIL/FILutils.h>

#include <SFI/SFIbase64DecodeFilter.h>

#include <DIR/DIRpath.h>

#include <ctime>

#include <COL/COLvarUtils.h>
#include <COL/COLostream.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static bool DOCguidExists(COLstring& Guid){
   COLstring Path = DIRcomponentDevDir(Guid);
   return FILfileExists(Path);
}

void DOCdocumentAdd(const COLvar& Data, COLvar* pResult){
   COL_FUNCTION(DOCdocumentAdd);  
   COLstring Error;
   if (!DOCgrammarCheck(Data, DOCdocumentGrammar(), &Error)){ throw COLerror(Error, 0); }

   DOCdocument Doc;
   DOCfromVar(Data,&Doc);
   DOCdocumentSanitizeTags(&Doc);
   COLstring Component = Data["component"];

   if (!DOCguidExists(Component))  { throw COLerror("Component " + Component + " does not exist.", 0); }

   Doc.Id = DOCnewId(Doc);
   Doc.Created = strtoull(Data["created"].asString().c_str(), NULL, 10);
   COL_VAR(Doc.Created);
   DOCdocumentAddDateTag(&Doc);
   COL_VAR(Doc);
   if (FILfileExists(DOCfileMeta(Component, Doc))){
      COLstring Content;
      FILreadFile(DOCfileMeta(Component, Doc), &Content);
      DOCdocument Orig;
      DOCfromJson(Content, &Orig);
      DOCdocumentMergeTags(Orig, &Doc);
      DOCdocumentMergeRelations(Orig, &Doc); // copy existing relationships  
   }
   DOCsaveDoc(Doc,Component);
   DOCsaveOrig(Data["data"], Doc, Component);
   DOCsaveMd(Data["markdown"], Doc, Component);
   DOCdocumentAddress(Doc, Component, pResult); // Give the co-ordinates of the added document
}

void DOCwebDocumentUpload(const COLwebRequest& Request){
   COL_FUNCTION(DOCwebDocumentUpload);
   COL_VAR(Request.Data.json(1));

   COLvar RequestData = Request.Data;
   COLstring DecodedData;
   SFIbase64Decode(Request.Data["data"],&DecodedData);

   RequestData["data"] = DecodedData;

   COLvar Result;

   DOCdocumentAdd(RequestData, &Result);
   COLrespondSuccess(Request.Address,Result);
}