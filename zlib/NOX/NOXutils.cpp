//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NOXutils
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Tuesday, July 26th, 2011 @ 12:12:23 PM
//
//---------------------------------------------------------------------------
#include "NOXprecomp.h"
#pragma hdrstop

#include "NOXutils.h"

#include <XML/XMLexpatParser.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

// The number of characters forwards and backwards from the error
// location we will show.
#define NOX_ERROR_SNIPPET_SIZE 40

void NOXrethrowExceptionWithSource(const XMLexpatParserError& Error, const COLstring& XmlSource) {
   COL_FUNCTION(NOXrethrowExceptionWithSource);
   COL_VAR2(Error, XmlSource);

   const long ErrorStart = Error.Byte;
   const long ErrorEnd   = ErrorStart + Error.Length;
   COL_VAR3(ErrorStart, ErrorEnd, Error.Length);

   long SnippetBegin = ErrorStart;
   while (SnippetBegin >= 0 && ErrorStart - SnippetBegin <= NOX_ERROR_SNIPPET_SIZE &&
          XmlSource[SnippetBegin] != '\r' && XmlSource[SnippetBegin] != '\n'){
      --SnippetBegin;
   }
   bool LeadingEllipsis = (SnippetBegin >= 0 && XmlSource[SnippetBegin] != '\r' && XmlSource[SnippetBegin] != '\n');
   ++SnippetBegin;

   long SnippetEnd = ErrorEnd;
   while (SnippetEnd < XmlSource.size() && SnippetEnd - ErrorEnd <= NOX_ERROR_SNIPPET_SIZE &&
          XmlSource[SnippetEnd] != '\r' && XmlSource[SnippetEnd] != '\n'){
      ++SnippetEnd;
   }
   bool TrailingEllipsis = (SnippetEnd < XmlSource.size() && XmlSource[SnippetEnd] != '\r' && XmlSource[SnippetEnd] != '\n');

   COLstring NewErrorText;
   COLostream NewErrorStream(NewErrorText);
   NewErrorStream << Error.description() << '\n';
   if (LeadingEllipsis) NewErrorStream << COL_T("...");
   NewErrorStream << XmlSource.substr(SnippetBegin, SnippetEnd - SnippetBegin);
   if (TrailingEllipsis) NewErrorStream << COL_T("...");
   NewErrorStream << '\n';
   if (LeadingEllipsis) NewErrorStream << COL_T("   ");
   for (long i = SnippetBegin; i < ErrorStart; i++){
      NewErrorStream << ' ';
   }
   NewErrorStream << '^';
   if (Error.Length > 1){
      for (int i = 1; i < Error.Length && ErrorStart + i < SnippetEnd; i++){
         NewErrorStream << '^';
      }
   }

   throw XMLexpatParserError(NewErrorText, Error.code(), Error.Line, Error.Column, Error.Byte, Error.Length);
}
