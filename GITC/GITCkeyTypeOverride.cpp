// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITCkeyTypeOverride
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Monday 10 February 2025 - 05:56PM
// ---------------------------------------------------------------------------

#include <GITC/GITCkeyTypeOverride.h>
#include <GIT/GITgenerateKeyPair.h>

#include <COL/COLweb.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static void GITCkeyTypeOverrideImpl(const COLaddress& Address){
   COL_FUNCTION(GITCkeyTypeOverrideImpl);
   COLvar Result;
   Result = GITCkeyTypeOverride().is_null() ? "" : GITCkeyTypeOverride();
   COL_VAR(Result);
   COLrespondSuccess(Address, Result);
}

// /git/key_type/override
void GITCkeyTypeOverride(const COLwebRequest& Request){
   COL_FUNCTION(GITCkeyTypeOverride);
   GITCkeyTypeOverrideImpl(Request.Address);
}
