// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SERVworkingDir
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Wednesday 23 August 2023 - 03:15PM
// ---------------------------------------------------------------------------

#include <SERV/SERVworkingDir.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

static COLstring g_ServiceWorkingDir = "";

void SERVsetWorkingDir(const COLstring& WorkingDir){
   COL_FUNCTION(SERVsetWorkingDir);
   if(!WorkingDir.is_null()){
      COL_TRC("Setting working directory to: " + WorkingDir);
      g_ServiceWorkingDir = WorkingDir;
   }
}

COLstring SERVworkingDir(){
   COL_FUNCTION(SERVworkingDir);
   return g_ServiceWorkingDir;
}
