#ifndef __DBG_API_H__
#define __DBG_API_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBGapi
//
// Description:
//
// API to obtain crash dumps.
//
// Author: Eliot Muir 
// Date:   Wednesday 22 March 2023 - 11:07AM
// ---------------------------------------------------------------------------

#include <COL/COLweb.h>
#include <COL/COLhashmap.h>

void DBGapiAsync(COLhashmap<COLstring, COLwebHandlerFunc>& ApiMap);

#endif // end of defensive include
