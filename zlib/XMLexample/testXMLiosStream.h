#ifndef __TEST_XMLIOS_STREAM_H__
#define __TEST_XMLIOS_STREAM_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2006 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testXMLiosStream
//
// Description: Unittest
//
// Author: Nasron Cheong
// Date:   Wed 05/01/2002 
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

class TSTtestable;

class testXMLiosStream
{
public:
   testXMLiosStream(){};
   virtual ~testXMLiosStream(){};

   static testXMLiosStream* create(){return new testXMLiosStream; }

   void basicExample();
   void testRangeEscape();
   void testAttributeEscapes();
   void testUnescapedData();

   //these are functions that are added to the framework
   static TSTtestable* getTests();

private:
};

#endif // end of defensive include
