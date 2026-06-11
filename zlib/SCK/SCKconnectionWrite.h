#ifndef __SCK_CONNECTION_WRITE_H__
#define __SCK_CONNECTION_WRITE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SCKconnectionWrite
//
// Description:
//
// Safe way to write to a socket since we don't have a pointer to the SCKconnection object.
//
// Author: Eliot Muir 
// Date:   Tuesday 20 June 2023 - 01:51PM
// ---------------------------------------------------------------------------

#include <COL/COLstring.h>

class SCKloop;


void SCKconnectionWrite(SCKloop* pLoop, COLuint64 ConnectionId, const char* pBuffer, int Size);
inline void SCKconnectionWrite(SCKloop* pLoop, COLuint64 ConnectionId, const COLstring& Data) { return SCKconnectionWrite(pLoop, ConnectionId, Data.c_str(), Data.size()); }

void SCKconnectionClose(SCKloop* pLoop, COLuint64 ConnectionId);
void SCKconnectionHardClose(SCKloop* pLoop, COLuint64 ConnectionId);

#endif // end of defensive include
