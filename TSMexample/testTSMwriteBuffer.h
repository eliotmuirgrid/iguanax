#ifndef __TEST_TSMWRITE_BUFFER_H__
#define __TEST_TSMWRITE_BUFFER_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2007 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testTSMwriteBuffer.h
//
// Description:
//
// A set of tests of functions in the TSM library.
//
// Author: Matt Lawrence
// Date:   Tuesday, February 27th, 2007 @ 08:50:35 AM
//
//---------------------------------------------------------------------------

class TSTtestable;

class testTSMwriteBuffer
{
public:
   testTSMwriteBuffer()  {}
   virtual ~testTSMwriteBuffer() {}

   // create a pointer to a new test object
   static testTSMwriteBuffer* create(){ return new testTSMwriteBuffer; }

   void testWriteAndFlush();   
   void testAutoFlush();
   void testFlushOnDestruction();

   void testOverflowHandling();
   void testWriteGrouping();

   static TSTtestable* testCollection();

private:
   testTSMwriteBuffer(const testTSMwriteBuffer& Orig);  
   testTSMwriteBuffer& operator=(const testTSMwriteBuffer& Orig);

};

#endif // end of defensive include
