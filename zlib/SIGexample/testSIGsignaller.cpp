//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2009 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: testSIGsignal.cpp
//
// Description:
//
// Copied from COLexample/testCOLsignal.cpp.
//
// Author: Nasron Cheong
// Date:   Thu 02/05/2004
//
//
//
//---------------------------------------------------------------------------

#include "SIGexamplePrecomp.h"
#pragma hdrstop

#include <SIG/SIGsignaller.h>

#include "testSIGsignaller.h"
#include <COL/COLdateTime.h>
#include <COL/COLostream.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

class testControlParent
{
public:
   void doNonVirtualEvent()
   {
      onNonVirtualEvent(this);
   }

   void doVirtualEvent()
   {
      onEvent(this);
   }

   void doBroadcastSignalEvent()
   {
      OnBroadcastEvent.signal(this);
   }

   virtual void onEvent(testControlParent* pThis) {}
   void onNonVirtualEvent(testControlParent* pThis);
   
   SIGsignaller1<testControlParent*> OnBroadcastEvent;
};

// I have purposely made the non virtual event non inlined.  This
// is just the control to see what the cost of a virtual function is.
void testControlParent::onNonVirtualEvent(testControlParent* pThis)
{

}


//SIGNAL TESTS
struct SignalDispatcher : public SIGsignalReceiver
{
   SignalDispatcher(){}
   virtual ~SignalDispatcher(){}

   SIGsignaller0<TVoid> SignalNoParam;
   SIGsignaller1<COLuint32> Signal1Param;
   SIGsignaller3<COLuint32,const COLstring&,COLint16> Signal3Param;
   SIGsignaller3<COLuint32,const COLstring&,COLint16> Signal3ParamDeferer;
};

struct SlotReceiverBase : public SIGsignalReceiver
{
   virtual void NoParamPureVirtual() = 0;
};

struct SlotReceiver : public SlotReceiverBase
{
   SlotReceiver() : SlotNoParamCalledCount(0)
   {
      reset();
   }

   ~SlotReceiver(){}

   void reset()
   {
      SlotNoParamPureVirtualCalled = false;
      SlotNoParamCalled = false;
      Slot1ParamCalled = false;
      Slot3ParamCalled = false;
   }

   virtual void NoParamPureVirtual(){}

   void NoParam()
   {
      SlotNoParamCalledCount++;
      COL_TRC("NoParam" << newline);
      SlotNoParamCalled = true;
   }

   void OneParam(COLuint32 Param1)
   {
      TST_ASSERT(Param1 != 0)
      COL_TRC(Param1 << newline);
      Slot1ParamCalled = true;
   }

   void ThreeParam(COLuint32 Param1, const COLstring& Param2, COLint16 Param3)
   {
      TST_ASSERT(Param1 != 0)
      TST_ASSERT(!Param2.is_null())
      TST_ASSERT(Param3 != 0)      
      COL_TRC(Param1 << Param2 << Param3 << newline);
      Slot3ParamCalled = true;
   }

   COLuint32 SlotNoParamCalledCount;
   bool SlotNoParamPureVirtualCalled;
   bool SlotNoParamCalled;
   bool Slot1ParamCalled;
   bool Slot3ParamCalled;
};

struct SlotReceiver2 : public SIGsignalReceiver
{
   SlotReceiver2() 
   {
      reset();
   }

   ~SlotReceiver2(){}

   void reset()
   {
      SlotNoParamCalled = false;
   }

   void NoParam()
   {
      COL_TRC("NoParam" << newline);
      SlotNoParamCalled = true;
   }

   bool SlotNoParamCalled;
};

struct SlotReceiverDerived : public SlotReceiver
{
   virtual void NoParamPureVirtual()
   {
      COL_TRC("Pure Virtual called" << newline);
      SlotNoParamPureVirtualCalled = true;
   }
};


void testSIGsignaller::testCOLvoidMethod()
{
   SlotReceiver Receiver;
   TST_ASSERT(Receiver.SlotNoParamCalled == false);
   TST_ASSERT(Receiver.Slot3ParamCalled == false);

   SIGvoidMethod0<SlotReceiver> TestMethod0(&Receiver,&SlotReceiver::NoParam);
   TestMethod0();
   TST_ASSERT(Receiver.SlotNoParamCalled == true);

   SIGvoidMethod3<SlotReceiver,COLuint32, const COLstring&, COLint16> TestMethod3(&Receiver,&SlotReceiver::ThreeParam);
   TestMethod3(1234,"TEST",1234);
   TST_ASSERT(Receiver.Slot3ParamCalled == true);
}


