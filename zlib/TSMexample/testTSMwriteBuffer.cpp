//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testTSMwriteBuffer
//
// Description:
//
// Implementation
//
// Author: Matt Lawrence
// Date:   Tuesday, February 27th, 2007 @ 08:50:35 AM
//
//---------------------------------------------------------------------------
#include "TSMexamplePrecomp.h"
#pragma hdrstop

#include "testTSMwriteBuffer.h"

#include <TSM/TSMwriteBuffer.h>
#include <COL/COLsinkString.h>
#include <COL/COLfilter.h>

#define TSM_WRITE_BUFFER_MAX_SIZE 30000

class TestSink : public COLfilter
{
public:
   TestSink(COLsink& Next)
      : COLfilter(&Next),
        CountOfFlush(0),
        CountOfWrite(0)
   {
   }

   COLuint32 CountOfFlush;
   COLuint32 CountOfWrite;
     
   virtual COLuint32 write(const void* cpBuffer, COLuint32 SizeOfBuffer)
   {
      CountOfWrite++;
      return next().write(cpBuffer, SizeOfBuffer);
   }


   virtual void resetFilter() {}

   virtual void flush()
   {
      CountOfFlush++;
      next().flush();
   }
};

void testTSMwriteBuffer::testWriteAndFlush()
{
   COLstring Output;
   COLsinkString Sink(Output);
   TestSink Intermediate(Sink);
   TSMwriteBuffer Buffer(Intermediate);

   Buffer.write("whee!", 5);
   TST_ASSERT_EQUALS("", Output);
   Buffer.write(" test!", 5);
   TST_ASSERT_EQUALS("", Output);
   Buffer.flush();

   TST_ASSERT_EQUALS("whee! test", Output);
   TST_ASSERT_EQUALS(1, Intermediate.CountOfWrite);
   TST_ASSERT_EQUALS(1, Intermediate.CountOfFlush);
}

void testTSMwriteBuffer::testAutoFlush()
{
   COLstring Data(TSM_WRITE_BUFFER_MAX_SIZE * 3 / 4, '0');
   COLstring Output;
   COLsinkString Sink(Output);
   TestSink Intermediate(Sink);
   TSMwriteBuffer Buffer(Intermediate);

   Buffer.write(Data.c_str(), Data.size());
   TST_ASSERT("" == Output);
   Buffer.write(Data.c_str(), Data.size());
   TST_ASSERT(COLstring(TSM_WRITE_BUFFER_MAX_SIZE, '0') == Output);
   TST_ASSERT_EQUALS(1, Intermediate.CountOfFlush);
   TST_ASSERT_EQUALS(1, Intermediate.CountOfWrite);
   Buffer.flush();
   TST_ASSERT(Data + Data == Output);
}

void testTSMwriteBuffer::testFlushOnDestruction()
{
   COLstring Output;
   COLsinkString Sink(Output);
   TestSink Intermediate(Sink);
   {
      TSMwriteBuffer Buffer(Intermediate);
      Buffer.write("whee!", 5);
      TST_ASSERT_EQUALS("", Output);
      TST_ASSERT_EQUALS(0, Intermediate.CountOfFlush);
      TST_ASSERT_EQUALS(0, Intermediate.CountOfWrite);
   }
   TST_ASSERT_EQUALS(1, Intermediate.CountOfFlush);
   TST_ASSERT_EQUALS(1, Intermediate.CountOfWrite);
   TST_ASSERT_EQUALS("whee!", Output);
}

void testTSMwriteBuffer::testOverflowHandling()
{
   COLstring Data(TSM_WRITE_BUFFER_MAX_SIZE / 2, '0');
   Data += COLstring(TSM_WRITE_BUFFER_MAX_SIZE / 2, '1');
   Data += "foo";
   COLstring Output;
   COLsinkString Sink(Output);
   TSMwriteBuffer Buffer(Sink);   

   TST_ASSERT_EQUALS(Data.size(), Buffer.write(Data.c_str(), Data.size()));
   Buffer.flush();
   
   TST_ASSERT_EQUALS(Data.size(), Output.size());
   TST_ASSERT(Data == Output);
}

void testTSMwriteBuffer::testWriteGrouping()
{
   COLstring HalfData(TSM_WRITE_BUFFER_MAX_SIZE / 2, '0');
   COLstring FullData(TSM_WRITE_BUFFER_MAX_SIZE, '1');
   COLstring Output;
   COLsinkString Sink(Output);
   TestSink Intermediate(Sink);
   TSMwriteBuffer Buffer(Intermediate);

   Buffer.write(HalfData.c_str(), HalfData.size());
   Buffer.write(FullData.c_str(), FullData.size());
   Buffer.write(HalfData.c_str(), HalfData.size());
   Buffer.flush();

   TST_ASSERT(HalfData + FullData + HalfData == Output);
   TST_ASSERT_EQUALS(2, Intermediate.CountOfWrite);   
}

TSTtestable* testTSMwriteBuffer::testCollection()
{
   TST_START_FIXTURE(testTSMwriteBuffer)
      TST_ADD_TEST_METHOD(testWriteAndFlush);      
      TST_ADD_TEST_METHOD(testAutoFlush);
      TST_ADD_TEST_METHOD(testFlushOnDestruction);
      
      TST_ADD_TEST_METHOD(testOverflowHandling);
      TST_ADD_TEST_METHOD(testWriteGrouping);
   TST_END_FIXTURE
}
;

