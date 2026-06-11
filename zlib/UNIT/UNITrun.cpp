//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: UNITrun
//
// Author: Eliot Muir
//
// Date: Wed 17 Mar 2021 13:18:26 EDT
//
// Description:
//
// Implementation
//---------------------------------------------------------------------------
#include "UNITrun.h"
#include "UNITlastFailed.h"

#include <COL/COLthreadPool.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <SCK/SCKloop.h>

static void UNITcancelTimer(SCKloop* pLoop, int* pId){
   COL_FUNCTION(UNITcancelTimer);
   if (*pId != -1){
      pLoop->cancelTimer(*pId);
      *pId = -1;
   }
}

UNITsummary::~UNITsummary(){
   COL_METHOD(UNITsummary::~UNITsummary);
}

void UNITtimedOut(SCKloop* pLoop, UNITsummary* pSummary){
   COL_FUNCTION(UNITtimedOut);
   COLcout << "Timed out waiting for tests to complete." << newline;
   pLoop->shutdown();
   pSummary->Hung = true;
}

void UNITresetTimeout(SCKloop* pLoop, UNITsummary* pSummary){
   COL_FUNCTION(UNITresetTimeout);
   UNITcancelTimer(pLoop, &pSummary->TimeoutTimerId);
   // JULIAN TO UNDO
   // pSummary->TimeoutTimerId = pLoop->addTimer(COLnewClosure0(&UNITtimedOut, pLoop, pSummary), 10000);  // TODO - might need to expose this in the UNITapp interface.
   pSummary->TimeoutTimerId = pLoop->addTimer(COLnewClosure0(&UNITtimedOut, pLoop, pSummary), 500000);  // TODO - might need to expose this in the UNITapp interface.
}

class UNITstatus{
public:
   COLstring Name;
   int Time;
   COLstring Fail;
};

void UNITupdateSummary(UNITsummary* pSummary, const UNITstatus& Status){
   COL_FUNCTION(UNITcomplete);
   COL_VAR(pSummary->TestCount);
   COLcout << pSummary->TestCount;
   if (!Status.Fail.is_null()){
      COLcout << " [FAIL   ] ";
      pSummary->m_FailedBuilds[Status.Name] = Status.Fail;
   } else {
      pSummary->m_FailedBuilds[Status.Name] = "succeeded";
      COLcout << " [Success] ";
      pSummary->CountOfFail--;
   }
   COLcout << Status.Name;
   if (Status.Time > 0){
      COLcout << " [" << Status.Time << " milliseconds]";
   }
   COLcout << newline;
   if (!Status.Fail.is_null()){
      COLcout << Status.Fail << newline;
   }
   (pSummary->TestCount)--;
}

void UNITcomplete(SCKloop* pLoop, UNITsummary* pSummary, UNITstatus Status){
   COL_FUNCTION(UNITcomplete);
   UNITupdateSummary(pSummary, Status);
   if (pSummary->TestCount == 0){
      pLoop->shutdown();
   } else {
      COL_TRC("Reset the timeout.");
      UNITresetTimeout(pLoop, pSummary);
   }
}

UNITstatus UNITexecuteTest(const COLstring& Name, const COLclosure0* pFunc){
   COL_FUNCTION(UNITexecuteTest);
   COLtimeStamp Start;
   COLcurrentTimeStamp(&Start);
   UNITstatus Status;
   try{
      pFunc->run();
   } catch (COLerror& Error){
      Status.Fail = Error.description(); 
   } catch(...){
      Status.Fail = "Unhandled exception.";
   }
   COLtimeStamp End;
   COLcurrentTimeStamp(&End);
   Status.Name = Name;
   Status.Time = COLtimeStampDiffInMillisecond(End,Start);
   return Status;
}

void UNITrunTest(SCKloop* pLoop, COLstring Name, UNITsummary* pSummary, const COLclosure0* pFunc){
   COL_FUNCTION(UNITrunTest);
   UNITstatus Status = UNITexecuteTest(Name, pFunc);
   pLoop->post(COLnewClosure0(&UNITcomplete, pLoop, pSummary, Status));
}

int UNITprintSummary(const UNITsummary& Summary){
   COL_FUNCTION(UNITprintSummary);
   int Result = EXIT_SUCCESS;
   if (Summary.CountOfFail == 0){
      COLcout << "COMPLETE SUCCESS" << newline;
   }else{
      COLcout << Summary.CountOfFail << (Summary.CountOfFail==1 ? " test failed" : " tests failed") << newline;
   }

   for (COLmap<COLstring, COLstring>::const_iterator i = Summary.m_FailedBuilds.cbegin(); i != Summary.m_FailedBuilds.cend(); i++){
      if (i->second != "succeeded"){
         COLcout << " " << i->first << newline;
         Result = EXIT_FAILURE;
      }
   }   
   return Result;
}

int UNITrunTestCollection(const COLmap<COLstring, COLauto<COLclosure0> >& Tests){
   COL_FUNCTION(UNITrunTestCollection);
   if (Tests.size() ==0){
      COLcout << "COMPLETE SUCCESS" << newline;
      return EXIT_SUCCESS;
   }
   COLtimeStamp Start;
   COLcurrentTimeStamp(&Start);
   COLthreadPool Pool(1,100);
   Pool.start();
   SCKloop Loop(&Pool);
   UNITsummary Summary(Tests.size());
   for (auto i = Tests.cbegin(); i != Tests.cend(); i++){
      Summary.m_FailedBuilds.add(i->first, "Timed out");
      Pool.scheduleTask(COLnewClosure0(&UNITrunTest, &Loop, i->first, &Summary, i->second.get()));
   }
   UNITresetTimeout(&Loop, &Summary);
   Loop.start();
   UNITcancelTimer(&Loop, &Summary.TimeoutTimerId);

   COLtimeStamp End;
   COLcurrentTimeStamp(&End);
   COLcout << "Time taken: " << COLtimeStampDiffInMillisecond(End, Start) << " milliseconds " << newline;
   COLcout << newline << "Completed tests - " ;
   int Success = UNITprintSummary(Summary);
   UNITsaveFailedTests(Summary);
   if (Summary.Hung){
      abort();
   }
   return Success;
}

int UNITrunSingleThreadedTestCollection(const COLmap<COLstring, COLauto<COLclosure0> >& Tests){
   COL_FUNCTION(UNITrunSingleThreadedTestCollection);
   if (Tests.size() ==0){
      COLcout << "COMPLETE SUCCESS" << newline;
      return EXIT_SUCCESS;
   }
   COLtimeStamp Start;
   COLcurrentTimeStamp(&Start);
   UNITsummary Summary(Tests.size());
   for (auto i = Tests.cbegin(); i != Tests.cend(); i++){
      Summary.m_FailedBuilds.add(i->first, "Timed out");
      UNITstatus Status = UNITexecuteTest(i->first, i->second.get());
      UNITupdateSummary(&Summary, Status);
   }
   COLtimeStamp End;
   COLcurrentTimeStamp(&End);
   COLcout << "Time taken: " << COLtimeStampDiffInMillisecond(End, Start) << " milliseconds " << newline;
   COLcout << newline << "Completed tests - " ;
   int Success = UNITprintSummary(Summary);
   UNITsaveFailedTests(Summary);
   if (Summary.Hung){
      abort();
   }
   return Success;
}
