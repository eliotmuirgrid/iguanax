// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testVarUtils
//
// Description:
//
// Implementation of tests
//
// Author: Eliot Muir 
// Date:   Wednesday 29 March 2023 - 10:13AM
// ---------------------------------------------------------------------------
#include "testVarUtils.h"

#include <UNIT/UNITapp.h>

#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

static void testPathExists(){
   COL_FUNCTION(testPathExists);
   COLvar Data;
   Data["this"]["path"]["exists"] = "life";
   Data["this"]["array"][0] = 111;
   Data["this"]["array"][1] = 112;
   Data[0]["array"][0] = 111;
   UNIT_ASSERT(COLvarPathExists(Data, "this/path/exists"));
   UNIT_ASSERT(COLvarPathExists(Data, "this/path"));
   UNIT_ASSERT(COLvarPathExists(Data, "this"));
   UNIT_ASSERT(!COLvarPathExists(Data, "this/path/exists/meh"));
   UNIT_ASSERT(!COLvarPathExists(Data, "this/path/exists2/meh"));
   UNIT_ASSERT(!COLvarPathExists(Data, "this/path2"));
   UNIT_ASSERT(!COLvarPathExists(Data, "this/path2/exists"));
   UNIT_ASSERT(!COLvarPathExists(Data, "this2"));
   UNIT_ASSERT(!COLvarPathExists(Data, "this/array/life"));
   UNIT_ASSERT( COLvarPathExists(Data, "this/array/0"));
   UNIT_ASSERT( COLvarPathExists(Data, "this/array/1"));
   UNIT_ASSERT(!COLvarPathExists(Data, "this/array/0/1"));
   UNIT_ASSERT(!COLvarPathExists(Data, "this/array/life/not/array"));
   UNIT_ASSERT( COLvarPathExists(Data, "0/array/0"));
}

static void testBigInt(){
   COL_FUNCTION(testBigInt);
   long long Answer = 12345678910;  
   COLvar Store = Answer;
   UNIT_ASSERT_EQUALS(Answer, Store.asInt());
   COLvar BigInt("12345678910");
   UNIT_ASSERT_EQUALS(Answer, BigInt.asInt());
}

void testVarUtils(UNITapp* pApp) {
   pApp->add("var/path/exists", &testPathExists);
   pApp->add("var/bigint", &testBigInt);
}
