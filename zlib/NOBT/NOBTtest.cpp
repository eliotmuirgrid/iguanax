//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NOBTtest
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Wednesday, November 3rd, 2010 @ 01:37:38 PM
//
//---------------------------------------------------------------------------
#include "NOBTprecomp.h"
#pragma hdrstop

#include "NOBTtest.h"


#include <TST/TSThelpers.h>
#include <TST/TSTtestCollection.h>
#include <TST/TSTtestFunction.h>

#include <NOB/NOBserialize.h>
#include <NOB/NOBparse.h>
#include <NOB/NOBcopy.h>

#include <NOD/NODplace.h>
#include <NOD/NODaddress.h>

#include <NODB/NODBresultSet.h>

#include <NOX/NOXparse.h>

#include <NTV/NTVtreeValue.h>

#include <DB/DBresultSetPtr.h>
#include <DB/DBresultSetRow.h>
#include <DB/DBvariant.h>

#include <FIL/FILfile.h>
#include <FIL/FILutils.h>

#include <COL/COLauto.h>
#include <COL/COLdateTime.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;


#define NOBT_ASSERT_PROPERTY_EQUAL(_NODE_ADDRESS, _EXPECTED, _ACTUAL, _PROPERTY)\
   if (_EXPECTED._PROPERTY() != _ACTUAL._PROPERTY()){\
      COL_ERROR_STREAM_PLAIN("Property " << #_PROPERTY << " does not match for nodes at address " << _NODE_ADDRESS.parent()->name() << _NODE_ADDRESS << '.' << newline <<\
                             "Expected: [" << _EXPECTED._PROPERTY() << ']' << newline << \
                             "Actual:   [" << _ACTUAL._PROPERTY()   << ']', COLerror::PreCondition);\
   }

static void NOBTassertTreesEqual(const NODaddress& Address, const NODplace& Expected, const NODplace& Actual){
   // We do the assertions in an order that will hopefully give us the most
   // relevant detail first if the trees aren't equal.
   NOBT_ASSERT_PROPERTY_EQUAL(Address, Expected, Actual, protocolName);
   NOBT_ASSERT_PROPERTY_EQUAL(Address, Expected, Actual, protocolType);
   NOBT_ASSERT_PROPERTY_EQUAL(Address, Expected, Actual, name);
   NOBT_ASSERT_PROPERTY_EQUAL(Address, Expected, Actual, isNamed);
   NOBT_ASSERT_PROPERTY_EQUAL(Address, Expected, Actual, nodeTypeName);
   NOBT_ASSERT_PROPERTY_EQUAL(Address, Expected, Actual, nodeType);
   NOBT_ASSERT_PROPERTY_EQUAL(Address, Expected, Actual, grammarType);
   NOBT_ASSERT_PROPERTY_EQUAL(Address, Expected, Actual, isNull);
   NOBT_ASSERT_PROPERTY_EQUAL(Address, Expected, Actual, nodeType);

   // Compare treeValues
   COLstring ExpectedTreeValue, ActualTreeValue;
   COLostream ExpectedStream(ExpectedTreeValue), ActualStream(ActualTreeValue);
   NTVtreeValue(ExpectedStream, Expected, false);
   NTVtreeValue(ActualStream, Actual, false);
   if (ExpectedTreeValue != ActualTreeValue){
      COL_ERROR_STREAM_PLAIN("TreeValues don't match for nodes at address " << Address.parent()->name() << Address << '.' << newline <<
                             "Expected: [" << ExpectedTreeValue << ']' << newline <<
                             "Actual:   [" << ActualTreeValue << ']', COLerror::PreCondition);
   }

   switch (Actual.grammarType()){
      case NOD_SIMPLE:
         if (!Actual.isNull()){
            NOBT_ASSERT_PROPERTY_EQUAL(Address, Expected, Actual, value);
         }
         NOBT_ASSERT_PROPERTY_EQUAL(Address, Expected, Actual, shouldEscapeValue);
         break;
      case NOD_VECTOR:
         NOBT_ASSERT_PROPERTY_EQUAL(Address, Expected, Actual, isHomogeneous);
         break;
      case NOD_COMPLEX:
         NOBT_ASSERT_PROPERTY_EQUAL(Address, Expected, Actual, favorIndexByName);
         NOBT_ASSERT_PROPERTY_EQUAL(Address, Expected, Actual, gracefulNoSuchMember);
         break;
      default:
         COLASSERT(false);
   }

   NOBT_ASSERT_PROPERTY_EQUAL(Address, Expected, Actual, size);
   int Size = Actual.size();
   for (int i = 0; i < Size; i++){
      NODaddress ChildAddress = Address;
      ChildAddress.pushAddressIndex(i+1);
      NOBTassertTreesEqual(ChildAddress, Expected.child(i), Actual.child(i));
   }
}

static void NOBTcopyAndCompare(COLref<NODplace>& pRoot){
   COLref<NODplace> pCopied = NOBcopy(*pRoot);

   NODaddress RootAddress;
   RootAddress.setParent(pCopied.get());
   NOBTassertTreesEqual(RootAddress, *pRoot, *pCopied);
}

static void NOBTreconstructAndCompare(COLref<NODplace>& pRoot){
   COLstring Buffer;
   NOBserialize(*pRoot, Buffer);
   TST_ASSERT(Buffer.size() > 0);
   COLref<NODplace> pReconstructed = NOBparse(Buffer);

   NODaddress RootAddress;
   RootAddress.setParent(pReconstructed.get());
   NOBTassertTreesEqual(RootAddress, *pRoot, *pReconstructed);
}

static void NOBTreadFile(COLstring& FileContents, const COLstring& FileName){
   FILreadFile(FileName, &FileContents);
}

static void NOBTtestDbEmptyResultSet(){
   DBresultSetPtr pRawResultSet = new DBresultSet();

   COLref<NODplace> pResultSet = new NODBresultSet(pRawResultSet.get());
   TST_ASSERT_NO_EXCEPTION(NOBTcopyAndCompare(pResultSet));
   TST_ASSERT_NO_EXCEPTION(NOBTreconstructAndCompare(pResultSet));
}

static void NOBTtestDbNoRowResultSet(){
   DBresultSetPtr pRawResultSet = new DBresultSet();
   pRawResultSet->addColumn("StringColumn", DB_STRING);
   pRawResultSet->addColumn("IntColumn", DB_INTEGER);
   pRawResultSet->addColumn("DoubleColumn", DB_DOUBLE);
   pRawResultSet->addColumn("DateTimeColumn", DB_DATETIME);

   COLref<NODplace> pResultSet = new NODBresultSet(pRawResultSet.get());
   TST_ASSERT_NO_EXCEPTION(NOBTcopyAndCompare(pResultSet));
   TST_ASSERT_NO_EXCEPTION(NOBTreconstructAndCompare(pResultSet));
}

static void NOBTtestDbNormalResultSet(){
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

   COLref<NODplace> pResultSet = new NODBresultSet(pRawResultSet.get());
   TST_ASSERT_NO_EXCEPTION(NOBTcopyAndCompare(pResultSet));
   TST_ASSERT_NO_EXCEPTION(NOBTreconstructAndCompare(pResultSet));
}

TSTtestable* NOBTtest()
{
   TST_START_COLLECTION("NOBTtest")
      TST_ADD_TEST_FUNCTION(NOBTtestDbEmptyResultSet);
      TST_ADD_TEST_FUNCTION(NOBTtestDbNoRowResultSet);
      TST_ADD_TEST_FUNCTION(NOBTtestDbNormalResultSet);
   TST_END_FIXTURE
}
