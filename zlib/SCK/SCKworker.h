#ifndef __SCK_WORKER_H__
#define __SCK_WORKER_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SCKworker
//
// Author: Eliot Muir
//
// Description:
//
// A safe by design way to invoke a thread.  This interface stops the sharing of
// data between threads.
//---------------------------------------------------------------------------

#include <COL/COLauto.h>
#include <COL/COLclosure6.h>

class SCKloop;

// Deliberately not allowing C++ objects to be passed in.
// https://interfaceware.atlassian.net/wiki/spaces/DEVELOPMEN/pages/2333179922/Handling+worker+threads+safely
typedef void (*SCKworker)(const COLvar& In, COLvar* pOut);
void SCKinvokeWorker(SCKloop* pLoop, SCKworker pHandler, const COLvar& In, COLauto<COLclosure1<COLvar>> pCallback);

// Non blocking asynchronous save file, it will do an atomic save, if successful bool = true, if unsuccessful bool = false and COLstring contains error message
void SCKwriteFile(SCKloop* pLoop, const COLstring& FileName, const COLstring& Content, COLauto< COLclosure2<bool, COLstring> > pCallback);
// Non blocking asynchronous read file, if successful bool = true and COLstring has the data, if unsuccessful bool = false and COLstring contains error message
void SCKreadFile(SCKloop* pLoop, const COLstring& FileName, COLauto< COLclosure2<bool, COLstring> > pCallback);

struct SCKprocessResult {
  COLstring ErrorMessage;
  int ExitCode; // TODO - document what the exit codes mean.
  COLstring StandardOut;
  COLstring StandardErr;
  // TODO no return code from PROexecute
  
  SCKprocessResult(int Code, const COLstring& Out, const COLstring& Err) 
    : ExitCode(Code), StandardOut(Out), StandardErr(Err){}
};

// If WorkingDir is blank we default to the working directory of the running process
void SCKexecute(SCKloop* pLoop, const COLstring& CommandLine, const COLstring& WorkingDir, int TimeoutInSeconds, COLauto<COLclosure1<SCKprocessResult> > pCallback);

// use me if you don't care about result
void SCKdummyCallback(bool Success, COLstring Result);
#endif // end of defensive include
