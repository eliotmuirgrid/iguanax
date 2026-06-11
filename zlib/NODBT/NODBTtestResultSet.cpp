//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NODBTtestResultSet
//
// Description:
//
// These test cases will construct DBresultSets, instead of performing
// actual queries.
//
// Author: Kevin Senn
// Date:   Wednesday, September 29th, 2010 @ 02:44:50 PM
//
//---------------------------------------------------------------------------
#include "NODBTprecomp.h"
#pragma hdrstop

#include "NODBTtestResultSet.h"

#include <TST/TSThelpers.h>
#include <TST/TSTtestCollection.h>
#include <TST/TSTtestFunction.h>

#include <THTM/THTMprinter.h>

#include <NODT/NODTutils.h>

#include <NODB/NODBresultSet.h>
#include <NODB/NODBtreeValue.h>

#include <DB/DBresultSetPtr.h>
#include <DB/DBresultSetRow.h>
#include <DB/DBvariant.h>

#include <COL/COLdateTime.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static COLstring NODTtreeValue(const NODplace& Place){
   COLstring Result;
   COLostream ResultStream(Result);
   NODBtreeValue(ResultStream, Place, false);
   return Result;
}

static void NODBtestCompletelyEmptyResultSet()
{
   DBresultSetPtr pRawResultSet = new DBresultSet();
   NODBresultSet ResultSet(pRawResultSet.get());

   TST_ASSERT_EQUALS(true, ResultSet.isNull());
   TST_ASSERT_EQUALS(true, ResultSet.isVector());
   TST_ASSERT_EQUALS(true, ResultSet.isReadOnly());
   TST_ASSERT_EQUALS(NODB_DB, ResultSet.protocolType());
   TST_ASSERT_EQUALS("db", ResultSet.protocolName());
   TST_ASSERT_EQUALS(NODB_RESULT_SET, ResultSet.nodeType());
   TST_ASSERT_EQUALS("result_set", ResultSet.nodeTypeName());
   TST_ASSERT_EQUALS(0, ResultSet.size());
   TST_ASSERT_EQUALS("", NODTtreeValue(ResultSet));

   COLref<NODplace> pDummyRow = ResultSet.createDummyItem();
   TST_ASSERT_EQUALS(true, pDummyRow->isNull());
   TST_ASSERT_EQUALS(true, pDummyRow->isComplex());
   TST_ASSERT_EQUALS(NODB_DB, pDummyRow->protocolType());
   TST_ASSERT_EQUALS("db", pDummyRow->protocolName());
   TST_ASSERT_EQUALS(NODB_ROW, pDummyRow->nodeType());
   TST_ASSERT_EQUALS("row", pDummyRow->nodeTypeName());
   TST_ASSERT_EQUALS(0, pDummyRow->size());
   TST_ASSERT_EQUALS("", NODTtreeValue(*pDummyRow));
}

