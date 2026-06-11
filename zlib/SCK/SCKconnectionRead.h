#ifndef __SCK_CONNECTION_READ_H__
#define __SCK_CONNECTION_READ_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SCKconnectionRead
//
// Description:
//
// Get data from socket
//
// Author: Eliot Muir 
// Date:   Tuesday 20 June 2023 - 02:13PM
// ---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

class COLstring;
class SCKloop;

void SCKconnectionRead(SCKloop* pLoop, COLuint64 ConnectionId, COLstring** ppInputBuffer);

#endif // end of defensive include
