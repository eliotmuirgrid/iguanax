// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITWparse
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Monday 02 October 2023 - 04:26PM
// ---------------------------------------------------------------------------

#include <GITW/GITWparse.h>
#include <GSV/GSVlink.h>

#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

// /git/url/parse
void GITWparse(const COLwebRequest& Request){
   COL_FUNCTION(GITWparse);
   const COLvar& Data = Request.Data;
   COL_VAR(Data);
   COLstring Content = COLvarRequiredString(Data, "url");
   COLvar Result;
	GSVlink Link;
   Result["valid"] = GSVconstructLink(Content, &Link);
	Result["links"] = GSVlinkToVar(Link);
   COLrespondSuccess(Request.Address, Result);
}