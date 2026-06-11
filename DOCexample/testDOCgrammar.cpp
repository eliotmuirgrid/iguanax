// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testDOCgrammar
//
// Description:
//
// Implementation of tests
//
// Author: Eliot Muir 
// Date:   Tuesday 26 August 2025 - 01:10PM
// ---------------------------------------------------------------------------
#include "testDOCgrammar.h"

#include <DOC/DOCgrammar.h>
#include <DOC/DOCdocumentGrammar.h>
#include <DOC/DOCdocument.h>
#include <DOC/DOCrandomTest.h>
#include <DOC/DOCserialize.h>

#include <UNIT/UNITapp.h>

#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static void testGrammar(){
   COL_FUNCTION(testGrammar);
   DOCdocument Document;
   DOCdocumentCreateRandom(&Document);
   COLvar Data;
   DOCtoVar(Document, &Data);
   Data["data"] = "Content of document";
   Data["title"] = "A title";
   Data["created"] = 2342342;
   
   COLvar Participant;
   Participant["name"] = "smith";
   Participant["role"] = "author";
   
   Data["participants"].push_back(Participant);
   Participant["life"] = "sdfs";
   Data["participants"].push_back(Participant);

   COLstring Error;
   const DOCgrammar& Grammar = DOCdocumentGrammar();
   bool Result = DOCgrammarCheck(Data, Grammar, &Error);
   COL_VAR3(Result, Error, Data);
   UNIT_ASSERT_EQUALS(Result, false);   
}

void testDOCgrammar(UNITapp* pApp) {
   pApp->add("test/grammar", &testGrammar);
}
