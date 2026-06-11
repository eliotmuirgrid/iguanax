//---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testKnownHosts
//
// Description:
//
// Implmentation
//
// Author: Eliot Muir
// Date:  Mon 16 Jan 2023 11:18:36 EST
//---------------------------------------------------------------------------
#include "testKnownHosts.h"

#include <COL/COLlog.h>
#include <GITU/GITUknownHost.h>
#include <UNIT/UNITapp.h>
COL_LOG_MODULE;

static void testGetHost() {
   COL_FUNCTION(testGetHost);
   COLstring Key, Error;
   bool Success = GITUgetKnownHostKey("bitbucket.org", &Key, &Error);
   COL_VAR3(Success, Key, Error);
}

static void testGetHostBad() {
   COL_FUNCTION(testGetHostBad);
   COLstring Key, Error;
   bool Success = GITUgetKnownHostKey("bitbucket.orgg", &Key, &Error);
   COL_VAR3(Success, Key, Error);
}

void testKnownHosts(UNITapp* pApp){
   COL_FUNCTION(testKnownHosts);
   pApp->add("test/known/hosts", &testGetHost);
   pApp->add("test/known/hosts/bad", &testGetHostBad);
}
