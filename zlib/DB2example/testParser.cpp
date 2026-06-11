// ---------------------------------------------------------------------------
//  Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: testParser
//
//  Description
//
//  Implementation
//
//  Author: Vismay Shah
//  Date:   Mon 13 Mar 2023 15:02:52 EST
// ---------------------------------------------------------------------------
#include "testParser.h"

#include <UNIT/UNITapp.h>

#include <DB/DBgrammar.h>
#include <DB/DBvmdParser.h>

#include <COL/COLlog.h>
#include <FIL/FILutils.h>
COL_LOG_MODULE;

static void testParserLegacy(){
   COL_FUNCTION(testParserLegacy);
   COLstring LegacyVmd;
   FILreadFile("legacy.vmd", &LegacyVmd);
   DBcollection Result;
   DBtranslateVmd(LegacyVmd, &Result);
   UNIT_ASSERT_EQUALS(3, Result.Tables.size());
}

static void testParserLegacyBasic(){
   COL_FUNCTION(testParserLegacyBasic);
   COLstring LegacyVmd;
   FILreadFile("basic.vmd", &LegacyVmd);
   DBcollection Result;
   DBtranslateVmd(LegacyVmd, &Result);
   UNIT_ASSERT_EQUALS(1, Result.Tables.size());
   UNIT_ASSERT_EQUALS(1, Result.Tables.count("Basic"));
   UNIT_ASSERT_EQUALS("Basic table with all column types", Result.Tables["Basic"].Description);
   UNIT_ASSERT_EQUALS(4, Result.Tables["Basic"].Columns.size());
   UNIT_ASSERT_EQUALS("C1stringKey",Result.Tables["Basic"].Columns[0].Name);
   UNIT_ASSERT_EQUALS(DBS_STRING,   Result.Tables["Basic"].Columns[0].Type);
   UNIT_ASSERT_EQUALS(true,         Result.Tables["Basic"].Columns[0].IsKey);
   UNIT_ASSERT_EQUALS("C2int",      Result.Tables["Basic"].Columns[1].Name);
   UNIT_ASSERT_EQUALS(DBS_INTEGER,  Result.Tables["Basic"].Columns[1].Type);
   UNIT_ASSERT_EQUALS(false,        Result.Tables["Basic"].Columns[1].IsKey);
   UNIT_ASSERT_EQUALS("C3date",     Result.Tables["Basic"].Columns[2].Name);
   UNIT_ASSERT_EQUALS(DBS_DATETIME, Result.Tables["Basic"].Columns[2].Type);
   UNIT_ASSERT_EQUALS(false,        Result.Tables["Basic"].Columns[2].IsKey);
   UNIT_ASSERT_EQUALS("C4double",   Result.Tables["Basic"].Columns[3].Name);
   UNIT_ASSERT_EQUALS(DBS_DOUBLE,   Result.Tables["Basic"].Columns[3].Type);
   UNIT_ASSERT_EQUALS(false,        Result.Tables["Basic"].Columns[3].IsKey);
}

void testParser(UNITapp* pApp){
   pApp->add("parse/legacy",         &testParserLegacy);
   pApp->add("parse/legacy/basic",   &testParserLegacyBasic);

}
