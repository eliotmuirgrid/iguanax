// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: WEBcheckIo
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Thursday 12 January 2023 - 11:39AM
// ---------------------------------------------------------------------------

#include <WEB/WEBcheckIo.h>

#include <COL/COLioCheck.h>
#include <COL/COLutils.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void WEBcheckCall(const COLvar& Input){
   //COL_FUNCTION(WEBcheckCall);
   COLstring Location = Input["location"];
   COL_VAR(Location);
   if (getenv("CHM_WEB_ERR")){
      if (COLglobMatch(getenv("CHM_WEB_ERR"), Location.c_str())){
         COL_ERR("Simulating API error for " << Location << newline << Input);
         COL_ERR("CHM_WEB_ERR=" << getenv("CHM_WEB_ERR"));
         COL_ERROR_STREAM_PLAIN("Location " << Location << " broken with CHM_WEB_ERR" << getenv("CHM_WEB_ERR"), 0);
      }
   }
}