void testSIGsignaller::testBasic()
{
   SignalDispatcher Dispatcher;
   SlotReceiver Receiver;

   Dispatcher.SignalNoParam.addReceiver(&SlotReceiver::NoParam,&Receiver);
   Dispatcher.Signal1Param.addReceiver(&SlotReceiver::OneParam,&Receiver);
   Dispatcher.Signal3Param.addReceiver(&SlotReceiver::ThreeParam,&Receiver);

   Dispatcher.SignalNoParam.signal();
   TST_ASSERT(Receiver.SlotNoParamCalled == true);

   Dispatcher.Signal1Param.signal(1234);
   TST_ASSERT(Receiver.Slot1ParamCalled == true);

   Dispatcher.Signal3Param.signal(1234,"TEST",1234);
   TST_ASSERT(Receiver.Slot3ParamCalled == true);

   //connect again
   Receiver.SlotNoParamCalledCount = 0;
   Dispatcher.SignalNoParam.addReceiver(&SlotReceiver::NoParam,&Receiver);
   Dispatcher.SignalNoParam.signal();
   TST_ASSERT(Receiver.SlotNoParamCalled == true);
   //should only be called once, connecting to same function does nothing.
   TST_ASSERT(Receiver.SlotNoParamCalledCount == 1);


   //check disconnection
   Receiver.reset();
      

   Dispatcher.SignalNoParam.removeReceiver(&SlotReceiver::NoParam,&Receiver);
   Dispatcher.Signal1Param.removeReceiver(&SlotReceiver::OneParam,&Receiver);
   Dispatcher.Signal3Param.removeReceiver(&SlotReceiver::ThreeParam,&Receiver);
   
   Dispatcher.SignalNoParam.signal();
   TST_ASSERT(Receiver.SlotNoParamCalled == false);

   Dispatcher.Signal1Param.signal(1234);
   TST_ASSERT(Receiver.Slot1ParamCalled == false);

   Dispatcher.Signal3Param.signal(1234,"TEST",1234);
   TST_ASSERT(Receiver.Slot3ParamCalled == false);
}

void testSIGsignaller::testScoping()
{
   testBasic(); //requirement

   SignalDispatcher Dispatcher;
   COLauto<SlotReceiver> pReceiver = new SlotReceiver;

   Dispatcher.SignalNoParam.addReceiver(&SlotReceiver::NoParam,pReceiver.get());
   Dispatcher.Signal1Param.addReceiver(&SlotReceiver::OneParam,pReceiver.get());
   Dispatcher.Signal3Param.addReceiver(&SlotReceiver::ThreeParam,pReceiver.get());

   pReceiver = 0;

   COL_TRC("Deleted Receiver...");
   //these calls should now do nothing
   Dispatcher.SignalNoParam.signal();
   Dispatcher.Signal1Param.signal(1234);
   Dispatcher.Signal3Param.signal(1234,"TEST",1234);
   COL_TRC("dispatch OK!" << newline);
}


testSIGsignaller* pGlobalSignalTest = 0;

void COL_STD_CALL PlainNoParam()
{
   pGlobalSignalTest->PlainNoParamCalled = true;
   COL_TRC("No Param " << newline);
}

void COL_STD_CALL PlainOneParam(COLuint32 Param1)
{
   pGlobalSignalTest->PlainOneParamCalled = true;
   COL_TRC("One Param " << newline);
}

void COL_STD_CALL PlainThreeParam(COLuint32 Param1, const COLstring& Param2, COLint16 Param3)
{
   pGlobalSignalTest->PlainThreeParamCalled = true;
   COL_TRC("Three Param " << newline);
}

void testSIGsignaller::testPureVirtualSingle()
{
   SignalDispatcher Dispatcher;
   SlotReceiverDerived Receiver;

   Dispatcher.SignalNoParam.addReceiver(&SlotReceiverDerived::NoParamPureVirtual, &Receiver);
   Dispatcher.SignalNoParam.signal();
   TST_ASSERT(Receiver.SlotNoParamPureVirtualCalled == true);
}

