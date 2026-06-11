// ---------------------------------------------------------------------------
// Copyright (C) 1997-2026 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INFOutils
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Thursday 22 January 2026 - 12:44PM
// ---------------------------------------------------------------------------
#include <INFO/INFOutils.h>
#include <INFO/INFOusage.h>

#include <ENV/ENVcatalogue.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

bool INFOapproachingFdLimit(int Threshold){
   COL_FUNCTION(INFOapproachingFdLimit);
   int CurrentFd = INFOfileHandleCount();
   int Limit     = INFOfileHandleLimit();
   COL_VAR3(CurrentFd, Limit, Threshold);
   return (Limit - CurrentFd) < Threshold;
}

void INFOsetFdLimitWrapper(){
   COL_FUNCTION(INFOsetFdLimitWrapper);
   COLvar EnvironmentVars;
   EnvironmentVars.setMapType();
   ENVretrieveAll(&EnvironmentVars);
   COL_VAR(EnvironmentVars.json(1));
   if (EnvironmentVars.exists(IFW_FD_LIMIT) && 
       EnvironmentVars[IFW_FD_LIMIT].asInt() > 256) {  // 256 is the limit for macos, the smallest limit among linux, macos and windows.
      COL_VAR(EnvironmentVars[IFW_FD_LIMIT]);
      INFOsetFdLimit(EnvironmentVars[IFW_FD_LIMIT]);
   }
}