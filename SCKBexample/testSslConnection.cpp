//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testSslConnection
//
// Description:
//
// Implementation
//
// Author: John Q
// Date:   Tue 25 Oct 2022 15:01:10 EDT
//---------------------------------------------------------------------------

#include <COL/COLdateTime.h>
#include <COL/COLlog.h>
#include <SCK/SCKsystem.h>
#include <SCK/SCKerror.h>  // depends on errno.h in SCKsystem.h
#include <SCK/SCKutils.h>
#include <SCKB/SCKBsslConnection.h>
#include <SCKB/SCKBsslContext.h>
#include <TST/TSThelpers.h>
#include <TST/TSTtestFixture.h>
#include <TST/TSTtestFunction.h>
COL_LOG_MODULE;

static void testBasic() {
   COL_FUNCTION(testBasic);
   SCKBsslContext Ctx;
   SCKBsslConnection Client(&Ctx);
   TST_ASSERT(Client.connect("www.interfaceware.com", 443));
   COLstring Buffer = "GET / HTTP/1.1\r\nHost: www.interfaceware.com\r\n\r\n";
   TST_ASSERT(Client.write(Buffer) > 0);
   Buffer.setCapacity(1024);
   Buffer.clear();
   TST_ASSERT(Client.read(&Buffer) > 0)
   TST_ASSERT(Buffer.size());
   COL_VAR(Buffer);
   Client.close();
}

static void testVerifyPeer() {
   COL_FUNCTION(testVerifyPeer);
   SCKBsslContext Ctx;
   Ctx.setCaLocations("ca-bundle.crt");
   Ctx.setVerifyPeer(true);
   SCKBsslConnection Client(&Ctx);
   TST_ASSERT(Client.connect("www.interfaceware.com", 443));
   COLstring Buffer = "GET / HTTP/1.1\r\nHost: www.interfaceware.com\r\n\r\n";
   TST_ASSERT(Client.write(Buffer) > 0);
   Buffer.setCapacity(1024);
   Buffer.clear();
   TST_ASSERT(Client.read(&Buffer) > 0)
   TST_ASSERT(Buffer.size());
   COL_VAR(Buffer);
   Client.close();
}

static void testResolveFail() {
   COL_FUNCTION(testResolveFail);
   SCKBsslContext Ctx;
   SCKBsslConnection Client(&Ctx);
   TST_ASSERT(!Client.connect("ifw", 443));
   TST_ASSERT_EQUALS(SCK_DNSFAILURE, Client.lastError());
}

static void testVerifyPeerFail() {
   COL_FUNCTION(testVerifyPeerFail);
   SCKBsslContext Ctx;
   // Don't set CA to trigger verify peer failure
   Ctx.setVerifyPeer(true);
   SCKBsslConnection Client(&Ctx);
   TST_ASSERT(!Client.connect("www.interfaceware.com", 443));
   TST_ASSERT_EQUALS(SCK_ESSLVERIFY, Client.lastError());
}

// set up testConnectTimeout
static int bindWithoutListen(int* pListener) {
   COL_FUNCTION(bindWithoutListen);
   int Handle = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
   COLPRECONDITION(Handle >= 0);
   sockaddr_in Address     = {0};
   Address.sin_family      = AF_INET;
   Address.sin_port        = htons(0);
   Address.sin_addr.s_addr = INADDR_ANY;
   SCKsocketLength AddrLen = sizeof(Address);
   COLPRECONDITION(       ::bind(Handle, (sockaddr*)&Address,  AddrLen) == 0);
   COLPRECONDITION(::getsockname(Handle, (sockaddr*)&Address, &AddrLen) == 0);
   *pListener = Handle;
   return ntohs(Address.sin_port);
}

static void testConnectTimeout() {
   COL_FUNCTION(testConnectTimeout);
   int Server;
   int SvrPort = bindWithoutListen(&Server);
   SCKBsslContext Ctx;
   SCKBsslConnection Client(&Ctx);
   Client.setTimeout(200); // ms
   COLtimeStamp Start, End;
   COLcurrentTimeStamp(&Start);
   TST_ASSERT(!Client.connect("127.0.0.1", SvrPort));
   COLcurrentTimeStamp(&End);
   SCKcloseSocket(Server);
   TST_ASSERT_EQUALS(SCK_ETIMEDOUT, Client.lastError());
   TST_ASSERT(COLtimeStampDiffInMillisecond(End, Start) < 400);
}

static void testReadTimeout() {
   COL_FUNCTION(testReadTimeout);
   SCKBsslContext Ctx;
   SCKBsslConnection Client(&Ctx);
   Client.setTimeout(200); // ms
   TST_ASSERT(Client.connect("www.interfaceware.com", 443));
   COLstring Buffer;
   COLtimeStamp Start, End;
   COLcurrentTimeStamp(&Start);
   TST_ASSERT(Client.read(&Buffer) < 0);
   COLcurrentTimeStamp(&End);
   TST_ASSERT_EQUALS(SCK_ETIMEDOUT, Client.lastError());
   TST_ASSERT(COLtimeStampDiffInMillisecond(End, Start) < 400);
}

static void testReadWriteError() {
   COL_FUNCTION(testReadWriteError);
   SCKBsslContext Ctx;
   SCKBsslConnection Client(&Ctx);
   Client.setTimeout(200); // ms
   TST_ASSERT(Client.connect("www.interfaceware.com", 443));
   Client.close();
   COLstring Buffer;
   TST_ASSERT(Client.read(&Buffer) < 0);
   TST_ASSERT_EQUALS(SCK_EBADF, Client.lastError());
   TST_ASSERT(Client.write("hello") < 0);
   TST_ASSERT_EQUALS(SCK_EBADF, Client.lastError());
}

TSTtestable* testSslConnection() {
   TST_START_COLLECTION("SslConnectionTests")
      TST_ADD_TEST_FUNCTION(testBasic);
      TST_ADD_TEST_FUNCTION(testVerifyPeer);
      TST_ADD_TEST_FUNCTION(testResolveFail);
      TST_ADD_TEST_FUNCTION(testVerifyPeerFail);
#ifndef __linux__
      // Linux connect returns either ECONNREFUSED or success
      TST_ADD_TEST_FUNCTION(testConnectTimeout);
#endif
      TST_ADD_TEST_FUNCTION(testReadTimeout);
      TST_ADD_TEST_FUNCTION(testReadWriteError);
   TST_END_FIXTURE
}
