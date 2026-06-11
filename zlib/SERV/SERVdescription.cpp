// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SERVdescription
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Wednesday 23 August 2023 - 02:17PM
// ---------------------------------------------------------------------------

#include <SERV/SERVdescription.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

static COLstring g_ServiceDescription = "iNTERFACEWARE IguanaX";

void SERVsetDescription(const COLstring& Description){
   COL_FUNCTION(SERVsetDescription);
   if(!Description.is_null()){
      COL_TRC("Setting service description to: " + Description);
      g_ServiceDescription = Description;
   }
}

COLstring SERVdescription(){
   COL_FUNCTION(SERVdescription);
   return g_ServiceDescription;
}
