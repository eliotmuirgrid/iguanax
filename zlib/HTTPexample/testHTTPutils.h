#ifndef __TEST_HTTPUTILS_H__
#define __TEST_HTTPUTILS_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testHTTPutils.h
//
// Description:
//
// A set of tests of functions in the HTTP library.
//
// Author: Eliot Muir
// Date:   Tuesday, December 12th, 2006 @ 02:40:10 PM
//
//---------------------------------------------------------------------------

class TSTtestable;

class testHTTPutilsPrivate;

class testHTTPutils
{
public:
   testHTTPutils()  {}
   virtual ~testHTTPutils() {}

   // create a pointer to a new test object
   static testHTTPutils* create(){ return new testHTTPutils; }

   void testHTTPsession();

   void testCookies();
   void testRequestParsing();
   void testHeaderParsing();
   void testHeaderParsingErrorHandling();
   void testBodyParsing();

   void testResponseObject();
   void testRequestObject();

   void testResponseUtils();

   void testBadParse();
   void testBadResponseParse();
   void testRequestNoHeaders();

   static TSTtestable* testCollection();

private:
   testHTTPutils(const testHTTPutils& Orig);  
   testHTTPutils& operator=(const testHTTPutils& Orig);

};

#endif // end of defensive include
