//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNshouldDoVariableAutocompletion
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Monday, December 20th, 2010 @ 02:27:26 PM
//---------------------------------------------------------------------------
#include "TRNvariableAutocompletion.h"
#include <LUACX/LUAXgrabOpenExpression.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

bool TRNshouldDoVariableAutocompletion(const LUAXautocompletionContext& Context){
   COL_FUNCTION(TRNshouldDoVariableAutocompletion);
   bool Result = false;
   if (Context.TrimmedExpression.is_null()){
      COL_TRC("Trimmed expression is empty so true");
      Result = true;
   } else if (!Context.SearchString.is_null()){
      COL_TRC("We have search string");
      COL_VAR(Context.SearchString);
      if (Context.IndexChar == '('){
         COL_TRC("Index character is ()");
         Result = true;
      } else if (Context.IndexChar == '{'){
         COL_TRC("Index character is {");
         COL_VAR(Context.LastPunctuation);
         Result = Context.LastPunctuation != '{' && Context.LastPunctuation != ',';
      }
   }
   COL_VAR(Result);
   return Result;
}
