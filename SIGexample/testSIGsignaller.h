#ifndef __TEST_SIGSIGNALLER_H__
#define __TEST_SIGSIGNALLER_H__
//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2005 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: testSIGsignaller.h
//
// Description:
//
// A set of utilities to test SIGsignaller Objects.
//
// Copied from COLexample/testCOLsignal.h.
//
// Author: Nasron Cheong
// Date:   Thu 02/05/2004
//
//
//---------------------------------------------------------------------------

#include <SIG/SIGsignalReceiver.h>

class TSTtestable;
class testControlParent;

class testSIGsignaller
{
public:
   typedef COLint16 TestType;
   
   testSIGsignaller()
   {
      resetPlain();
   };
   virtual ~testSIGsignaller(){};

   static testSIGsignaller* create(){ return new testSIGsignaller; }

   void testCOLvoidMethod();
   void testMixed();
   void testBasic();
   void testToPlainFunction();
   void testScoping();
   void testPureVirtualSingle();
   void testDisconnectWhileReceiving();
   void testSpeedBroadcast();
   
   void testDisconnection();


   static TSTtestable* signallerTests();

   void resetPlain()
   {
      PlainNoParamCalled = false;
      PlainOneParamCalled = false;
      PlainThreeParamCalled = false;
   }

   bool PlainNoParamCalled;
   bool PlainOneParamCalled;
   bool PlainThreeParamCalled;

private:
   testSIGsignaller(const testSIGsignaller& VectorTest);  
   testSIGsignaller& operator=(const testSIGsignaller& VectorTest);
};

#endif     // end of defensive include
