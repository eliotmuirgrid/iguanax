// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DOCmarkdown
//
// Description:
//
// Implementation
//
// Author: Stephen Piccone 
// Date:   Tuesday 02 September 2025 - 01:08PM
// ---------------------------------------------------------------------------

#include <DOC/DOCdocumentGet.h>
#include <DOC/DOCserialize.h>
#include <DOC/DOCtag.h>
#include <DOC/DOCdir.h>
#include <DOC/DOChex.h>
#include <DOC/DOCdocument.h>

#include <FIL/FILutils.h>

#include <DIR/DIRpath.h>

#include <COL/COLsplit.h>
#include <COL/COLarray.h>
#include <COL/COLvarUtils.h>
#include <COL/COLvar.h>
#include <COL/COLweb.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void DOCdocumentGet(const COLvar& Data, COLvar* pResult){
   COL_FUNCTION(DOCdocumentGet);
   COLvar& Result = *pResult;
   COLstring ComponentId  = COLvarRequiredString(Data, "component");
   COLstring Id           = COLvarRequiredString(Data, "id");
   COLstring Created      = COLvarRequiredString(Data, "created");

   COL_VAR3(ComponentId, Id, Created);

   DOCdocument Doc;
   DOCreadDoc(ComponentId, Id, Created, &Doc);   
   //DOCreadRelations(ComponentId,&Doc);
   COL_VAR(Doc);
   //DOCdocumentGetAddRelations(Doc, &Result["meta"]["relations"]);
   COLstring FileName = DOCfileMd(ComponentId, Doc);
   COLstring Markdown;
   FILreadFile(FileName, &Markdown);
   Result["markdown"] = Markdown;   
   DOCtoVar(Doc, &Result["meta"]);
   
   if (Data["include_source"].asBool()){
      // we get the full source document.
      COLstring Source;
      FILreadFile(DOCfileOriginal(ComponentId,Doc), &Source);
      Result["source"].swap(&Source);
   }
   COL_VAR(Result.json(1));
}

void DOCwebDocumentGet(const COLwebRequest& Request){
   COL_FUNCTION(DOCwebDocumentGet);
   COL_VAR(Request.Data.json(1));
   COLvar Result;
   DOCdocumentGet(Request.Data, &Result);
   COLrespondSuccess(Request.Address,Result);
}
