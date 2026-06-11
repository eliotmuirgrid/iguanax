//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testSslContext
//
// Description:
//
// Implementation
//
// Author: John Q
// Date:   Tue 25 Oct 2022 15:01:10 EDT
//---------------------------------------------------------------------------

#include <COL/COLlog.h>
#include <SCKB/SCKBsslContext.h>
#include <TST/TSThelpers.h>
#include <TST/TSTtestFixture.h>
#include <TST/TSTtestFunction.h>
COL_LOG_MODULE;

static void testBasic() {
   COL_FUNCTION(testBasic);
   SCKBsslContext Ctx;
   TST_ASSERT( Ctx.setMinVersion(TLS1_VERSION));
   TST_ASSERT( Ctx.setMaxVersion(TLS1_2_VERSION));
   TST_ASSERT( Ctx.setCipherList("ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-RSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES256-GCM-SHA384:ECDHE-RSA-AES256-GCM-SHA384:ECDHE-ECDSA-CHACHA20-POLY1305:ECDHE-RSA-CHACHA20-POLY1305:DHE-RSA-AES128-GCM-SHA256:DHE-RSA-AES256-GCM-SHA384;"));
   TST_ASSERT( Ctx.setCaLocations("localhost_crt.pem"));
   TST_ASSERT( Ctx.setCaLocations("", "."));
   TST_ASSERT(!Ctx.setCaLocations("nofile"));
   TST_ASSERT(!Ctx.setCaLocations("main.cpp"));
   TST_ASSERT( Ctx.setCertificate("localhost_crt.pem"));
   TST_ASSERT(!Ctx.setCertificate("nofile"));
   TST_ASSERT(!Ctx.setCertificate("main.cpp"));
   TST_ASSERT( Ctx.setPrivateKey("localhost_key.pem"));
   TST_ASSERT( Ctx.setPrivateKey("localhost_key.pem", 1, "password"));
   TST_ASSERT(!Ctx.setPrivateKey("localhost_key.pem", 2, "password"));
}

TSTtestable* testSslContext() {
   TST_START_COLLECTION("SslContextTests")
      TST_ADD_TEST_FUNCTION(testBasic);
   TST_END_FIXTURE
}
