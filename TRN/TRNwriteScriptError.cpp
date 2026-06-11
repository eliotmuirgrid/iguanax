//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNwriteScriptError
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Friday, November 26th, 2010 @ 12:57:05 PM
//---------------------------------------------------------------------------
#include "TRNwriteScriptError.h"
#include "TRNannotationState.h"
#include "TRNfile.h"
#include "TRNconstant.h"
#include "TRNparseError.h"
#include "TRNparseErrorForMissingEnd.h"

#include <REX/REXmatcher.h>

#include <LUAL/LUALprint.h>
#include <LUA/LUAutils.h>

#include <FMT/FMTbinaryDump.h>

#include <FIL/FILutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void TRNwriteScriptError(COLvar* pErrorTank, const COLstring& ErrorMessage, const TRNannotationState& State, bool CompileError){
   COL_FUNCTION(TRNwriteScriptError);

   // Get the error file/line from the event handler.
   int LineNumber;
   COLstring FileName, FinalErrorMessage(ErrorMessage);
   if (TRNgetErrorFileAndLine(State.m_ErrorFile, State.m_ErrorLine, State.m_Stack, LineNumber, FileName, CompileError) ){
      // Error was not thrown in an external file, so we should be
      // able to parse the error message to get a possibly "more correct"
      // filename/line.  So if the error message contains a file/line, use that.
      TRNparseError(ErrorMessage, &FinalErrorMessage, &LineNumber, &FileName);
   }
   COL_VAR3(FileName, LineNumber, FinalErrorMessage);

   COLvar ErrorDetails;
   ErrorDetails["Description"] = FinalErrorMessage;

   if (FileName.is_null()){
      ErrorDetails["FileName"] = TRN_MAIN_FILE_NAME;
   } else {
      ErrorDetails["FileName"] = FileName;
   }
   if (LineNumber != -1) {
      ErrorDetails["LineNumber"] = LineNumber;
   }
   TRNparseErrorForMissingEnd(&ErrorDetails);
   if (CompileError) {
      (*pErrorTank)["ScriptError"].swap(&ErrorDetails);
      return; // No call stack if there's a compilation error
   }
   // Note that the top of the call stack may not be the same location
   // specified by ParsedFile and ParsedLine.  If the error was thrown
   // a N levels up (e.g. error('foo', N)) then the stack trace will
   // still show the COMPLETE stack up to the call to error().  This
   // is very handy, and should not be "fixed".
   if (State.m_Stack.size()){
      COLvar StackTank;
      TRNdumpCallStackVar(&StackTank, State.m_Stack, State.m_HashStack);
      ErrorDetails["CallStack"].swap(&StackTank);
   }
   (*pErrorTank)["ScriptError"].swap(&ErrorDetails);
   COL_VAR(pErrorTank->json(true));
}
