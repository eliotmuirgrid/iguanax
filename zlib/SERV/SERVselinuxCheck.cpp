// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SERVselinuxCheck
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Tuesday 19 September 2023 - 12:52PM
// ---------------------------------------------------------------------------

#include <SERV/SERVselinuxCheck.h>

#include <PRO/PROwhich.h>
#include <PRO/PROexecute.h>

#include <POP/POPcolor.h>

#include <LINK/LINKconstants.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void SERVselinuxWarning(){
   COLcout << POPwarning("WARNING!") << " SELinux appears to be running on your system." << newline;
   COLcout << "To run as a systemd service additional configuration is required, please read:" << newline;
   COLcout << POPpop(LINKservConfigSeLinux) << newline;
}

void SERVselinuxCheck(){
   COL_FUNCTION(SERVselinuxCheck);
   COLstring Path;
   if (!PROwhich(&Path, "getenforce")){
      COL_TRC("getenforce is not present.");
      return;
   }
   COL_VAR(Path);
   COLstring Err, Out;
   int ExitCode = PROexecuteString(".", "getenforce", &Out, &Err);
   COL_VAR3(ExitCode, Out, Err);
   if (Out.find("Enforcing") != npos){
      SERVselinuxWarning();
   }
}
