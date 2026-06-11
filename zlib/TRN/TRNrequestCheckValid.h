#ifndef __TRN_REQUEST_CHECK_VALID_H__
#define __TRN_REQUEST_CHECK_VALID_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNrequestCheckValid
//
// Description:
//
// Helper function which can be run to see if a connection with the provided connection
// id still exists in the WEBserver hashmap. 
// Closure is set in DBDsetup, see WEBconnectionCheck.
//
// Author: Vismay Shah 
// Date:   Wednesday 17 May 2023 - 03:49PM
// ---------------------------------------------------------------------------

#include <COL/COLclosure.h>
#include <COL/COLminimumInclude.h>

extern COLclosure2<COLuint64, bool*>* pTRNrequestCheckValid;

bool TRNcheckRequestStillValid(COLuint64 ConnectionId);

#endif // end of defensive include
