//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testTSMhandlerStack
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

#include "testTSMhandlerStack.h"

#include <TSM/TSMhandlerStack.h>

void DummyHandler(TSMhandlerStack&, const TSMlabel&, const TSMattributeList&)
{
   COL_ERROR_STREAM("DummyHandler!", COLerror::ParsingError);
}

COLostream& operator<<(COLostream& Stream, void* pPtr)
{
   COLstring Temp;
   COLostream TempStream(Temp);
   TempStream << "0x" << hex << *((long*)pPtr);
   return Stream << Temp;
}

void testTSMhandlerStack::testDefaults()
{
   TSMhandlerStack Stack;

   TST_ASSERT_EQUALS(false, Stack.initialized());
   //TST_ASSERT_EXCEPTION(Stack.pop(), COLerror("Failed precondition: Initialized", COLerror::PreCondition));
   //TST_ASSERT_EXCEPTION(Stack.push(), COLerror("Failed precondition: Initialized", COLerror::PreCondition));
   //TST_ASSERT_EXCEPTION(Stack.currentObject(), COLerror("Failed precondition: Initialized", COLerror::PreCondition));
   //TST_ASSERT_EXCEPTION(Stack.currentHandler(), COLerror("Failed precondition: Initialized", COLerror::PreCondition));
   //TST_ASSERT_EXCEPTION(Stack.setCurrentHandler(NULL), COLerror("Failed precondition: Initialized", COLerror::PreCondition));
   //TST_ASSERT_EXCEPTION(Stack.setCurrentObject(NULL), COLerror("Failed precondition: Initialized", COLerror::PreCondition));
}

void testTSMhandlerStack::testInit()
{
   TSMhandlerStack Stack;

   Stack.init(&DummyHandler, &Stack);
   
   TST_ASSERT(DummyHandler == Stack.currentHandler());
   TST_ASSERT_EQUALS(&Stack, Stack.currentObject());

   //TST_ASSERT_EXCEPTION(Stack.pop(), COLerror("Failed precondition: DepthOfStack > 0", COLerror::PreCondition));

   Stack.push();
   Stack.init(&DummyHandler, &Stack);
   //TST_ASSERT_EXCEPTION(Stack.pop(), COLerror("Failed precondition: DepthOfStack > 0", COLerror::PreCondition));   
}

void testTSMhandlerStack::testSetGet()
{
   TSMhandlerStack Stack;

   Stack.init(NULL, NULL);
   Stack.setCurrentHandler(DummyHandler);
   Stack.setCurrentObject(&Stack);

   TST_ASSERT(DummyHandler == Stack.currentHandler());
   TST_ASSERT_EQUALS(&Stack, Stack.currentObject());
}

void testTSMhandlerStack::testPushPop()
{
   TSMhandlerStack Stack;

   Stack.init(DummyHandler, &Stack);

   Stack.push();
   TST_ASSERT(DummyHandler == Stack.currentHandler());
   TST_ASSERT_EQUALS(&Stack, Stack.currentObject());

   Stack.setCurrentHandler(NULL);
   Stack.setCurrentObject(NULL);
   Stack.pop();
   TST_ASSERT(DummyHandler == Stack.currentHandler());
   TST_ASSERT_EQUALS(&Stack, Stack.currentObject());
  // TST_ASSERT_EXCEPTION(Stack.pop(), COLerror("Failed precondition: DepthOfStack > 0", COLerror::PreCondition));

   Stack.push();
   Stack.setCurrentHandler(NULL);
   Stack.setCurrentObject(NULL);
   Stack.push();   
   TST_ASSERT((TSMobjectHandler)NULL == Stack.currentHandler());
   TST_ASSERT_EQUALS((void*)NULL, Stack.currentObject());
   Stack.pop();
   Stack.pop();
   TST_ASSERT(DummyHandler == Stack.currentHandler());
   TST_ASSERT_EQUALS(&Stack, Stack.currentObject());
   //TST_ASSERT_EXCEPTION(Stack.pop(), COLerror("Failed precondition: DepthOfStack > 0", COLerror::PreCondition));
}

TSTtestable* testTSMhandlerStack::testCollection()
{
   TST_START_FIXTURE(testTSMhandlerStack)
      TST_ADD_TEST_METHOD(testDefaults);            
      TST_ADD_TEST_METHOD(testInit);
      TST_ADD_TEST_METHOD(testSetGet);
      TST_ADD_TEST_METHOD(testPushPop);
   TST_END_FIXTURE
}
;
