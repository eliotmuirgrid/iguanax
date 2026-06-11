// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDMname
//
// Description:
//
// Service name for IguanaX.
//
// Author: Eliot Muir
// Date:   Thursday 22 September 2022 - 03:44PM
// ---------------------------------------------------------------------------

#include <COL/COLlog.h>
COL_LOG_MODULE;

#ifdef __APPLE__
   #define SDM_DEFAULT_SERVICE_NAME "com.interfaceware.IguanaX"
#else
   #define SDM_DEFAULT_SERVICE_NAME "IguanaX"
#endif

static COLstring g_ServiceName = SDM_DEFAULT_SERVICE_NAME;

void SDMsetServiceName(const COLstring& Name){
   COL_FUNCTION(SDMsetServiceName);
   if(!Name.is_null()){
      COL_TRC("Setting service name to: " + Name);
      g_ServiceName = Name;
   }
}

COLstring SDMserviceName(){
   COL_FUNCTION(SDMserviceName);
   return g_ServiceName;
}
