// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: POPcolor
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Tuesday 19 September 2023 - 04:25PM
// ---------------------------------------------------------------------------

#include <POP/POPcolor.h>
#include <PRO/PROwhich.h>
#include <PRO/PROexecute.h>

#include <stdlib.h>

#include <FIL/FILutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;


int s_IWEBcolorOn = 0;   // 0 means we haven't detected it.

int IWEBrunTput(){
   COL_FUNCTION(IWEBrunTput);
   COLstring Path;
   if (!PROwhich(&Path, "tput")){
      COL_TRC("Could not find tput on path - assume no color.");
      return 2;  
   }
   COLstring Out, Err;
   int ExitCode = PROexecuteString(".", "tput colors", &Out, &Err);
   COL_VAR(Out);
   int Count = atoi(Out.c_str());
   COL_VAR(Count);
   if (Count > 200){
      COL_TRC("This terminal supports color!");
      return 1;
   } else {
      COL_TRC("Color not enabled.");
      return 2;
   }
}

bool IWEBcheckTerm(){
   if (s_IWEBcolorOn == 0){
      s_IWEBcolorOn = IWEBrunTput();
   }
   COL_VAR(s_IWEBcolorOn);
   return (s_IWEBcolorOn == 1);  // This means tput ran
}

bool IWEBhasColor(){
   #ifdef _WIN32
   return false;
   #else
   return IWEBcheckTerm();
   #endif
}


COLstring POPpop(const COLstring& In){
   COL_FUNCTION(POPpop);
   if (!IWEBhasColor()){
      return In;
   }
   COLstring Out = "\033[1;4;94;107m" + In + "\033[0m";
   return Out;
}

COLstring POPwarning(const COLstring& In){
   COL_FUNCTION(POPwarning);
   if (!IWEBhasColor()){
      return In;
   }
   COLstring Out = "\033[97;41m" + In + "\033[0m";
   return Out;
}

COLstring POPiguanaCommandLine(bool NeedRoot){
   COL_FUNCTION(POPiguanaCommandLine);
   COLstring Result;
   #ifdef __linux__
   if (NeedRoot){
      Result = "sudo ";
   }
   #endif
   Result += "\"" + FILexecutableDir() + FILexecutableFileName() + "\"";
   return Result;
}