void testSIGsignaller::testToPlainFunction()
{
   pGlobalSignalTest = this;
   SignalDispatcher Dispatcher;

   Dispatcher.SignalNoParam.addReceiver(&PlainNoParam);
   Dispatcher.SignalNoParam.addReceiver(&PlainNoParam);
   Dispatcher.SignalNoParam.signal();
   Dispatcher.Signal1Param.addReceiver(&PlainOneParam);
   Dispatcher.Signal1Param.signal(123);
   Dispatcher.Signal3Param.addReceiver(&PlainThreeParam);
   Dispatcher.Signal3Param.signal(123,"hello",123);

   TST_ASSERT(PlainNoParamCalled == true);
   TST_ASSERT(PlainOneParamCalled == true);
   TST_ASSERT(PlainThreeParamCalled == true);

   resetPlain();

   Dispatcher.SignalNoParam.removeReceiver(&PlainNoParam);
   Dispatcher.SignalNoParam.signal();
   Dispatcher.Signal1Param.removeReceiver(&PlainOneParam);
   Dispatcher.Signal1Param.signal(123);
   Dispatcher.Signal3Param.removeReceiver(&PlainThreeParam);
   Dispatcher.Signal3Param.signal(123,"hello",123);

   TST_ASSERT(PlainNoParamCalled == false);
   TST_ASSERT(PlainOneParamCalled == false);
   TST_ASSERT(PlainThreeParamCalled == false);

}

void testSIGsignaller::testMixed()
{
   pGlobalSignalTest = this;
   resetPlain();

   SignalDispatcher Dispatcher;
   SlotReceiver Receiver;
   SlotReceiver2 Receiver2;

   Dispatcher.SignalNoParam.addReceiver(&SlotReceiver::NoParam,&Receiver);
   Dispatcher.SignalNoParam.addReceiver(&PlainNoParam);
   Dispatcher.SignalNoParam.addReceiver(&SlotReceiver2::NoParam,&Receiver2);
   Dispatcher.SignalNoParam.signal();
   TST_ASSERT(PlainNoParamCalled == true);
   TST_ASSERT(Receiver.SlotNoParamCalled == true);
   TST_ASSERT(Receiver2.SlotNoParamCalled == true);
}

template <class TSignalDispatcher>
struct SlotReceiverDisconnecting : public SIGsignalReceiver
{
   SlotReceiverDisconnecting(TSignalDispatcher& iDispatcher)
      : Dispatcher(iDispatcher)
   {
      reset();
   }
   virtual ~SlotReceiverDisconnecting(){};

   void reset()
   {
      DisconnecterCalled = false;
   }

   void DisconnectOurDispatcher();

   void DisconnectOurDispatcherAgain();

   bool DisconnecterCalled;
   TSignalDispatcher& Dispatcher;
};

//regular signal specialization
template <>
void SlotReceiverDisconnecting<SignalDispatcher>::DisconnectOurDispatcher()
{
   //disconnect us from what actually called us.
   Dispatcher.SignalNoParam.removeReceiver(&SlotReceiverDisconnecting::DisconnectOurDispatcher,this);
   DisconnecterCalled = true;
}

template <>
void SlotReceiverDisconnecting<SignalDispatcher>::DisconnectOurDispatcherAgain()
{
   //disconnect us from what actually called us.
   Dispatcher.SignalNoParam.removeReceiver(&SlotReceiverDisconnecting::DisconnectOurDispatcherAgain,this);
   DisconnecterCalled = true;
}

void testSIGsignaller::testDisconnectWhileReceiving()
{
   { //test for only one target
      SignalDispatcher Dispatcher;
      typedef SlotReceiverDisconnecting<SignalDispatcher> TDisconnectingReceiver;
   
      TDisconnectingReceiver Receiver(Dispatcher);

      Dispatcher.SignalNoParam.addReceiver(&TDisconnectingReceiver::DisconnectOurDispatcher,&Receiver);

      Dispatcher.SignalNoParam.signal();
      TST_ASSERT(Receiver.DisconnecterCalled == true);

      //calling again should do nothing
      Receiver.reset();
      Dispatcher.SignalNoParam.signal();
      TST_ASSERT(Receiver.DisconnecterCalled == false);
   }

   { //test for 2 targets
      SignalDispatcher Dispatcher;
      typedef SlotReceiverDisconnecting<SignalDispatcher> TDisconnectingReceiver;
   
      TDisconnectingReceiver Receiver(Dispatcher);

      Dispatcher.SignalNoParam.addReceiver(&TDisconnectingReceiver::DisconnectOurDispatcher,&Receiver);
      Dispatcher.SignalNoParam.addReceiver(&TDisconnectingReceiver::DisconnectOurDispatcherAgain,&Receiver);

      Dispatcher.SignalNoParam.signal();
      TST_ASSERT(Receiver.DisconnecterCalled == true);

      //calling again should do nothing
      Receiver.reset();
      Dispatcher.SignalNoParam.signal();
      TST_ASSERT(Receiver.DisconnecterCalled == false);
   }

}