static void NODBtestResultSetNoRows()
{
   DBresultSetPtr pRawResultSet = new DBresultSet();
   pRawResultSet->addColumn("StringColumn", DB_STRING);
   pRawResultSet->addColumn("IntColumn", DB_INTEGER);
   pRawResultSet->addColumn("DoubleColumn", DB_DOUBLE);
   pRawResultSet->addColumn("DateTimeColumn", DB_DATETIME);

   NODBresultSet ResultSet(pRawResultSet.get());

   TST_ASSERT_EQUALS(true, ResultSet.isNull());
   TST_ASSERT_EQUALS(true, ResultSet.isVector());
   TST_ASSERT_EQUALS(NODB_DB, ResultSet.protocolType());
   TST_ASSERT_EQUALS("db", ResultSet.protocolName());
   TST_ASSERT_EQUALS(NODB_RESULT_SET, ResultSet.nodeType());
   TST_ASSERT_EQUALS("result_set", ResultSet.nodeTypeName());
   TST_ASSERT_EQUALS(0, ResultSet.size());
   TST_ASSERT_EQUALS("", NODTtreeValue(ResultSet));

   COLref<NODplace> pDummyRow = ResultSet.createDummyItem();
   TST_ASSERT_EQUALS(true, pDummyRow->isNull());
   TST_ASSERT_EQUALS(true, pDummyRow->isComplex());
   TST_ASSERT_EQUALS(NODB_DB, pDummyRow->protocolType());
   TST_ASSERT_EQUALS("db", pDummyRow->protocolName());
   TST_ASSERT_EQUALS(NODB_ROW, pDummyRow->nodeType());
   TST_ASSERT_EQUALS("row", pDummyRow->nodeTypeName());
   TST_ASSERT_EQUALS(4, pDummyRow->size());
   TST_ASSERT_EQUALS("NULL, NULL, NULL, NULL", NODTtreeValue(*pDummyRow));

   NODplace& Column1 = pDummyRow->child("StringColumn");
   TST_ASSERT_EQUALS(&Column1, &pDummyRow->child(0));
   TST_ASSERT_EQUALS(true, Column1.isNull());
   TST_ASSERT_EQUALS(true, Column1.isSimple());
   TST_ASSERT_EQUALS(NODB_DB, Column1.protocolType());
   TST_ASSERT_EQUALS("db", Column1.protocolName());
   TST_ASSERT_EQUALS(NODB_VALUE_STRING, Column1.nodeType());
   TST_ASSERT_EQUALS("string", Column1.nodeTypeName());
   TST_ASSERT_EQUALS("NULL", NODTtreeValue(Column1));
}

