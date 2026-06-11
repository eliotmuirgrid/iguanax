// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DOCrandomTest
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Sunday 24 August 2025 - 03:17PM
// ---------------------------------------------------------------------------

#include <DOC/DOCrandomTest.h>
#include <DOC/DOCdocument.h>
#include <DOC/DOCserialize.h>
#include <DOC/DOCindex.h>
#include <DOC/DOCtag.h>

#include <time.h>

#include <COL/COLvarUtils.h>
#include <COL/COLvar.h>
#include <COL/COLweb.h>
#include <COL/COLmath.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static DOCrelation DOCrandomRelation(){
   switch(COLmath::randomNumber(3)){
   case 0:  return DOCrelation("1234", "email chain");
   case 1:  return DOCrelation("4567", "email chain");
   default: return DOCrelation("7654", "email chain");
   };
}

static COLstring DOCrandomTag(){
   COLstring Tag;
   switch(COLmath::randomNumber(3)){
   case 0:  Tag += "legal";    break;
   case 1:  Tag += "customer"; break;
   default: Tag += "feature";  break;
   };
   switch(COLmath::randomNumber(3)){
   case 0:  Tag += "_important"; break;
   case 1:  Tag += "_noise";     break;
   default: Tag += "_medium";
   };
   switch(COLmath::randomNumber(3)){
   case 0:  Tag += "_dash";     break;
   case 1:  Tag += "_conflict"; break;
   default: Tag += "_life";     break;
   };
   return Tag;
}

void DOCdocumentCreateRandom(DOCdocument* pDocument){
   COL_FUNCTION(DOCdocumentCreateRandom);
   pDocument->Created = time(NULL) - COLmath::randomNumber(64000000);
   pDocument->Source = "email";
   pDocument->Data = "Heres some fake original data " + COLintToString(pDocument->Created);
   pDocument->Markup = "Some markup " + COLintToString(pDocument->Created);
   pDocument->Title = "A title of life" + COLintToString(pDocument->Created);
   pDocument->Id = DOCnewId(*pDocument);
   pDocument->Relations.push_back(DOCrandomRelation());
   int TagCount = COLmath::randomNumber(4);
   for (int i=0; i< TagCount; i++){
      pDocument->addTag(DOCrandomTag());
   }
}

void DOCrandomTestGenerate(const COLstring& Id){
   COL_FUNCTION(DOCrandomTestGenerate);
   COLstring Error;
   COLstring IndexDir = DOCtagDir("", Id);

   for (int i=0; i < 10; i++){
      DOCdocument Doc;
      DOCdocumentCreateRandom(&Doc);
      COL_VAR(Doc);
      DOCsaveDoc(Doc, Id);
      DOCsaveMd(Doc.Data,Doc,Id);
      DOCindex(Doc, Id);
   }
}


void DOCrandomTest(const COLwebRequest& Request){
   COL_FUNCTION(DOCrandomTest);
   COLstring Id  = COLvarRequiredString(Request.Data, "component");
   DOCrandomTestGenerate(Id);
   COLrespondSuccess(Request.Address);
}