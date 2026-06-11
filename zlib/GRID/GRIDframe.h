#ifndef __GRIDFRAME__
#define __GRIDFRAME__
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GRIDprotocolFramer.h
//
//  Description: Handles GRID protocol frame building and parsing
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLstring.h>

enum GRIDwebSocketFrameType {
   GRIDWS_MSG_HEARTBEAT      = 0x01,
   GRIDWS_MSG_JSON           = 0x02,
   GRIDWS_MSG_AUTH_CHALLENGE = 0x03,
   GRIDWS_MSG_AUTH_RESPONSE  = 0x04,
   GRIDWS_MSG_AUTH_SUCCESS   = 0x05
};

struct GRIDwebSocketHeartbeatFrame {
   COLuint8  m_Type;
   COLuint64 m_TimeStampMs;
   COLstring m_InstanceName;
};

COLstring GRIDbuildJsonFrame(const COLstring& JsonPayload);
COLstring GRIDbuildHeartbeatFrame(COLuint64 TimestampMs, const COLstring& InstanceName);
void GRIDencodeFrame(const void* pPayload, int PayloadSize, bool Mask, COLstring* pOut);

#endif
