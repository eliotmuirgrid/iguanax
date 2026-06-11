//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testCOLclosure
//
// Description: test COLclosure
//---------------------------------------------------------------------------
#include "COLexamplePrecomp.h"
#pragma hdrstop

#include <stdio.h>

#include <TST/TSThelpers.h>
#include <TST/TSTtestCollection.h>
#include <TST/TSTtestFunction.h>
#include <COL/COLvar.h>
#include <COL/COLclosure.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

bool s_NoParamCalled = false;

static void NoParam(){
   COL_FUNCTION(NoParam);
   s_NoParamCalled = true;
}

static void AddOne(int* pCount){
   COL_FUNCTION(AddOne);
   (*pCount)++;
   COL_VAR(*pCount);
}
static void testClosure0_0(){
   COL_FUNCTION(testClosure0_0);
   s_NoParamCalled = false;
   COLclosure0* pClosure = COLnewClosure0(&NoParam);
   pClosure->runAndDelete();
   TST_ASSERT_EQUALS(true, s_NoParamCalled);
}

class COLclosureTarget{
public:
   COLclosureTarget() : m_Member0(false) {}
   ~COLclosureTarget() {}

   void call0() {
      COL_METHOD(COLclosureTarget::call0);
      m_Member0 = true; 
   }

   void call1(int Value) {
      COL_METHOD(COLclosureTarget::call1);
      m_Member1 = Value;
   }

   void call2(int Value1, int* pResult){
      COL_METHOD(COLclosureTarget::call2);
      *pResult = 2 * Value1;
   }

   void call8(int* pSum, int Num1, int Num2, int Num3, int Num4, int Num5, int Num6, int Num7){
      *pSum = Num1 + Num2 + Num3 + Num4 + Num5 + Num6 + Num7;
   }

   bool m_Member0;
   int m_Member1;
};

static void testClosure0_m0(){
   COL_FUNCTION(testClosure0_m0);
   COLclosureTarget ClosureTarget;

   COLclosure0* pClosure = COLnewClosure0(&ClosureTarget, &COLclosureTarget::call0);
   pClosure->runAndDelete();
   TST_ASSERT_EQUALS(true, ClosureTarget.m_Member0);
}

static void testClosure0_m1(){
   COL_FUNCTION(testClosure0_m1);
   COLclosureTarget ClosureTarget;
   COLclosure0* pClosure = COLnewClosure0(&ClosureTarget, &COLclosureTarget::call1,101);
   pClosure->runAndDelete();
   TST_ASSERT_EQUALS(101, ClosureTarget.m_Member1);
}

static void testClosure1_m0(){
   COL_FUNCTION(testClosure1_m0);
   COLclosureTarget ClosureTarget;
   COLclosure1<int>* pClosure = COLnewClosure1(&ClosureTarget, &COLclosureTarget::call1);
   pClosure->runAndDelete(100);
   TST_ASSERT_EQUALS(100, ClosureTarget.m_Member1)
}

static void testClosure1_m1(){
   COL_FUNCTION(testClosure1_m1);
   COLclosureTarget ClosureTarget;
   int Result = 0;
   COLclosure1<int>* pClosure = COLnewClosure1(&ClosureTarget, &COLclosureTarget::call2,  &Result);
   pClosure->runAndDelete(100);
   COL_VAR(Result);
   TST_ASSERT_EQUALS(200, Result);
}

static void testClosure4_m4(){
   COL_FUNCTION(testClosure4_m4);
   COLclosureTarget ClosureTarget;
   int Result=0;
   COLclosure4<int*, int, int,int>* pClosure = COLnewClosure4(&ClosureTarget, &COLclosureTarget::call8, 1,2,3,4);
   pClosure->runAndDelete(&Result, -1,-2,-3);
   COL_VAR(Result);
   TST_ASSERT_EQUALS(4, Result);
}

static void testClosure0_1(){
   COL_FUNCTION(testClosure0_1);
   int Count = 0;
   COLclosure0* pClosure = COLnewClosure0(&AddOne, &Count);
   pClosure->runAndDelete();
   TST_ASSERT_EQUALS(1, Count);
}

static void testClosure1_0(){
   COL_FUNCTION(testClosure1_0);
   int Count = 0;
   COLclosure1<int*>* pClosure = COLnewClosure1<int*>(&AddOne);
   pClosure->runAndDelete(&Count);
   TST_ASSERT_EQUALS(1, Count);
}

static void AddNumbers(int* pNum1, int* pNum2){
   COL_FUNCTION(AddNumbers);
   COL_VAR2(*pNum1, *pNum2);
   (*pNum1) += *pNum2;
   COL_VAR(*pNum1);
}

static void testClosure1_1(){
   COL_FUNCTION(testClosure1_1);
   int Count = 1;
   int Increment = 10;
   COLclosure1<int*>* pClosure = COLnewClosure1<int*>(&AddNumbers, &Increment);
   pClosure->runAndDelete(&Count);
   TST_ASSERT_EQUALS(11, Count);
}

static void HandleRequest(const COLvar& Input, COLvar* pAppData){
   COL_FUNCTION(HandleRequest);
   (*pAppData)["last_id"] = Input["id"];
}

static void testWebScenario(){
   COL_FUNCTION(testWebScenario);
   COLvar AppData;
   AppData["main_dir"] = "C:\\";
   COLvar Input;
   Input["id"] = 1212;
   Input["name"] = "Smith";
   COLclosure1<const COLvar&>* pHandler = COLnewClosure1<const COLvar&>(&HandleRequest, &AppData);
   pHandler->runAndDelete(Input);
   COL_VAR(AppData);
   TST_ASSERT_EQUALS(1212, AppData["last_id"].asInt());
}

TSTtestable* COLclosureTests(){
   TST_START_COLLECTION("COLclosureTests")
      TST_ADD_TEST_FUNCTION(testClosure0_0);
      TST_ADD_TEST_FUNCTION(testClosure0_1);
      TST_ADD_TEST_FUNCTION(testClosure0_m0);
      TST_ADD_TEST_FUNCTION(testClosure0_m1);
      TST_ADD_TEST_FUNCTION(testClosure1_0);
      TST_ADD_TEST_FUNCTION(testClosure1_1);
      TST_ADD_TEST_FUNCTION(testClosure1_m0);
      TST_ADD_TEST_FUNCTION(testClosure1_m1);
      TST_ADD_TEST_FUNCTION(testClosure4_m4);
      TST_ADD_TEST_FUNCTION(testWebScenario);
   TST_END_FIXTURE
}