class testControlChildBroadcast
: public testControlParent,
  public SIGsignalReceiver
{
public:
   testControlChildBroadcast()
   {
      OnBroadcastEvent.addReceiver(&testControlChildBroadcast::onSignalEvent, this);
      OnBroadcastEvent.addReceiver(&testControlChildBroadcast::onSignalEvent2, this);
   }
   virtual ~testControlChildBroadcast()
   {
   }

   void onSignalEvent(testControlParent* pThis);
   void onSignalEvent2(testControlParent* pThis);
};

void testControlChildBroadcast::onSignalEvent(testControlParent* pThis)
{
   // do nothing
}

void testControlChildBroadcast::onSignalEvent2(testControlParent* pThis)
{
   // do nothing
}

void testSIGsignaller::testSpeedBroadcast()
{
   testControlChildBroadcast SignalObject;

   // use the following line instead if you really want to see the 
   // speed difference between the different implementations
   //static const COLuint32 CountOfOperation = 100000000;
   static const COLuint32 CountOfOperation = 5000000;

   COLuint32 OperationIndex;
   COLdateTime StartTime = COLdateTime::currentTime();  
   for (OperationIndex = 0; OperationIndex < CountOfOperation; OperationIndex++)
   {
      SignalObject.doBroadcastSignalEvent();
   }
   COLdateTime EndBroadCastTime = COLdateTime::currentTime();

   COL_TRC("To do " << CountOfOperation << " operations to 2 slots took: " << newline
           << "SIGsignaller           : " << EndBroadCastTime - StartTime << newline);
}

