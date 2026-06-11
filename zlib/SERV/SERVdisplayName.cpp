// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SERVdisplayName
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Wednesday 23 August 2023 - 02:07PM
// ---------------------------------------------------------------------------

#include <SERV/SERVdisplayName.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

static COLstring g_ServiceDisplayName = "iNTERFACEWARE IguanaX";

void SERVsetDisplayName(const COLstring& DisplayName){
   COL_FUNCTION(SERVsetDisplayName);
   if(!DisplayName.is_null()){
      COL_TRC("Setting service display name to: " + DisplayName);
      g_ServiceDisplayName = DisplayName;
   }
}

COLstring SERVdisplayName(){
   COL_FUNCTION(SERVdisplayName);
   return g_ServiceDisplayName;
}
