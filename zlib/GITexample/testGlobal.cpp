// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testGlobal
//
// Description:
//
// Implementation of tests
//
// Author: Eliot Muir 
// Date:   Saturday 25 March 2023 - 11:59AM
// ---------------------------------------------------------------------------
#include "testGlobal.h"

#include <COL/COLlog.h>
#include <GITU/GITUconfigGlobal.h>
#include <INI/INIdata.h>
#include <UNIT/UNITapp.h>
COL_LOG_MODULE;

static void testPublicPrivateKey() {
	COL_FUNCTION(testPublicPrivateKey);
	const COLstring PrivateKey = "private_key";
	UNIT_ASSERT_EQUALS("private_key.pub", GITUconfigPublicKey(PrivateKey));
	const COLstring PrivateKeyWithQuotes = "\"private_key_withSpaceInPath\"";
	UNIT_ASSERT_EQUALS("\"private_key_withSpaceInPath.pub\"", GITUconfigPublicKey(PrivateKeyWithQuotes));
}

static void testSimple(){
   COL_FUNCTION(testSimple);
   INIdata Config;
   GITUconfigGlobalLoad(&Config);
   COLstring UserName = GITUconfigUserName(Config);
   COLstring Email    = GITUconfigUserEmail   (Config);
   GITUconfigGlobalSave(Config); // just to make sure this works
   
   COL_VAR2(UserName, Email);
}

void testGlobal(UNITapp* pApp) {
//   pApp->add("global/credentials/read", &testSimple);
	pApp->add("global/keys", &testPublicPrivateKey);
}
