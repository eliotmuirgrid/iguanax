//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNparseError
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Friday, November 26th, 2010 @ 12:57:05 PM
//---------------------------------------------------------------------------
#include "TRNparseError.h"
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

// This is very dependent on Lua's error messages.
static void TRNstripString(COLstring* pFile){
   COL_FUNCTION(TRNstripString);
   COL_VAR(*pFile);
   if (pFile->substr(0, 7) == "[string"){
      *pFile = pFile->substr(9, pFile->size()-11);
   } else if (pFile->size()) {
      // Strip the leading path and the trailing extension.
      size_t LastSlash = pFile->rfind(FIL_DIR_SEPARATOR);
      size_t FileLabelStart = (LastSlash < pFile->size()-1) ? LastSlash+1 : 0;
      size_t ExtensionIndex = pFile->rfind(".");
      size_t FileLabelLength = (ExtensionIndex != npos) ? ExtensionIndex-FileLabelStart : npos;
      *pFile = pFile->substr(FileLabelStart, FileLabelLength);
   }
   COL_VAR(*pFile);
}

static void TRNparseErrorTuple(const COLstring& Clipped, COLstring* pDescription, int* pLine, COLstring* pFile){
   COL_FUNCTION(TRNparseErrorTuple);
   COL_VAR(Clipped);
   // This is very dependent on Lua's error messages.
   REXmatcher Matcher("^\\[string \"[^\"\\]]*\"\\]:\\d+:");
   COLindex Start, End;
   if (Matcher.findMatch(Clipped.c_str(), 0, Start, End)){
      COL_TRC("Regex Parse complete.");
      size_t FileSectionEnd = Clipped.find("]");
      *pFile= Clipped.substr(0, FileSectionEnd+1);
      TRNstripString(pFile);
      size_t NumberStart = FileSectionEnd + 2;
      COLstring Number = Clipped.substr(NumberStart, End-NumberStart);
      *pLine = atoi(Number.c_str());
      *pDescription = COLstripWhiteSpace(Clipped.substr(End+1), true, true);
      COL_VAR2(*pDescription, *pLine);
   } else { COL_TRC("Regex parse failed.");}
}

static bool TRNisRequireError(const COLstring& ErrorMessage, COLstring* pDescription, int* pLine, COLstring* pFile){
   COL_FUNCTION(TRNisRequireError);
   size_t i = ErrorMessage.rfind(TRN_REQUIRE_SENTINEL);
   if (i == npos) return false;
   COL_TRC("It's a require error.");
   COLstring Clipped = ErrorMessage.substr(i + strlen(TRN_REQUIRE_SENTINEL));
   COL_VAR(Clipped);
   *pFile = "";
   TRNparseErrorTuple(Clipped, pDescription, pLine, pFile);
   COL_VAR3(*pDescription, *pFile, *pLine);
   return true;
}

// start parsing from the end of the error message and look back for the \n character
void TRNparseError(const COLstring& ErrorMessage, COLstring* pDescription, int* pLine, COLstring* pFile){
   COL_FUNCTION(TRNparseError);
   COL_VAR(ErrorMessage);
   COL_DBG(FMTstreamHex(ErrorMessage.c_str(), ErrorMessage.size()));
   *pDescription = ErrorMessage;

   if (TRNisRequireError(ErrorMessage, pDescription, pLine, pFile)){
      return;
   }
   TRNparseErrorTuple(ErrorMessage, pDescription, pLine, pFile);

   COL_VAR3(*pFile,*pLine,*pDescription);
}
