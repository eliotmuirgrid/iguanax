//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LHLPhelp
//
// Description:
//
// Implementation
//
// Author: Nasron Cheong
// Date:   Wednesday, September 21st, 2011 @ 10:36:33 AM
//
//---------------------------------------------------------------------------
#include "LHLPhelp.h"

#include <ctype.h>
#include <SFI/SFIhtmlCrop.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

COLstring LHLPhelpTruncateText(const COLstring& Text, COLuint32 MaxLength){
   COLstring Out;
   SFIhtmlCrop(Text.c_str(),Text.size(),"&hellip;",MaxLength,Out);
   return Out;
}

void LHLPhelpAddStyledValue(COLostream& Stream, const COLstring& Val){
   COLstring Out = Val;
   COL_VAR2(Val, Out);
   Out.replace("<p>",  "");
   Out.replace("</p>", "");
   COL_VAR(Out);
   Stream << COL_T(" <span class=\"ITSintellisenseOptionHelpValue\">") << Out << COL_T("</span>");
}
