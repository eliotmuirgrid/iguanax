//---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DOCexample
//
// Description:
//
// DOCexample for testing DOC library
//
// Author: Eliot Muir
// Date:   Wed 29 Mar 2023 10:14:29 EDT
//---------------------------------------------------------------------------
#include "testDOCdocument.h"
#include "testDOCgrammar.h"
#include "testDOCsearchThreaded.h"

#include <UNIT/UNITapp.h>

#include <COL/COLweb.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void testDummyCallback(COLuint64 ConnectionId, const COLstring& Result){
   COL_FUNCTION(testDummyCallback);
   COL_VAR2(ConnectionId, Result);
}

// Run with as little boiler plate as possible.
int main(int argc, const char** argv){
   UNITapp App;
   COLwebSetResponseCallback(COLnewClosure2(&testDummyCallback));
   //COLlog::init(argc, argv);
   testGenerateRandomRepo();

   testDOCdocument(&App);
   testDOCgrammar(&App);
   testDOCsearchThreaded(&App);
   return App.run(argc, argv);
}
