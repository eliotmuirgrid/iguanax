// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DOCdocumentDelete
//
// Description:
//
// Implementation
//
// Author: Stephen Piccone 
// Date:   Wednesday 01 October 2025 - 10:42AM
// ---------------------------------------------------------------------------

#include <DOC/DOCdocumentDelete.h>
#include <DOC/DOCdocument.h>
#include <DOC/DOCserialize.h>
#include <DOC/DOCdir.h>

#include <FIL/FILutils.h>

#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void DOCdocumentDelete(const COLvar& Data, COLvar* pResult){
   COLvar& Result = *pResult;
   COLstring ComponentId  = COLvarRequiredString(Data, "component");
   COLstring Id           = COLvarRequiredString(Data, "id");
   COLstring Created      = COLvarRequiredString(Data, "created");

   COL_VAR3(ComponentId, Id, Created);

   DOCdocument Doc;
   DOCreadDoc(ComponentId, Id, Created, &Doc);
   //The files we need to delete
   COLstring DocMeta = DOCfileMeta(ComponentId, Doc);
   COLstring DocMd   = DOCfileMd(ComponentId, Doc);
   COLstring DocBase = DOCfileOriginal(ComponentId, Doc);
   //The parent dir
   COLstring DocDir = DOCfileBase(ComponentId, Doc);
   size_t LastSep = DocDir.find_last_of("/\\");
   DocDir = DocDir.substr(0,LastSep);

   FILdeleteFileIfExists(DocMeta);
   FILdeleteFileIfExists(DocMd);
   FILdeleteFileIfExists(DocBase);
   Result["result"] = "Document deleted";
}
void DOCwebDocumentDelete(const COLwebRequest& Request){
   COL_FUNCTION(DOCwebDocumentDelete);
   COL_VAR(Request.Data.json(1));
   COLvar Result;
   DOCdocumentDelete(Request.Data,&Result);
   COLrespondSuccess(Request.Address,Result);
}
