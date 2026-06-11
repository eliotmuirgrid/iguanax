// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDMutils
//
// Description:
//
// Implementation
//
// Author: Vismay Shah
// Date:   Friday 01 September 2023 - 02:41PM
// ---------------------------------------------------------------------------

#include <SDM/SDMutils.h>
#include <SDM/SDMname.h>
#include <stdlib.h> //getenv

#include <COL/COLlog.h>
COL_LOG_MODULE;

bool SDMisUnderLaunchD(){
   COL_FUNCTION(SDMisUnderLaunchD);
   const char* pXPC = getenv("XPC_SERVICE_NAME");
   if (pXPC == NULL){
      COL_TRC("No XPC_SERVICE_NAME");
      return false;
   }
   if (strcmp(SDMserviceName().c_str(), pXPC) == 0){
      COL_TRC("SDMserviceName matches - we are running as service");
      return true;
   }
   return false;
}