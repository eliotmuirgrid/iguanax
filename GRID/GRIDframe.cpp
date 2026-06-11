//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GRIDprotocolFramer.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLendian.h>
#include <COL/COLlog.h>
#include <GRID/GRIDframe.h>
#include <GRID/GRIDutils.h>
#include <GRID/GRIDwebSocket.h>
#include <openssl/rand.h>
COL_LOG_MODULE;

COLstring GRIDbuildJsonFrame(const COLstring& JsonPayload) {
   COL_FUNCTION(GRIDbuildJsonFrame);
   COLstring Frame;
   const COLuint16 Version = COLsystemToBigEndian(GRIDWS_VERSION);
   Frame.append((const char*)&Version, sizeof(Version));
   Frame.append(1, GRIDWS_MSG_JSON);
   const COLuint32 PayloadSize = COLsystemToBigEndian((COLuint32)JsonPayload.size());
   Frame.append((const char*)&PayloadSize, sizeof(PayloadSize));
   Frame.append(JsonPayload);
   const COLuint32 Crc = COLsystemToBigEndian(GRIDcalculateCRC32(Frame.c_str(), Frame.size()));
   Frame.append((const char*)&Crc, sizeof(Crc));
   return Frame;
}

COLstring GRIDbuildHeartbeatFrame(const COLuint64 TimestampMs, const COLstring& InstanceName) {
   COL_FUNCTION(GRIDbuildHeartbeatFrame);
   COLstring Frame;
   const COLuint16 Version = COLsystemToBigEndian(GRIDWS_VERSION);
   Frame.append((const char*)&Version, sizeof(Version));
   Frame.append(1, GRIDWS_MSG_HEARTBEAT);
   const COLuint64 Timestamp = COLsystemToBigEndian(TimestampMs);
   Frame.append((const char*)&Timestamp, sizeof(Timestamp));
   const COLuint16 InstanceNameLen = COLsystemToBigEndian((COLuint16)InstanceName.size());
   Frame.append((const char*)&InstanceNameLen, sizeof(InstanceNameLen));
   Frame.append(InstanceName);
   const COLuint32 Crc = COLsystemToBigEndian(GRIDcalculateCRC32(Frame.c_str(), Frame.size()));
   Frame.append((const char*)&Crc, sizeof(Crc));
   return Frame;
}

void GRIDencodeFrame(const void* pPayload, const int PayloadSize, const bool Mask, COLstring* pOut) {
   COL_FUNCTION(GRIDencodeFrame);
   COL_TRC("Encoding WebSocket frame, payload size=" << PayloadSize << ", mask=" << Mask);
   pOut->append(1, (char)0x82);
   if(PayloadSize <= 125) {
      pOut->append(1, (char)((Mask ? 0x80 : 0x00) | PayloadSize));
   } else if(PayloadSize <= 65535) {
      pOut->append(1, (char)((Mask ? 0x80 : 0x00) | 126));
      const COLuint16 Len16 = COLsystemToBigEndian((COLuint16)PayloadSize);
      pOut->append((const char*)&Len16, 2);
   } else {
      pOut->append(1, (char)((Mask ? 0x80 : 0x00) | 127));
      const COLuint64 Len64 = COLsystemToBigEndian((COLuint64)PayloadSize);
      pOut->append((const char*)&Len64, 8);
   }
   if(Mask) {
      COLuint8 MaskingKey[4];
      RAND_bytes(MaskingKey, 4);
      pOut->append((const char*)MaskingKey, 4);
      COLstring MaskedPayload;
      MaskedPayload.append((const char*)pPayload, PayloadSize);
      for(int i = 0; i < PayloadSize; i++) { MaskedPayload[i] ^= MaskingKey[i % 4]; }
      pOut->append(MaskedPayload);
   } else {
      pOut->append((const char*)pPayload, PayloadSize);
   }
}
