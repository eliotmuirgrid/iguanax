//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: IWEBserverSettingsTimeout.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
COL_LOG_MODULE;

static void IWEBcheckSessionTimeoutImpl(int Timeout, const COLaddress& Address) {
   COL_FUNCTION(IWEBcheckSessionTimeoutImpl);
   if(Timeout < 5)        { return COLrespondError(Address, COL_T("Session Timeout must be more than 5 minutes")); }
   if(Timeout > 35791394) { return COLrespondError(Address, COL_T("Session Timeout must be less than 35791394 minutes")); }
   return COLrespondSuccess(Address);
}

// /web/server/session_timeout
void IWEBcheckSessionTimeout(const COLwebRequest& Request) {
   COL_FUNCTION(IWEBcheckSessionTimeout);
   COL_VAR(Request.Data.json(1));
   const int Timeout = COLvarRequiredInt(Request.Data, "session_timeout");
   IWEBcheckSessionTimeoutImpl(Timeout, Request.Address);
}