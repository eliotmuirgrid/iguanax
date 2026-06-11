// ---------------------------------------------------------------------------
//  Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: testSchema.cpp
//
//  Description
//
//  Implementation
//
//  Author: Eliot Muir 
//  Date:   Mon  7 Nov 2022 15:06:31 EST
// ---------------------------------------------------------------------------
#include "testSchema.h"

#include <UNIT/UNITapp.h>

#include <DB/DBgrammar.h>
#include <DB/DBgrammarSave.h>
#include <DB/DBgrammarLoad.h>


#include <COL/COLlog.h>
COL_LOG_MODULE;

static void addColumns(DBtable* pTable){
   COL_FUNCTION(addColumns);
   pTable->Columns.push_back(DBcolumn("ID",        "Unique ID"   ,    DBS_STRING,   true));
   pTable->Columns.push_back(DBcolumn("FirstName", "Given name"  ,    DBS_STRING,   false));
   pTable->Columns.push_back(DBcolumn("LastName",  "Surname name",    DBS_STRING,   false));
   pTable->Columns.push_back(DBcolumn("Title",     "Mr, Ms etc." ,    DBS_STRING,   false));
   pTable->Columns.push_back(DBcolumn("Age",       "In years"    ,    DBS_INTEGER,  false));
   pTable->Columns.push_back(DBcolumn("DOB",       "Birth date"  ,    DBS_DATETIME, false));
   pTable->Columns.push_back(DBcolumn("Height",    "In centimetres",  DBS_DOUBLE,   false));
}

static void addColumnsBill(DBtable* pTable){
   COL_FUNCTION(addColumnsBill);
   pTable->Columns.push_back(DBcolumn("Invoice#",  "Invoice"     ,                  DBS_STRING,   true));
   pTable->Columns.push_back(DBcolumn("ClientID",  "Given name"  ,                  DBS_INTEGER,  false));
   pTable->Columns.push_back(DBcolumn("Reason",    "A very very very good reason.", DBS_STRING,   false));
   pTable->Columns.push_back(DBcolumn("Amount",    "In USD",                        DBS_DOUBLE,   false));
}

static void createSimpleGrammar(DBcollection* pCollection){
   COL_FUNCTION(createSimpleGrammar);
   DBcollection& Collection = *pCollection;
   Collection.Tables.add("Patient", DBtable());
   Collection.Tables["Patient"].Description = "Patient Information";
   addColumns(&Collection.Tables["Patient"]);
   Collection.Tables.add("EmptyTable", DBtable());
   Collection.Tables.add("Bill", DBtable());
   Collection.Tables["Bill"].Description = "Bills and Stuff";
   addColumnsBill(&Collection.Tables["Bill"]);
}

static void createExport(DBcollection* pCollection, const COLstring& DbFile){
   COL_FUNCTION(createExport);
   DBcollection& Collection = *pCollection;
   if (Collection.Export.size()) { Collection.Export["datasource"] = DbFile;    } 
   else                          { Collection.Export.add("datasource", DbFile); }
}

static void testSchemaFormat(){
   COL_FUNCTION(testSchemaFormat);
   DBcollection Collection;
   createSimpleGrammar(&Collection);
   COLstring Content;
   DBsave(Collection, &Content);
   COLcout << Content << newline;
}

static void testSchemaFormatEmpty(){
   COL_FUNCTION(testSchemaFormatEmpty);
   DBcollection Collection;
   COLstring Content;
   DBsave(Collection, &Content);
   COLcout << Content << newline;
}

static void testSchemaLoad(){
   COL_FUNCTION(testSchemaLoad);
   DBcollection Orig;
   createSimpleGrammar(&Orig);
   COLstring OrigContent;
   DBsave(Orig, &OrigContent);
   DBcollection Copy;
   COLstring Error;
   DBload(OrigContent, &Copy, &Error);
   COLstring CopyContent;
   DBsave(Copy, &CopyContent);
   COLcout << CopyContent << newline;
   UNIT_ASSERT_EQUALS(OrigContent, CopyContent);
}

static void testSchemaWithExport(){
   COL_FUNCTION(testSchemaWithExport);
   DBcollection Orig;
   createSimpleGrammar(&Orig);
   createExport(&Orig, "foo/bar.sqlite");
   COLstring OrigContent;
   DBsave(Orig, &OrigContent);
   DBcollection Copy;
   COLstring Error;
   DBload(OrigContent, &Copy, &Error);
   COLstring CopyContent;
   DBsave(Copy, &CopyContent);
   COLcout << CopyContent << newline;
   UNIT_ASSERT_EQUALS(OrigContent, CopyContent);
}

static void testSchemaExportOverwrite(){
   COL_FUNCTION(testSchemaExportOverwrite);
   DBcollection Orig;
   createSimpleGrammar(&Orig);
   createExport(&Orig, "foo/bar.sqlite");
   COLstring OrigContent;
   DBsave(Orig, &OrigContent);
   DBcollection Copy;
   COLstring Error;
   DBload(OrigContent, &Copy, &Error);
   createExport(&Copy, "new/db.sqlite");
   COLstring CopyContent;
   DBsave(Copy, &CopyContent);
   int PosOrig, PosCopy;
   UNIT_ASSERT(OrigContent.find(&PosOrig, "foo/bar.sqlite"));
   UNIT_ASSERT(CopyContent.find(&PosOrig, "new/db.sqlite"));
}

static void testSchemaDuplicateColumns(){
   COL_FUNCTION(testSchemaDuplicateColumns);
   DBcollection Collection;
   COLstring Error;
   bool DummyLegacyWarning = false;
   COLstring VdbJson = "{\"tables\":{\"Table1\":{\"desc\":\"\",\"columns\":[{\"name\":\"test\",\"desc\":\"\",\"type\":\"string\"},{\"name\":\"test\",\"desc\":\"\",\"type\":\"string\"}]}}}";
   UNIT_ASSERT(!DBsmartLoad(VdbJson, &Collection, &Error, &DummyLegacyWarning));
   UNIT_ASSERT_EQUALS("Error loading vmd/vdb file: Encountered duplicate column: test in table: Table1", Error);
}

void testSchema(UNITapp* pApp){
   pApp->add("schema/format",       &testSchemaFormat);
   pApp->add("schema/format/empty", &testSchemaFormatEmpty);
   pApp->add("schema/load",         &testSchemaLoad);
   pApp->add("schema/export",       &testSchemaWithExport);
   pApp->add("schema/export/write", &testSchemaExportOverwrite);
   pApp->add("schema/duplicate_cols", &testSchemaDuplicateColumns);
}
