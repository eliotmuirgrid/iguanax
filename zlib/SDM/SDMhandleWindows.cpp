// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDMhandle
//
// Description:
//
// Narrowly scoped RAII helper to clean up the Windows service HANDLEs.
//
// Author: John Q
// Date:   Mon 28 Aug 2023 09:02:30 PDT
// ---------------------------------------------------------------------------
#include <SDM/SDMhandleWindows.h>

#include <COL/COLerror.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

SDMhandle::~SDMhandle(){
   COL_METHOD(SDMhandle::~SDMhandle);
   COL_VAR(handle);
   if (handle == nullptr) { 
      COL_TRC("Handle was null so nothing to clean up.");
      return; 
   }
   if (!::CloseServiceHandle(handle)) {
      COLstring Err = COLlastErrorAsString();
      COL_ERR(Err);
   }
   handle = nullptr;
}
