//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: testTRNblockCompletion.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   06/12/23 11:26 AM
//  ---------------------------------------------------------------------------
#include "testTRNblockCompletion.h"
#include <TRN/TRNautocompleteEnd.h>
#include <TRN/TRNannotationState.h>

#include <UNIT/UNITapp.h>

#include <TRN/TRNannotationJob.h>

#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static COLstring basicNested()   { return "function main()\n\tif true then\n      for i=1, 5 do\n         \n   end\nend"; }
static COLstring basicNposCode() { return "function main()\n"; }
static COLstring basicIfCode()   { return "function main()\n   if true then\n   \nend"; }

static COLvar generateErrorVar(const COLstring& Error, int Line) {
   COL_FUNCTION(generateErrorVar);
   COLvar ErrorVar;
   ErrorVar["ScriptError"]["Description"] = Error;
   ErrorVar["ScriptError"]["LineNumber"]  = Line;
   return ErrorVar;
}

static void testBasicNested() {
   COL_FUNCTION(testBasicNested);
   TRNannotationState State;
   COLvar Result = generateErrorVar("basicNested.lua:4: 'end' expected (to close 'function' at line 1) near '<eof>'", 1);
   TRNautocompleteEnd(basicNested(), 3, &Result, &State);
   COL_VAR(Result.json(1));
   UNIT_ASSERT(Result["autocomplete"].asBool());
   UNIT_ASSERT_EQUALS("function main()\n\tif true then\n      for i=1, 5 do\n         \nend\n   end\nend", Result["code"]);
}

static void testNpos() {
   COL_FUNCTION(testNpos);
   TRNannotationState State;
   COLvar Result = generateErrorVar("basicNposCode.lua:4: 'end' expected (to close 'function' at line 4) near '<eof>'", 1);
   TRNautocompleteEnd(basicNposCode(), 1, &Result, &State);
   COL_VAR(Result.json(1));
   UNIT_ASSERT(Result["autocomplete"].asBool());
   UNIT_ASSERT_EQUALS("function main()\n\nend", Result["code"]);
}

static void testBlockIf() {
   COL_FUNCTION(testBlockIf);
   TRNannotationState State;
   COLvar Result = generateErrorVar("basicIf.lua:1: 'end' expected (to close 'function' at line 1) near '<eof>'", 1);
   TRNautocompleteEnd(basicIfCode(), 2, &Result, &State);
   COL_VAR(Result.json(1));
   UNIT_ASSERT(Result["autocomplete"].asBool());
   UNIT_ASSERT_EQUALS("function main()\n   if true then\n   \nend\nend", Result["code"]);
}

void testTRNblockCompletion(UNITapp* pApp) {
   pApp->add("block/if",     &testBlockIf);
   pApp->add("block/npos",   &testNpos);
   pApp->add("block/nested", &testBasicNested);
}