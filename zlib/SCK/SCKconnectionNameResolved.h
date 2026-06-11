#ifndef __SCK_CONNECTION_NAME_RESOLVED_H__
#define __SCK_CONNECTION_NAME_RESOLVED_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SCKconnectionNameResolved
//
// Description:
//
// Callback for resolving a host name for a connect operation
//
// Author: Eliot Muir 
// Date:   Tuesday 20 June 2023 - 11:03AM
// ---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

class SCKaddress;
class SCKloop;

void SCKconnectionNameResolved(const SCKaddress& Address, SCKloop* pLoop, COLuint64 ConnectionId);

#endif // end of defensive include
