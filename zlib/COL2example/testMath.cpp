// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testMath
//
// Description:
//
// Implementation of tests
//
// Author: Tyler Brown 
// Date:   Thursday 30 March 2023 - 06:34PM
// ---------------------------------------------------------------------------
#include "testMath.h"

#include <UNIT/UNITapp.h>

#include <COL/COLstring.h>
#include <COL/COLmath.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static void testIsInteger(){
   COL_FUNCTION(testIsInteger);
   UNIT_ASSERT( COLmath::isInteger("123450"));
   UNIT_ASSERT( COLmath::isInteger("1"));
   UNIT_ASSERT( COLmath::isInteger("-123"));
   UNIT_ASSERT(!COLmath::isInteger("002340"));
   UNIT_ASSERT(!COLmath::isInteger("00000"));
   UNIT_ASSERT(!COLmath::isInteger("123450abc123"));
   UNIT_ASSERT(!COLmath::isInteger("Some string with random garbage"));
   
   UNIT_ASSERT( COLmath::isInteger(COLstring("123450")));
   UNIT_ASSERT( COLmath::isInteger(COLstring("1")));
   UNIT_ASSERT( COLmath::isInteger(COLstring("-123")));
   UNIT_ASSERT(!COLmath::isInteger(COLstring("002340")));
   UNIT_ASSERT(!COLmath::isInteger(COLstring("00000")));
   UNIT_ASSERT(!COLmath::isInteger(COLstring("123450abc123")));
   UNIT_ASSERT(!COLmath::isInteger(COLstring("Some string with random garbage")));
}

void testMath(UNITapp* pApp) {
   pApp->add("math/isInteger/string", &testIsInteger);
}
