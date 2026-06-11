// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testDOCdocument
//
// Description:
//
// Implementation of tests
//
// Author: Eliot Muir 
// Date:   Tuesday 19 August 2025 - 09:17AM
// ---------------------------------------------------------------------------
#include "testDOCdocument.h"

#include <DOC/DOCdocument.h>
#include <DOC/DOCserialize.h>
#include <DOC/DOChex.h>
#include <DOC/DOCdir.h>
#include <DOC/DOCtag.h>
#include <DOC/DOCindex.h>
#include <DOC/DOCtagTree.h>
#include <DOC/DOCrandomTest.h>
#include <DOC/DOCindexCollection.h>
#include <DOC/DOCluaAdd.h>
#include <DOC/DOCdocumentGet.h>

#include <FIL/FILutils.h>

#include <GUID/GUIDid.h>

#include <UNIT/UNITapp.h>

#include <COL/COLweb.h>
#include <COL/COLmath.h>
#include <COL/COLlog.h>
#include <COL/COLvarUtils.h>
#include <COL/COLstring.h>
#include <COL/COLostream.h>

#include <stdlib.h>

COL_LOG_MODULE;

static void testSimple(){
   COL_FUNCTION(testSimple);
   DOCdocument Doc;
   Doc.Data = "A document body";
   Doc.Created = 21342342;
   Doc.Id = DOCnewId(Doc);
   Doc.Source = "email";
   Doc.Relations.push_back(DOCrelation("12345","legal"));
   Doc.addTag("legal_separation_agreement");
   Doc.addTag("legal_will");
   
   COLstring Out;
   DOCtoJson(Doc, &Out);
   COL_VAR(Out);

   DOCdocument Doc2;
   DOCfromJson(Out, &Doc2);
   COLstring Out2;
   DOCtoJson(Doc2, &Out2);
   COL_VAR(Out2);
}

static void testId(){
   COL_FUNCTION(testId);
   DOCdocument Doc;
   Doc.Data = "abcd";
   Doc.Created = 233233223;
   COL_VAR(DOCnewId(Doc));
}

static void testLookup(){
   COL_FUNCTION(testLookup);
   DOCdocument Doc;
   Doc.Data = "abcd";
   Doc.Created = 233233223;
   Doc.Id = DOCnewId(Doc);
   COL_VAR(DOCfileGuid(Doc));
}

static void testTag(){
   COL_FUNCTION(testTag);
   COL_VAR(DOCtagDir("legal_important_separation", "abc"));
}

void testGenerateRandomRepo(){
   COL_FUNCTION(testGenerateRandomRepo);
   COLstring Error;
   FILremoveFullDirNewSafe("./config", &Error);
   COLstring Id = "abc";
   DOCrandomTestGenerate(Id);
}

static void testApi(){
   COL_FUNCTION(testApi);
   COLwebRequest Request;
   Request.Data["tag"] = "legal_important_dash";
   Request.Data["component"]  = "abc";
   DOCtagTree(Request);
   COL_VAR(Request);

   // doc.insert{data=doc_table, id='sdaafsdfkjsdfjhk_sxdfsdf'}
}

static void testIndex(){
   COL_FUNCTION(testIndex);
   COLwebRequest Request;
   Request.Data["component"]  = "abc";
   DOCindexCollection(Request);
   COL_VAR(Request);
}

static void testHexToInt(){
   COL_FUNCTION(testHexToInt);
   COLwebRequest Request;
   Request.Data["created"] = "6803ce05";
   DOCwebHexStringToInt(Request);
   COL_VAR(Request);

   
}

static void testGetMd(){
   COLwebRequest Request;
   Request.Data["component"]  = "abc";
   Request.Data["tag"]        = "feature_medium_conflict";
   Request.Data["docid"]      = "3f99cb13d6b1612c3d0a39247a9e3a7b";

   DOCwebDocumentGet(Request);
   COL_VAR(Request);

}

static void testUnixHex(){
   COL_FUNCTION(testUnixHex);
   COLuint64 sample = time(NULL) - COLmath::randomNumber(64000000);
   COL_VAR(sample);

}

void testDOCdocument(UNITapp* pApp) {
   pApp->add("test/serial",    &testSimple);
   pApp->add("test/id",        &testId);
   pApp->add("test/lookup",    &testLookup);
   pApp->add("test/tag",       &testTag);
   pApp->add("test/api",       &testApi);
   //pApp->add("test/index",     &testIndex);
   pApp->add("test/hexToInt",  &testHexToInt);
   pApp->add("test/randomGen", &testGenerateRandomRepo);
   //pApp->add("test/getMd",     &testGetMd);
   pApp->add("test/UnixHex",   &testUnixHex);
}
