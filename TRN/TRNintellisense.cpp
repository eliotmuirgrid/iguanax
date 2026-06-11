//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNintellisense
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Monday, December 20th, 2010 @ 01:18:11 PM
//---------------------------------------------------------------------------

#include "TRNintellisense.h"
#include "TRNintellisenseContext.h"

#include <COL/COLvar.h>
#include <COL/COLlog.h>
#include <HELP/HELPload.h>
COL_LOG_MODULE;

void TRNintellisenseJson(COLvar* pOut, const TRNintellisenseContext& Context, int LineNumber, int CursorPosition){
   COL_FUNCTION(TRNintellisenseJson);
   COLvar& Out = *pOut;
   Out["full_expression"] = Context.ParseContext.FullExpression;
   Out["trimmed_expression"] = Context.ParseContext.TrimmedExpression;
   Out["search_string"] = Context.ParseContext.SearchString;
   Out["line_number"] = LineNumber;
   Out["cursor_position"] = CursorPosition;
   Out["help_file"] = Context.HelpFile;
   COLvar& OptionArray = Out["options"].setArrayType();
   const TRNsortedOptionsList& Options = Context.Options;
   for (COLavlTreePlace i = Options.first(); i; i = Options.next(i)){
      const TRNintellisenseOption& Option = Options.option(i);
      COLvar& Item = OptionArray.push_back();
      Item["pt"] = Option.ProtocolType;
      Item["nt"] = Option.NodeType;
      Item["s"]  = Option.IsSimple;
      Item["ft"] = Option.FullText;
      Item["dt"]=  Option.DisplayText;
      COL_VAR(OptionArray);
   }
   Out["options_truncated"] = Options.optionsTruncated();
   COL_VAR(pOut->json(true));
}
