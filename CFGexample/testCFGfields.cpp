// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testCFGfields
//
// Description:
//
// Implementation of tests
//
// Author: Eliot Muir
// Date:   Friday 14 April 2023 - 10:22AM
// ---------------------------------------------------------------------------
#include "testCFGfields.h"

#include <CFG/CFGfieldGrammar.h>
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <FIL/FILutils.h>
#include <PASS/PASSword.h>
#include <UNIT/UNITapp.h>
COL_LOG_MODULE;

static bool loadGrammar(COLarray<CFGfield>* pFieldGrammar) {
   COL_FUNCTION(loadGrammar);
   PASSsetKeyIV("test", "test");
   COLstring Err;
   COLvar Data;
	bool Success = CFGfieldLoadGrammar("config/config.json", pFieldGrammar, &Err);
	COL_VAR2(Success, Err);
	return Success;
}

static bool loadValues(const COLstring& Path, COLmap<COLstring, COLstring>* pValues) {
   COL_FUNCTION(loadValues);
   COL_VAR(Path);
   COLstring Err;
	if(!FILfileExists(Path)) {
		COL_ERR(Path + " does not exist");
		return true;
	}
	try {
		COLstring RawData;
		FILreadFile(Path, &RawData);
		if(!RawData.is_null()) {
			COLvar Json;
			if(!Json.parse(RawData)) { COL_ERR("Failed to parse field grammar"); return false; }
			COL_VAR(Json.json(1));
			if(!Json["fields"].isMap()) { return false; }
			for(const auto& it : Json["fields"].map()) { pValues->add(it.first, it.second); }
		}
		return true;
	}
	catch(const COLerror& E) {
		COL_ERR(E.description());
		return false;
	}
}

static void testSanitizeMissing() {
   COL_FUNCTION(testSanitizeMissing);
   COLarray<CFGfield> Grammar;
   COLmap<COLstring, COLstring> Values;
   UNIT_ASSERT(loadGrammar(&Grammar))
   UNIT_ASSERT(loadValues("config/full_fields.json", &Values))
   COLstring Err;
   UNIT_ASSERT(!CFGfieldIsValid(Grammar, "iDontExist", Values["iDontExist"], &Err))
}

static void testSanitizePassword() {
   COL_FUNCTION(testSanitizePassword);
   COLarray<CFGfield> Grammar;
   COLmap<COLstring, COLstring> Values;
   UNIT_ASSERT(loadGrammar(&Grammar))
   UNIT_ASSERT(loadValues("config/full_fields.json", &Values))
   COLstring Err;
   UNIT_ASSERT(CFGfieldIsValid(Grammar, "SomePassword", Values["SomePassword"], &Err))
   COLstring Empty = "";
   UNIT_ASSERT(CFGfieldIsValid(Grammar, "SomePassword", Empty, &Err))
}

static void testSanitizeBool() {
   COL_FUNCTION(testSanitizeBool);
   COLarray<CFGfield> Grammar;
   COLmap<COLstring, COLstring> Values;
   UNIT_ASSERT(loadGrammar(&Grammar))
   UNIT_ASSERT(loadValues("config/full_fields.json", &Values))
   COLstring Err;
   UNIT_ASSERT(CFGfieldIsValid(Grammar, "SomeBool", Values["SomeBool"], &Err))
   COLstring String = "asd";
   UNIT_ASSERT(!CFGfieldIsValid(Grammar, "SomeBool", String, &Err))
}

static void testSanitizePort() {
   COL_FUNCTION(testSanitizePort);
   COLarray<CFGfield> Grammar;
   COLmap<COLstring, COLstring> Values;
   UNIT_ASSERT(loadGrammar(&Grammar))
   UNIT_ASSERT(loadValues("config/full_fields.json", &Values))
   COLstring Err;
   UNIT_ASSERT(CFGfieldIsValid(Grammar, "SomePort", Values["SomePort"], &Err))
   COLstring String = "asd";
   UNIT_ASSERT(!CFGfieldIsValid(Grammar, "SomePort", String, &Err))
}

static void testSanitizeNumber() {
   COL_FUNCTION(testSanitizeNumber);
   COLarray<CFGfield> Grammar;
   COLmap<COLstring, COLstring> Values;
   UNIT_ASSERT(loadGrammar(&Grammar))
   UNIT_ASSERT(loadValues("config/full_fields.json", &Values))
   COLstring Err;
   UNIT_ASSERT(CFGfieldIsValid(Grammar, "SomeNumber", Values["SomeNumber"], &Err))
   COLstring String = "asd";
   UNIT_ASSERT(!CFGfieldIsValid(Grammar, "SomeNumber", String, &Err))
   COLstring Negative = "-22";
   UNIT_ASSERT(CFGfieldIsValid(Grammar, "SomeNumber", Negative, &Err))
}

