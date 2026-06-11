//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testIp
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Tue 11 Nov 2014 14:03:22 EST
//---------------------------------------------------------------------------

#include "testIp.h"

#include <UNIT/UNITapp.h>

#include <SCK/SCKutils.h>
#include <SCK/SCKaddress.h>
#include <SCK/SCKloop.h>
#include <SCK/SCKnameResolver.h>
#include <SCK/SCKlistener.h>

#include <COL/COLclosure.h>
#include <COL/COLthreadPool.h>
#include <COL/COLarray.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static void testBadHostResolve(){
   COL_FUNCTION(testBadHostResolve);
   SCKaddress BadHostIp;
   bool Success = SCKhostResolve("badhost", &BadHostIp);
   COL_VAR(BadHostIp);
   UNIT_ASSERT(!Success);
}

static void testGoogleNameResolve(){
   COL_FUNCTION(testGoogleNameResolve);
   SCKaddress GoogleIp;
   bool Success = SCKhostResolve("www.google.com", &GoogleIp);
   COL_VAR(GoogleIp);
   UNIT_ASSERT(Success);
}

// Not really a rigorous test but at least we can asset it doesn't give an access violation.
// With tracing one can understand the library though so it's useful.
static void testLocalIp(){
   COL_FUNCTION(testLocalIp);
   COLstring LocalIp = SCKlocalIpAsString();
   COL_VAR(LocalIp);
   UNIT_ASSERT(!LocalIp.is_null());
}

static void testGetLocalIp(){
   COL_FUNCTION(testGetLocalIp);
   COLstring LocalHostName = SCKlocalHostName();
   COLarray<SCKaddress> Out;
   SCKhostResolveAllIps(LocalHostName, &Out);
   COL_VAR(Out.size());
   for (COLarray<SCKaddress>::iterator i=Out.begin(); i != Out.end(); ++i){
      COL_VAR(*i);
   }
}

// End of non rigorous tests.

static void testIpUtils4(){
   SCKaddress TestIpV4Address;
   COLstring V4IpString("192.168.0.78");
   // ExpectedV4Ip is not being used to assert... not sure if this needs to be fixed up
   unsigned char ExpectedV4Ip[16] = {0,0,0,0,0,0,0,0,0,0,0xFF,0xFF,192,168,0,78};
   UNIT_ASSERT( TestIpV4Address.fromString(V4IpString) );
   UNIT_ASSERT_EQUALS(V4IpString,TestIpV4Address.toString() );
}

static void testIpUtils6(){
   SCKaddress TestIpV6Address;
   COLstring V6IpString("3ffe:1900:4545:3:200:f8ff:fe21:67cf");
   // ExpectedV6Ip is not being used to assert... not sure if this needs to be fixed up
   char ExpectedV6Ip[16] = {'\x3f','\xfe','\x19','\x00','\x45','\x45','\x00','\x03','\x02','\x00','\xf8','\xff','\xfe','\x21','\x67','\xcf'};
   if (SCKsocketIpV6Supported()){
      UNIT_ASSERT( TestIpV6Address.fromString(V6IpString) );
      UNIT_ASSERT_EQUALS( V6IpString, TestIpV6Address.toString() );

      SCKaddress AnyAddress;
      AnyAddress.setIpv6(true);
      AnyAddress.setPort(8888);
      UNIT_ASSERT_EQUALS("<any>", AnyAddress.toString());
   } else {
      UNIT_ASSERT( !TestIpV6Address.fromString(V6IpString) );
   }
}

static void testSCKaddress(){
   COL_FUNCTION(testSCKaddress);
   SCKaddress Address;
   UNIT_ASSERT_EQUALS(0,Address.port());
   COL_VAR(Address);
   Address.setPort(1000);
   UNIT_ASSERT_EQUALS(1000, Address.port());
   COL_VAR(Address);
   Address.fromString("127.0.0.1");
   UNIT_ASSERT_EQUALS("127.0.0.1", Address.toString());
   UNIT_ASSERT_EQUALS(1000, Address.port());  // port should not be cleared by using fromString.
}

static void testFromString(){
   COL_FUNCTION(testFromString);
   SCKaddress Address1;
   UNIT_ASSERT_EQUALS(true, Address1.fromString("192.168.0.1"));
   UNIT_ASSERT_EQUALS(false,Address1.isIpv6());
   COL_VAR(Address1);
   if (SCKsocketIpV6Supported()){
      UNIT_ASSERT_EQUALS("192.168.0.1", Address1.toString());
      UNIT_ASSERT_EQUALS(true, Address1.fromString("2001:cdba:0000:0000:0000:0000:3257:9652"));
      UNIT_ASSERT_EQUALS(true, Address1.isIpv6());
      UNIT_ASSERT_EQUALS("2001:cdba::3257:9652", Address1.toString());
   }
}

