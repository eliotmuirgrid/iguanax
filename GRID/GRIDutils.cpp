//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GRIDwebSocket.cpp
//
//  Description: WebSocket protocol encoding/decoding implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <GRID/GRIDwebSocket.h>
#include <openssl/buffer.h>
#include <openssl/rand.h>
COL_LOG_MODULE;

static COLuint32 GRIDwebSocketCRC32Table[256];
static bool      GRIDwebSocketCRC32TableInitialized = false;

// checksum used to detect accidental corruption in byte streams
void GRIDinitCRC32() {
   if(GRIDwebSocketCRC32TableInitialized) { return; }
   COL_TRC("Initializing CRC32 table");
   for(COLuint32 i = 0; i < 256; i++) {
      COLuint32 Crc = i;
      for(int j = 0; j < 8; j++) {
         if(Crc & 1) { Crc = (Crc >> 1) ^ 0xEDB88320; }
         else        { Crc >>= 1; }
      }
      GRIDwebSocketCRC32Table[i] = Crc;
   }
   GRIDwebSocketCRC32TableInitialized = true;
}

COLuint32 GRIDcalculateCRC32(const void* pData, const int Size) {
   COLuint32  Crc    = 0xFFFFFFFF;
   const auto pBytes = (const COLuint8*)pData;
   for(int i = 0; i < Size; i++) {
      const COLuint8 Index = (Crc ^ pBytes[i]) & 0xFF;
      Crc                  = (Crc >> 8) ^ GRIDwebSocketCRC32Table[Index];
   }
   return ~Crc;
}

void GRIDgenerateWebSocketKey(COLstring* pKey) {
   COL_FUNCTION(GRIDgenerateWebSocketKey);
   COLuint8 randomBytes[16];
   RAND_bytes(randomBytes, 16);
   BIO* pBio = BIO_new(BIO_s_mem());
   BIO* pB64 = BIO_new(BIO_f_base64());
   BIO_set_flags(pB64, BIO_FLAGS_BASE64_NO_NL);
   pBio = BIO_push(pB64, pBio);
   BIO_write(pBio, randomBytes, 16);
   BIO_flush(pBio);
   BUF_MEM* pBufferPtr;
   BIO_get_mem_ptr(pBio, &pBufferPtr);
   pKey->set(pBufferPtr->data, pBufferPtr->length);
   BIO_free_all(pBio);
}