static void testSanitizeFolder() {
   COL_FUNCTION(testSanitizeFolder);
   COLstring Err;
   COLarray<CFGfield> Grammar;
   COLmap<COLstring, COLstring> Values;
   UNIT_ASSERT(loadGrammar(&Grammar))
   UNIT_ASSERT(loadValues("config/full_fields.json", &Values))
   UNIT_ASSERT(CFGfieldIsValid(Grammar, "SomeFolder", Values["SomeFolder"], &Err))
   COLstring ExtraDirSep = "/Users/developermacbook/Library/iNTERFACEWARE/IguanaX/AOiYVxC3M8DY7HCxV4GBJL/cache/ComponentState.json";
   UNIT_ASSERT(CFGfieldIsValid(Grammar, "SomeFolder", ExtraDirSep, &Err)) // first check if true when missing sep
   ExtraDirSep.append(FIL_DIR_SEPARATOR); // add double sep
   ExtraDirSep.append(FIL_DIR_SEPARATOR);
   UNIT_ASSERT(CFGfieldIsValid(Grammar, "SomeFolder", ExtraDirSep, &Err))
}

static void testSanitizeFile() {
   COL_FUNCTION(testSanitizeFile);
   COLstring Err;
   COLarray<CFGfield> Grammar;
   COLmap<COLstring, COLstring> Values;
   UNIT_ASSERT(loadGrammar(&Grammar))
   UNIT_ASSERT(loadValues("config/full_fields.json", &Values))
   UNIT_ASSERT(CFGfieldIsValid(Grammar, "SomeFile", Values["SomeFile"], &Err))
   COLstring EndWithDirSep = "/Users/developermacbook/Library/iNTERFACEWARE/IguanaX/AOiYVxC3M8DY7HCxV4GBJL/cache/ComponentState.json";
   EndWithDirSep.append(FIL_DIR_SEPARATOR);
   UNIT_ASSERT(!CFGfieldIsValid(Grammar, "SomeFile", EndWithDirSep, &Err))
}

static void testSanitizeString() {
   COL_FUNCTION(testSanitizeString);
   COLstring Err;
   COLarray<CFGfield> Grammar;
   COLmap<COLstring, COLstring> Values;
   UNIT_ASSERT(loadGrammar(&Grammar))
   UNIT_ASSERT(loadValues("config/partial_fields.json", &Values))
   UNIT_ASSERT(CFGfieldIsValid(Grammar, "SomeString", Values["SomeString"], &Err))
   COLstring Empty = "";
   UNIT_ASSERT(CFGfieldIsValid(Grammar, "SomeString", Empty, &Err))
}

static void testLoadPartial() {
   COL_FUNCTION(testLoadPartial);
   COLmap<COLstring, COLstring> Values;
   UNIT_ASSERT(loadValues("config/partial_fields.json", &Values))
   COL_VAR(Values);
   UNIT_ASSERT_EQUALS(1, Values.size());
}

static void testInvalid(){
   COL_FUNCTION(testInvalid);
   COLstring Err;
   COLvar Data;
   Data.setMapType();
   COLarray<CFGfield> Grammar;
   bool Success = CFGfieldCreateGrammar(Data, &Grammar, &Err);
   UNIT_ASSERT(!Success);
}

static void testLoadGrammar() {
   COL_FUNCTION(testLoadGrammar);
   COLarray<CFGfield> Grammar;
   UNIT_ASSERT(loadGrammar(&Grammar))
   UNIT_ASSERT_EQUALS(7, Grammar.size())
}

void testCFGfields(UNITapp* pApp) {
   pApp->add("fields/load_grammar",      &testLoadGrammar);
   pApp->add("fields/invalid",           &testInvalid);
   pApp->add("fields/load_partial",      &testLoadPartial);
   pApp->add("fields/sanitize_string",   &testSanitizeString);
   pApp->add("fields/sanitize_file",     &testSanitizeFile);
   pApp->add("fields/sanitize_folder",   &testSanitizeFolder);
   pApp->add("fields/sanitize_number",   &testSanitizeNumber);
   pApp->add("fields/sanitize_port",     &testSanitizePort);
   pApp->add("fields/sanitize_bool",     &testSanitizeBool);
   pApp->add("fields/sanitize_password", &testSanitizePassword);
   pApp->add("fields/sanitize_missing",  &testSanitizeMissing);
}