static void OnResolveDone(const SCKaddress& Address, SCKloop* pSelect, int* pCount){
   COL_FUNCTION(OnResolveDone);
   UNIT_ASSERT_EQUALS(1000, Address.port());
   UNIT_ASSERT_EQUALS("localhost", Address.hostName());
   UNIT_ASSERT_EQUALS("127.0.0.1", Address.toString());
   UNIT_ASSERT_EQUALS(true, Address.set());
   (*pCount)++;
   COL_VAR2(Address.hostName(), *pCount);
}

static void DontCallMe(const SCKaddress& Address){
   COL_FUNCTION(DontCallMe);
   COLASSERT(1==0);
}

static void BadRequest(const SCKaddress& Address, SCKloop* pSelect, int* pCount){
   COL_FUNCTION(BadRequest);
   UNIT_ASSERT_EQUALS(false, Address.set());
   COL_VAR(Address.hostName());
   (*pCount)++;
}

static void CloseLoopOnRequestFinish(SCKloop* pLoop){
   COL_FUNCTION(CloseLoopOnRequestFinish);
   static int count = 0;
   count++;
   COL_VAR(count);
   if (count == 3){
      COL_TRC("3 resolves have come back, ending loop");
      pLoop->shutdown();
   }
}

static void testAsyncNameResolve(){
   COL_FUNCTION(testAsyncNameResolve);
   SCKaddress Address;
   Address.setPort(1000);
   int Count = 0;
   bool CouldResolve = Address.fromString("localhost");
   COL_VAR(Address.hostName());
   UNIT_ASSERT_EQUALS(false, CouldResolve);
   UNIT_ASSERT_EQUALS(1000, Address.port());
   UNIT_ASSERT_EQUALS("localhost", Address.hostName());
   COL_VAR(Address.hostName());
   COL_TRC("Passed initial tests.");
   COLthreadPool Pool(1,10);
   Pool.start();
   SCKloop Loop(&Pool);
   SCKnameResolver NameResolver(&Pool, &Loop);
   NameResolver.setOnRequestReturn(COLnewClosure0(&CloseLoopOnRequestFinish, &Loop));
   COL_VAR(Address.hostName());
   NameResolver.resolveStart(Address, COLnewClosure1(&OnResolveDone, &Loop, &Count));
   SCKaddress Address2;
   Address2.fromString("life");
   int CancelRequest = NameResolver.resolveStart(Address2, COLnewClosure1(&DontCallMe));
   NameResolver.resolveCancel(CancelRequest);
   SCKaddress Address3;
   Address3.fromString("badhost");
   NameResolver.resolveStart(Address3, COLnewClosure1(&BadRequest, &Loop, &Count));
   Loop.start();
   UNIT_ASSERT_EQUALS(2, Count);
}

static void testSCKhostResolveAllIps(){
   COL_FUNCTION(testSCKhostResolveAllIps);
   COLarray<SCKaddress> AddressVector;
   SCKhostResolveAllIps("www.yahoo.com", &AddressVector);
   for (int i=0; i< AddressVector.size(); i++){
      COL_VAR2(i, AddressVector[i]);
   }
   UNIT_ASSERT(AddressVector.size() > 2);
}

static void testFromString2(){
   COL_FUNCTION(testFromString2);
   SCKaddress Address;
   Address.fromString("127.0.0.1");
   Address.setPort(4000);
   COLstring Out;
   COLostream Stream(Out);
   Stream << Address;
   COL_VAR(Out);
   UNIT_ASSERT_EQUALS("127.0.0.1:4000", Out);
}

static void testIpv6(){
   COL_FUNCTION(testIpv6);
   if (!SCKsocketIpV6Supported()){
      return;
   }
   SCKaddress Address;
   Address.setIpv6(true);
   Address.fromString("::1");
   COLstring StringAddress = Address.toString();
   UNIT_ASSERT_EQUALS("::1", StringAddress);
}

void testIp(UNITapp* pApp){
   pApp->add("ip/badHost", &testBadHostResolve);
   pApp->add("ip/googleNameResolve", &testGoogleNameResolve);
      // Commmented out this test since it can fail on my mac - Eliot
      //UNIT_ADD_TEST_FUNCTION(testLocalIp)
      //UNIT_ADD_TEST_FUNCTION(testGetLocalIp)
   pApp->add("ip/utils4", &testIpUtils4);
   pApp->add("ip/utils6", &testIpUtils6);
   pApp->add("ip/sckAddress", &testSCKaddress);
   pApp->add("ip/fromString", &testFromString);
   pApp->add("ip/asyncNameResolve", &testAsyncNameResolve);
      //UNIT_ADD_TEST_FUNCTION(testSCKhostResolveAllIps);  // This test isn't reliable.
   pApp->add("ip/fromString2", &testFromString2);
   pApp->add("ip/ipv6", &testIpv6);

}

