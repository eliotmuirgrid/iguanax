// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDMshutdown
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Monday 28 August 2023 - 02:55PM
// ---------------------------------------------------------------------------
#include <SDM/SDMshutdown.h>

#include <COL/COLauto.h>
#include <COL/COLclosure.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static COLauto<COLclosure0> pSDMserverShutdown = NULL;

void SDMshutdownSet(COLclosure0* pShutdown){
   COL_FUNCTION(SDMshutdownSet);
   pSDMserverShutdown = pShutdown;
}

void SDMshutdown(){
   COL_FUNCTION(SDMshutdown);
   if (pSDMserverShutdown){
      pSDMserverShutdown->run();
   } else {
      COL_ERR("SDM Server shutdown closure not set");
   }
}