void testSIGsignaller::testDisconnection()
{
   {
      SignalDispatcher Dispatcher1;
      {
         SlotReceiver Receiver1;
         Dispatcher1.SignalNoParam.addReceiver(&SlotReceiver::NoParam,&Receiver1);
      }
      Dispatcher1.SignalNoParam.signal(); //should be safe
   }
   {
      SignalDispatcher Dispatcher1;
      {
         SlotReceiver Receiver1;
         Dispatcher1.SignalNoParam.addReceiver(&SlotReceiver::NoParam,&Receiver1);
         Dispatcher1.SignalNoParam.removeReceiver(&SlotReceiver::NoParam,&Receiver1);
      }
      Dispatcher1.SignalNoParam.signal(); //should be safe
   }
   {
      SignalDispatcher Dispatcher1;
      {
         SlotReceiver Receiver1;
         Dispatcher1.SignalNoParam.addReceiver(&SlotReceiver::NoParam,&Receiver1);
         Dispatcher1.SignalNoParam.removeReceiver(&SlotReceiver::NoParam,&Receiver1);
         Dispatcher1.SignalNoParam.addReceiver(&SlotReceiver::NoParam,&Receiver1);
         Dispatcher1.SignalNoParam.removeReceiver(&SlotReceiver::NoParam,&Receiver1);
         Dispatcher1.SignalNoParam.addReceiver(&SlotReceiver::NoParam,&Receiver1);
         Dispatcher1.SignalNoParam.removeReceiver(&SlotReceiver::NoParam,&Receiver1);
      }
      Dispatcher1.SignalNoParam.signal(); //should be safe
   }
   {
      SignalDispatcher Dispatcher1;
      {
         SlotReceiver Receiver1;
         SlotReceiver Receiver2;
         SlotReceiver Receiver3;
         Dispatcher1.SignalNoParam.addReceiver(&SlotReceiver::NoParam,&Receiver1);
         Dispatcher1.SignalNoParam.addReceiver(&SlotReceiver::NoParam,&Receiver1);
         Dispatcher1.SignalNoParam.addReceiver(&SlotReceiver::NoParam,&Receiver2);
         Dispatcher1.SignalNoParam.addReceiver(&SlotReceiver::NoParam,&Receiver2);
         Dispatcher1.SignalNoParam.addReceiver(&SlotReceiver::NoParam,&Receiver3);
         Dispatcher1.SignalNoParam.addReceiver(&SlotReceiver::NoParam,&Receiver3);
      }
      Dispatcher1.SignalNoParam.signal(); //should be safe
   }
   {
      SignalDispatcher Dispatcher1;
      {
         SlotReceiver Receiver1;
         SlotReceiver Receiver2;
         SlotReceiver Receiver3;
         Dispatcher1.SignalNoParam.addReceiver(&SlotReceiver::NoParam,&Receiver1);
         Dispatcher1.SignalNoParam.addReceiver(&SlotReceiver::NoParam,&Receiver2);
         Dispatcher1.SignalNoParam.addReceiver(&SlotReceiver::NoParam,&Receiver3);
         Dispatcher1.SignalNoParam.removeReceiver(&SlotReceiver::NoParam,&Receiver1);
         Dispatcher1.SignalNoParam.removeReceiver(&SlotReceiver::NoParam,&Receiver2);
         Dispatcher1.SignalNoParam.removeReceiver(&SlotReceiver::NoParam,&Receiver3);
      }
      Dispatcher1.SignalNoParam.signal(); //should be safe
   }
   {
      SignalDispatcher Dispatcher1;
      {
         SlotReceiver Receiver1;
         SlotReceiver Receiver2;
         SlotReceiver Receiver3;
         Dispatcher1.SignalNoParam.addReceiver(&SlotReceiver::NoParam,&Receiver1);
         Dispatcher1.SignalNoParam.removeReceiver(&SlotReceiver::NoParam,&Receiver1);
         Dispatcher1.SignalNoParam.addReceiver(&SlotReceiver::NoParam,&Receiver2);
         Dispatcher1.SignalNoParam.removeReceiver(&SlotReceiver::NoParam,&Receiver2);
         Dispatcher1.SignalNoParam.addReceiver(&SlotReceiver::NoParam,&Receiver3);
         Dispatcher1.SignalNoParam.removeReceiver(&SlotReceiver::NoParam,&Receiver3);
      }
      Dispatcher1.SignalNoParam.signal(); //should be safe
   }
   {
      SignalDispatcher Dispatcher1;
      {
         SlotReceiver Receiver1;
         SlotReceiver Receiver2;
         SlotReceiver Receiver3;
         Dispatcher1.SignalNoParam.addReceiver(&SlotReceiver::NoParam,&Receiver1);
         Dispatcher1.SignalNoParam.addReceiver(&SlotReceiver::NoParam,&Receiver2);
         Dispatcher1.SignalNoParam.addReceiver(&SlotReceiver::NoParam,&Receiver3);
      }
      Dispatcher1.SignalNoParam.signal(); //should be safe
   }
   {
      SignalDispatcher Dispatcher1;
      SlotReceiver Receiver1;
      Dispatcher1.SignalNoParam.addReceiver(&SlotReceiver::NoParam,&Receiver1);
      Dispatcher1.SignalNoParam.clear();
      Dispatcher1.SignalNoParam.signal(); //should be safe
   }
   {
      SignalDispatcher Dispatcher1;
      SlotReceiver Receiver1;
      SlotReceiver Receiver2;
      SlotReceiver Receiver3;
      Dispatcher1.SignalNoParam.addReceiver(&SlotReceiver::NoParam,&Receiver1);
      Dispatcher1.SignalNoParam.addReceiver(&SlotReceiver::NoParam,&Receiver2);
      Dispatcher1.SignalNoParam.addReceiver(&SlotReceiver::NoParam,&Receiver3);
      Dispatcher1.SignalNoParam.clear();
      Dispatcher1.SignalNoParam.signal(); //should be safe
   }

}

TSTtestable* testSIGsignaller::signallerTests()
{
   TST_START_FIXTURE(testSIGsignaller)
      TST_ADD_TEST_METHOD(testCOLvoidMethod)
      TST_ADD_TEST_METHOD(testScoping)
      TST_ADD_TEST_METHOD(testBasic);
      TST_ADD_TEST_METHOD(testToPlainFunction)
      TST_ADD_TEST_METHOD(testPureVirtualSingle)
      TST_ADD_TEST_METHOD(testMixed)
      TST_ADD_TEST_METHOD(testDisconnectWhileReceiving)
      //TST_ADD_TEST_METHOD(testSpeedBroadcast)
      TST_ADD_TEST_METHOD(testDisconnection)
   TST_END_FIXTURE
}
