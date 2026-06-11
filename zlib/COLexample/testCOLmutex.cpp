//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testCOLmutex.h
//
// Description:
//
// Implementation 
//
// Author: Eliot Muir
// Date:   Wed  7 Apr 2021 09:13:09 EDT
//---------------------------------------------------------------------------
#include "testCOLmutex.h"

#include <TST/TSThelpers.h>
#include <TST/TSTtestCollection.h>
#include <TST/TSTtestFunction.h>

#include <COL/COLmutex.h>
#include <COL/COLthread.h>
#include <COL/COLlocker.h>
#include <COL/COLmath.h>
#include <COL/COLutils.h>
#include <COL/COLthreadPool.h>
#include <COL/COLclosure.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

class testCOLmutexCollection{
public:
   testCOLmutexCollection()  { COL_METHOD(testCOLmutexCollection::testCOLmutexCollection);  }
   ~testCOLmutexCollection() { COL_METHOD(testCOLmutexCollection::~testCOLmutexCollection); }

   COLmutex Mutex1, Mutex2;
   int Count1, Count2;
};

static void testIncrement(COLmutex* pMutex, int* pCount){
   COL_FUNCTION(testIncrement);
   COLlocker Locker(*pMutex);
   COLsleep(COLmath::randomNumber(50));
   (*pCount)++;
}

static void testDecrement(COLmutex* pMutex, int* pCount){
   COL_FUNCTION(testDecrement);
   COLlocker Locker(*pMutex);
   COLsleep(COLmath::randomNumber(200));
   (*pCount)--;
}

static void testCOLmutex(){
   COL_FUNCTION(testCOLmutex);
   testCOLmutexCollection Collection; 
   {
      COLthreadPool Pool;
      Pool.start();
      Collection.Count1 = 0;
      Collection.Count2 = 0;
      for (int i=0; i < 6; i++){
         Pool.scheduleTask(COLnewClosure0(&testIncrement, &Collection.Mutex1, &Collection.Count1));
         Pool.scheduleTask(COLnewClosure0(&testDecrement, &Collection.Mutex1, &Collection.Count1));
         Pool.scheduleTask(COLnewClosure0(&testIncrement, &Collection.Mutex2, &Collection.Count2));
         Pool.scheduleTask(COLnewClosure0(&testDecrement, &Collection.Mutex2, &Collection.Count2));
      }
      COL_TRC("Scheduled tasks.");
   }
   COL_TRC("Thread pool done.");
   TST_ASSERT_EQUALS(0, Collection.Count1);
   TST_ASSERT_EQUALS(0, Collection.Count2);
}

TSTtestable* COLmutexTests(){
   TST_START_COLLECTION("testCOLmutex")
      TST_ADD_TEST_FUNCTION(testCOLmutex);
   TST_END_FIXTURE
}

