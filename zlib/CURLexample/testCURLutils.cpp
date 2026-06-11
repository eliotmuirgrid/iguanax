//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testCURLutils
//
// Description:
//
// Implementation
//
// Author: Yuriy Kachanov
// Date:   Tuesday, October 3rd, 2006 @ 02:50:47 PM
//
//---------------------------------------------------------------------------
#include <CURLexample/testCURLutils.h>
#include <CURL/CURLclient.h>
#include <CURL/CURLutils.h>
#include <CURL/CURLftp.h>
#include <TST/TSThelpers.h>
#include <TST/TSTtestFixture.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

// test of the old interface which is used by ifware_register
void testCURLutils::testHttpClient() {
   CURLclient Client;
   COLstring Response;
   COLstring Request("http://example.com");

   COLuint32 HttpResponseCode = Client.sendRequest(Request, Response);

   COL_TRC("Here is example.com's HTML via HTTP:" << newline << Response);

   TST_ASSERT_MESSAGE(HttpResponseCode == CURLclient::HttpStatusOk, "HTTP client test failed");

   COLstring SecureRequest("https://www.example.com/");

   HttpResponseCode = Client.sendRequest(SecureRequest, Response);
   Response.zero();

   COL_TRC("Here is example.com's HTML via HTTPS:" << newline << Response);

   TST_ASSERT_MESSAGE(HttpResponseCode == CURLclient::HttpStatusOk, "HTTP client test failed");

}

void testCURLutils::testHttpGetRequest() {
   CURLclient Client;
   Client.setUrl("http://www.google.ca/search?hl=en&q=iNTERFACEWARE&btnG=Google+Search&meta=");
   COLuint32 HttpResponseCode = Client.requestGet();

   COL_TRC("Here is header via HTTP:" << newline << Client.responseHeader());
   COL_TRC("Here is content via HTTP:" << newline << Client.responseContent());

   TST_ASSERT_MESSAGE(HttpResponseCode == CURLclient::HttpStatusOk, "HTTP client test failed");


   Client.setUrl("https://www.google.ca/search?hl=en&q=iNTERFACEWARE&btnG=Google+Search&meta=");
   HttpResponseCode = Client.requestGet();

   COL_TRC("Here is header via HTTPS:" << newline << Client.responseHeader());
   COL_TRC("Here is content via HTTPS:" << newline << Client.responseContent());

   TST_ASSERT_MESSAGE(HttpResponseCode == CURLclient::HttpStatusOk, "HTTP client test failed");
}

// See #10365, testing to see if curllib's strerror() crashes and burns
void testCURLutils::testErrorImplementation() {
   CURLclient Client;

   try {
      COLstring Dummy;
      Client.sendRequest("http://localhost:1/", Dummy);
   }
   catch (const COLerror& e) {
      // Don't care about the content of the message, just that it didn't die
      COL_TRC(e.description());
   }
}

// CURLftpExtractList can't handle the output of Kiteworks SFTP Server.
// make sure CURLftpExtractList stuffs the raw data in FileName instead of skipping it.
// https://fogbugzx.interfaceware.com/default.asp?36021
void testCURLutils::testFtpListParse() {
   COLarray<CURLftpListDirEntry> Result;
   COLstring TestOne =
      "dr-xr-xr-x   17 root     root          249 Mar  5 18:26 .\n"
      "dr-xr-xr-x   17 root     root          249 Mar  5 18:26 ..\n"
      "dr-xr-xr-x    5 root     root         4096 Jun  9  2020 boot\n"
      "drwxr-xr-x   19 root     root         3240 Aug  4 13:20 dev\n"
      "drwxr-xr-x    3 root     root           23 Aug 20  2018 home\n"
      "dr-xr-xr-x  248 root     root            0 Aug  3 14:00 proc\n"
      "drwxr-xr-x   42 root     root         1280 Aug  4 13:20 run\n";
   CURLftpExtractList(TestOne, Result);
   TST_ASSERT_EQUALS(7, Result.size());
   // Kiteworks SFTP Server test case
   COLstring TestTwo =
      "d---rwx---   1 6        6            4096 04 Jun 15:46 ADMA2_Inventory_Prod'\n"
      "d---rwx---   1 6        6            4096 04 Jun 15:46 ADMA2_Prod'\n"
      "d---rwx---   1 9        9            4096 12 Jul 17:20 sftp_shared'\n"
      "drwxrwx---   1 6        6            4096 10 Aug 08:00 ADMA2_Inventory_Test'\n"
      "drwxrwx---   1 6        6            4096 10 Aug 14:01 ADMA2_Test'\n";
   Result.clear();
   CURLftpExtractList(TestTwo, Result);
   TST_ASSERT_EQUALS(5, Result.size());
   TST_ASSERT_EQUALS("d---rwx---   1 6        6            4096 04 Jun 15:46 ADMA2_Inventory_Prod'", Result[0].FileName);
}

TSTtestable* testCURLutils::testCollection() {
   TST_START_FIXTURE(testCURLutils)
      TST_ADD_TEST_METHOD(testHttpClient);
      TST_ADD_TEST_METHOD(testHttpGetRequest);
      TST_ADD_TEST_METHOD(testErrorImplementation);
      TST_ADD_TEST_METHOD(testFtpListParse);
   TST_END_FIXTURE
}
