// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: IWEBsetup
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Monday 19 September 2022 - 01:49PM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <IWEB/IWEBconfig.h>
#include <IWEB/IWEBsetup.h>
COL_LOG_MODULE;

void IWEBcreate(IWEBconfig** ppConfig) {
   COL_FUNCTION(IWEBcreate);
   *ppConfig = new IWEBconfig();
}
void IWEBdestroy(IWEBconfig** ppConfig) {
   COL_FUNCTION(IWEBcreate);
   delete *ppConfig;
   *ppConfig = NULL;
}