static void NODBtestResultSetSeveralRows()
{
   DBresultSetPtr pRawResultSet = new DBresultSet();
   pRawResultSet->addColumn("StringColumn", DB_STRING);
   pRawResultSet->addColumn("IntColumn", DB_INTEGER);
   pRawResultSet->addColumn("DoubleColumn", DB_DOUBLE);
   pRawResultSet->addColumn("DateTimeColumn", DB_DATETIME);
   pRawResultSet->addRow();
   pRawResultSet->setValue(0, 0, COLstring("foo"));
   pRawResultSet->setValue(0, 1, 37);
   pRawResultSet->setValue(0, 2, 9.99);
   pRawResultSet->setValue(0, 3, COLdateTime(2010, 9, 29, 15, 54, 47));
   pRawResultSet->addRow();
   // Leave the second row as all null values.

   NODBresultSet ResultSet(pRawResultSet.get());

   TST_ASSERT_EQUALS(false, ResultSet.isNull());
   TST_ASSERT_EQUALS(2, ResultSet.size());
   TST_ASSERT_EQUALS("{'foo', 37, 9.990000, '2010-09-29 15:54:47'}, {NULL, NULL, NULL, NULL}", NODTtreeValue(ResultSet));

   NODplace& Row1 = ResultSet.child(0);
   TST_ASSERT_EQUALS(false, Row1.isNull());
   TST_ASSERT_EQUALS(4, Row1.size());
   NODplace& Row1Column1 = Row1.child("StringColumn");
   TST_ASSERT_EQUALS(&Row1Column1, &Row1.child(0));
   TST_ASSERT_EQUALS(false, Row1Column1.isNull());
   TST_ASSERT_EQUALS(NODB_VALUE_STRING, Row1Column1.nodeType());
   TST_ASSERT_EQUALS("string", Row1Column1.nodeTypeName());
   TST_ASSERT_EQUALS("foo", Row1Column1.value());
   TST_ASSERT_EQUALS("foo", NODTtreeValue(Row1Column1));
   NODplace& Row1Column2 = Row1.child("IntColumn");
   TST_ASSERT_EQUALS(&Row1Column2, &Row1.child(1));
   TST_ASSERT_EQUALS(false, Row1Column2.isNull());
   TST_ASSERT_EQUALS(NODB_VALUE_INTEGER, Row1Column2.nodeType());
   TST_ASSERT_EQUALS("integer", Row1Column2.nodeTypeName());
   TST_ASSERT_EQUALS("37", Row1Column2.value());
   TST_ASSERT_EQUALS("37", NODTtreeValue(Row1Column2));
   NODplace& Row1Column3 = Row1.child("DoubleColumn");
   TST_ASSERT_EQUALS(&Row1Column3, &Row1.child(2));
   TST_ASSERT_EQUALS(false, Row1Column3.isNull());
   TST_ASSERT_EQUALS(NODB_VALUE_DOUBLE, Row1Column3.nodeType());
   TST_ASSERT_EQUALS("double", Row1Column3.nodeTypeName());
   TST_ASSERT_EQUALS("9.990000", Row1Column3.value());
   TST_ASSERT_EQUALS("9.990000", NODTtreeValue(Row1Column3));
   NODplace& Row1Column4 = Row1.child("DateTimeColumn");
   TST_ASSERT_EQUALS(&Row1Column4, &Row1.child(3));
   TST_ASSERT_EQUALS(false, Row1Column4.isNull());
   TST_ASSERT_EQUALS(NODB_VALUE_DATETIME, Row1Column4.nodeType());
   TST_ASSERT_EQUALS("datetime", Row1Column4.nodeTypeName());
   TST_ASSERT_EQUALS("2010-09-29 15:54:47", Row1Column4.value());
   TST_ASSERT_EQUALS("2010-09-29 15:54:47", NODTtreeValue(Row1Column4));

   NODplace& Row2 = ResultSet.child(1);
   TST_ASSERT_EQUALS(false, Row2.isNull());
   TST_ASSERT_EQUALS(4, Row2.size());
   NODplace& Row2Column1 = Row2.child("StringColumn");
   TST_ASSERT_EQUALS(true, Row2Column1.isNull());
   TST_ASSERT_EQUALS(NODB_VALUE_STRING, Row2Column1.nodeType());
   TST_ASSERT_EQUALS("string", Row2Column1.nodeTypeName());
   TST_ASSERT_EQUALS("NULL", NODTtreeValue(Row2Column1));
   NODplace& Row2Column2 = Row2.child("IntColumn");
   TST_ASSERT_EQUALS(true, Row2Column2.isNull());
   TST_ASSERT_EQUALS(NODB_VALUE_INTEGER, Row2Column2.nodeType());
   TST_ASSERT_EQUALS("integer", Row2Column2.nodeTypeName());
   TST_ASSERT_EQUALS("NULL", NODTtreeValue(Row2Column2));
   NODplace& Row2Column3 = Row2.child("DoubleColumn");
   TST_ASSERT_EQUALS(true, Row2Column3.isNull());
   TST_ASSERT_EQUALS(NODB_VALUE_DOUBLE, Row2Column3.nodeType());
   TST_ASSERT_EQUALS("double", Row2Column3.nodeTypeName());
   TST_ASSERT_EQUALS("NULL", NODTtreeValue(Row2Column3));
   NODplace& Row2Column4 = Row2.child("DateTimeColumn");
   TST_ASSERT_EQUALS(true, Row2Column4.isNull());
   TST_ASSERT_EQUALS(NODB_VALUE_DATETIME, Row2Column4.nodeType());
   TST_ASSERT_EQUALS("datetime", Row2Column4.nodeTypeName());
   TST_ASSERT_EQUALS("NULL", NODTtreeValue(Row2Column4));
}

TSTtestable* NODBTtestResultSet()
{
   TST_START_COLLECTION("NODBTtestResultSet")
      TST_ADD_TEST_FUNCTION(NODBtestCompletelyEmptyResultSet);
      TST_ADD_TEST_FUNCTION(NODBtestResultSetNoRows);
      TST_ADD_TEST_FUNCTION(NODBtestResultSetSeveralRows);
   TST_END_FIXTURE
}
