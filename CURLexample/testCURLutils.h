#ifndef __TEST_CURLUTILS_H__
#define __TEST_CURLUTILS_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testCURLutils.h
//
// Description:
//
// A set of tests of functions in the CURL library.
//
// Author: Yuriy Kachanov
// Date:   Tuesday, October 3rd, 2006 @ 02:50:47 PM
//
//---------------------------------------------------------------------------

class TSTtestable;

class testCURLutilsPrivate;

class testCURLutils
{
public:
   testCURLutils()  {}
   virtual ~testCURLutils() {}

   // create a pointer to a new test object
   static testCURLutils* create(){ return new testCURLutils; }

   void testHttpClient();
   void testHttpGetRequest();
   void testHttpPutRequest();
   void testHttpPostRequest();
   void testHttpSecureGetRequest();
   void testHttpSecurePutRequest();
   void testHttpSecurePostRequest();
   void testErrorImplementation();
   void testFtpListParse();

   static TSTtestable* testCollection();

private:
   testCURLutils(const testCURLutils& Orig);
   testCURLutils& operator=(const testCURLutils& Orig);

};

#endif // end of defensive include
