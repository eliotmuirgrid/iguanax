#ifndef __SCK_CONNECTION_REGISTER_H__
#define __SCK_CONNECTION_REGISTER_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SCKconnectionRegister
//
// Description:
//
// Connections come and go with sockets.  This function keeps track of them.
//
// Register connection in map
//
// Author: Eliot Muir 
// Date:   Tuesday 20 June 2023 - 10:47AM
// ---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

class SCKloop;
class SCKconnection;

void SCKconnectionRegister  (SCKloop* pLoop, SCKconnection* pConnection);
void SCKconnectionUnregister(SCKloop* pLoop, COLuint64 Id);

#endif // end of defensive include