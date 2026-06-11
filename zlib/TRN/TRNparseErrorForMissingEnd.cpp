//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNparseErrorForMissingEnd
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Friday, November 26th, 2010 @ 12:57:05 PM
//
//---------------------------------------------------------------------------
#include "TRNparseErrorForMissingEnd.h"
#include "TRNannotationState.h"
#include "TRNfile.h"
#include "TRNconstant.h"

#include <REX/REXmatcher.h>

#include <LUAL/LUALprint.h>
#include <LUA/LUAutils.h>

#include <FMT/FMTbinaryDump.h>

#include <FIL/FILutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

// Special case to catch error messages for missing 'end' in a function, and to overwrite the 'LineNumber' value
//  in the error COLvar from the last line of the file to the line of the function which has the problem.
void TRNparseErrorForMissingEnd(COLvar* pErrorDetails){
   COL_FUNCTION(TRNparseErrorForMissingEnd);
   int HaveMatch = 0;
   COLstring StrToCheck = (*pErrorDetails)["Description"].asString();
   if (StrToCheck.find(&HaveMatch, "'end' expected (to close 'function' at line")){
      COL_TRC("Have match on special check");
      int LineNumStart = 0;
      StrToCheck.find(&LineNumStart, "line");
      int LineNumEnd = 0;
      StrToCheck.find(&LineNumEnd, ")", LineNumStart);
      COLstring LineNum = (*pErrorDetails)["Description"].asString();
      LineNum.setSize(LineNumEnd);
      LineNum.shift(LineNumStart + 5); // + 5 to offset "line ", space intentional.
      COL_VAR(LineNum);
      (*pErrorDetails)["LineNumber"] = LineNum;
   }
}
