// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SECdata
//
// Description:
//
// Author: Matthew Sobkowski
// Date: 2024-11-28
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLweb.h>
#include <SEC/SECaccess.h>
#include <SEC/SECdata.h>
COL_LOG_MODULE;

// /security/get
void SECdata(const COLwebRequest& Request) {
   COL_FUNCTION(SANDdata);
   const COLvar Out = SECtoVar();
   COL_VAR(Out.json(1));
   COLrespondSuccess(Request.Address, Out);
}
