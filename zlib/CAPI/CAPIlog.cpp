//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CAPIlog
//
// Description:
//
// Implementation
//
// Author: John Q
// Date:   Mon  4 Apr 2022 11:30:30 EDT
//---------------------------------------------------------------------------
#include "CAPIlog.h"

#include <COL/COLlog.h>
#include <COL/COLstring.h>
COL_LOG_MODULE;


static void logToStdOut(const COLstring& Message){
   COLcout << Message << newline;
}

static void logToStdErr(const COLstring& Message){
   COLcerr << Message << newline;
}

static CAPIlogFcn pStdOut = &logToStdOut;
static CAPIlogFcn pStdErr = &logToStdErr;

void CAPIlogStdout(const COLstring& Message) {
   (*pStdOut)(Message);
}

void CAPIlogStderr(const COLstring& Message) {
   (*pStdErr)(Message);
}

void CAPIredirectStdout(CAPIlogFcn pCallback) {
   pStdOut = pCallback;
}
void CAPIredirectStderr(CAPIlogFcn pCallback) {
   pStdOut = pCallback;
}
