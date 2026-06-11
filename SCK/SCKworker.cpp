//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SCKworker
//
// Author: Eliot Muir
//
// Description:
//
// Implementation
//---------------------------------------------------------------------------
#include "SCKworker.h"
#include <FIL/FILatomic.h>
#include <FIL/FILutils.h>
#include <COL/COLvar.h>
#include <COL/COLthreadPool.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <SCK/SCKloop.h>
#include <PRO/PROexecute.h>

static void SCKrunCallbackOnMain(COLvar Result, COLauto<COLclosure1<COLvar>> pCallback){
   COL_FUNCTION(SCKrunCallbackOnMain);
   COL_VAR(Result.json(1));
   try{
      pCallback->run(Result);
   } catch(const COLerror& Error){
     COL_ERR("ERROR - MAJOR PROBLEM - SCKinvokeWorker HANDLER THREW EXCEPTION" << newline << Error);
   }
}

static void SCKrunFileIoCallbackOnMain(bool Success, COLstring Data, COLauto<COLclosure2<bool, COLstring>> pCallback){
   COL_FUNCTION(SCKrunFileIoCallbackOnMain);
   try{
      pCallback->run(Success, Data);
   } catch(const COLerror& Error){
      COL_ERR("ERROR - MAJOR PROBLEM - SCKwriteFile HANDLER THREW EXCEPTION" << newline << Error);     
   }
}

static void SCKrunWorker(SCKloop* pLoop, SCKworker pHandler, COLvar In, COLauto<COLclosure1<COLvar>> pCallback){
   COL_FUNCTION(SCKrunWorker);
   COLvar Result; 
   try{
      pHandler(In, &Result);
   } catch(const COLerror& Error){
      Result["error"]   = Error.description();
      Result["success"] = false;
      COL_ERR("Error: " << Error);
   }
   pLoop->post(COLnewClosure0(SCKrunCallbackOnMain, Result, pCallback)); 
}

// On Main
void SCKinvokeWorker(SCKloop* pLoop, SCKworker pHandler, const COLvar& In, COLauto<COLclosure1<COLvar>> pCallback){
   COL_FUNCTION(SCKinvokeWorker);
   COL_VAR(In.json(1));
   pLoop->threadPool()->scheduleTask(COLnewClosure0(SCKrunWorker, pLoop, pHandler, In, pCallback));
}

void SCKdummyCallback(bool Success, COLstring Result){
   COL_FUNCTION(SCKdummyCallback);
   COL_VAR2(Success, Result);
}

static void SCKwriteFileWorker(SCKloop* pLoop, const COLstring FileName, const COLstring Content, COLauto< COLclosure2<bool, COLstring> > pCallback){
   COL_FUNCTION(SCKwriteFileWorker);
   bool Success = true;
   COLstring ErrorMessage;
   FILcreateDirectoryTreeToFile(FileName, FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX);
   try {
      FILwriteFile(FileName, Content);
   } catch(const COLerror& Error){
      Success = false;
      ErrorMessage = Error.description();
      COL_ERR("Error: " << Error);
   }
   pLoop->post(COLnewClosure0(&SCKrunFileIoCallbackOnMain, Success, ErrorMessage, pCallback));
}

void SCKwriteFile(SCKloop* pLoop, const COLstring& FileName, const COLstring& Content, COLauto< COLclosure2<bool, COLstring> > pCallback){
   COL_FUNCTION(SCKwriteFile);
   pLoop->threadPool()->scheduleTask(COLnewClosure0(&SCKwriteFileWorker, pLoop, FileName, Content, pCallback));
}

static void SCKreadFileWorker(SCKloop* pLoop, const COLstring FileName, COLauto< COLclosure2<bool, COLstring> > pCallback){
   COL_FUNCTION(SCKreadFileWorker);
   bool Success = true;
   COLstring Data;
   try {
      FILreadFile(FileName, &Data);
   } catch(const COLerror& Error){
      Success = false;
      Data = Error.description();
      COL_ERR("Error: " << Error);
   }
   pLoop->post(COLnewClosure0(&SCKrunFileIoCallbackOnMain, Success, Data, pCallback));
}

void SCKreadFile(SCKloop* pLoop, const COLstring& FileName, COLauto< COLclosure2<bool, COLstring> > pCallback){
   COL_FUNCTION(SCKreadFile);
   pLoop->threadPool()->scheduleTask(COLnewClosure0(&SCKreadFileWorker, pLoop, FileName, pCallback));
}

void SCKresultOnMain(SCKprocessResult Result, COLauto<COLclosure1<SCKprocessResult>> pResult){
   COL_FUNCTION(SCKresultOnMain);
   try{
      pResult->run(Result);
   } catch (const COLerror& Error){
      COL_ERR("ERROR - MAJOR PROBLEM - SCKexecute CALLBACK THREW EXCEPTION" << newline << Error);    
   }
}

void SCKexecuteOnWorker(SCKloop* pLoop, const COLstring CommandLine, const COLstring WorkingDir, int TimeoutInSeconds, COLauto<COLclosure1<SCKprocessResult> > pCallback){
   COL_FUNCTION(SCKexecuteOnWorker);

   COLstring StdOut, StdError;
   int ExitCode = PROexecuteString(WorkingDir, CommandLine, &StdOut, &StdError, TimeoutInSeconds);
   SCKprocessResult Result(ExitCode, StdOut, StdError);
   if (ExitCode != 0) { Result.ErrorMessage = "Command failed to execute"; }
   pLoop->post(COLnewClosure0(&SCKresultOnMain, Result, pCallback));
}

void SCKexecute(SCKloop* pLoop, const COLstring& CommandLine, const COLstring& WorkingDir, int TimeoutInSeconds, COLauto<COLclosure1<SCKprocessResult> > pCallback){
   COL_FUNCTION(SCKexecute);
   pLoop->threadPool()->scheduleTask(COLnewClosure0(&SCKexecuteOnWorker, pLoop, CommandLine, WorkingDir, TimeoutInSeconds, pCallback));
}