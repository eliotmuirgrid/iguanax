// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testTUNhash
//
// Description:
//
// Implementation of tests
//
// Author: Eliot Muir 
// Date:   Saturday 24 June 2023 - 11:43AM
// ---------------------------------------------------------------------------
#include "testTUNhash.h"

#include <TUN/TUNkey.h>
#include <TUN/TUNkeyLoad.h>
#include <TUN/TUNhash.h>

#include <UNIT/UNITapp.h>

#include <COL/COLhashmap.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static void testHashClash(){
   COL_FUNCTION(testHashClash);
   TUNkey Key1;
   TUNkey Key2;
   TUNkeyLoad("client/private1.pem", &Key1);
   TUNkeyLoad("client/private2.pem", &Key2);
   COLstring Hash1 = TUNhashKey(Key1);
   COLstring Hash2 = TUNhashKey(Key2);
   COL_VAR(Hash1);
   COL_VAR(Hash2);
   COLhashmap<COLstring, COLuint64> Map;
   Map.add(Hash1, 1);
   COL_VAR(Map.count(Hash1));
   COL_VAR(Map.count(Hash2));
   UNIT_ASSERT_EQUALS(1, Map.count(Hash1));
   UNIT_ASSERT_EQUALS(0, Map.count(Hash2));
}

void testTUNhash(UNITapp* pApp) {
   pApp->add("hash/clash/test", &testHashClash);
}
