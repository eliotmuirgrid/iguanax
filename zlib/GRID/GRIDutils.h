#ifndef _GRIDwebSocket_H
#define _GRIDwebSocket_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GRIDwebSocket.h
//
//  Description: WebSocket protocol encoding/decoding functions
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
class COLstring;

void GRIDinitCRC32();
COLuint32 GRIDcalculateCRC32(const void* pData, int Size);

void GRIDgenerateWebSocketKey(COLstring* pKey);

#endif  // end of defensive include
