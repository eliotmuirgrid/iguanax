// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBenvelope
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Thursday 23 February 2023 - 03:08PM
// ---------------------------------------------------------------------------

#include <SFI/SFIcrc.h>
#include <SDB/SDBcrc.h>
#include <SDB/SDBenvelope.h>
#include <SDB/SDBcontent.h>
#include <SDB/SDBnumber.h>
#include <SDB/SDBmessage.h>
#include <ENVC/ENVCcache.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

// just in case SDBcrc has an issue, allow user to go back to SFIcrc
bool SDBslowCRC() {
   return ENVCgetEnv("IFW_SLOW_CRC");
}

int SDBmarkerSize() {
   return sizeof(SDB_ENVELOPE_MARKER) - 1;   // exclude the \0 byte
}

bool SDBisMarker(const char* pBuffer) {
   return 0 == strncmp(SDB_ENVELOPE_MARKER, pBuffer, SDBmarkerSize());
}

int SDBcrcSize() {
   return sizeof(COLuint32);  // 32-bit CRC
}

int SDBenvelopeSizeFromContent(SDBcontent* pContent){
   COL_FUNCTION(SDBenvelopeSizeFromContent);
   int PayloadSize = SDBcrcSize() + SDBcontentSize(pContent);
   return SDBmarkerSize() + SDBnumberSize(PayloadSize) + PayloadSize;
}

void SDBenvelopeWrite(COLstring* pBuffer, const COLstring& Content){
   COL_FUNCTION(SDBenvelopeWrite);
   COL_VAR(Content.size());
   pBuffer->append(SDB_ENVELOPE_MARKER);
   int PayloadSize = SDBcrcSize() + Content.size();
   COL_VAR(PayloadSize);
   COLstring Number;
   int AmountWritten = SDBwriteNumber(PayloadSize, Number.get_buffer());
   Number.setSize(AmountWritten);
   pBuffer->append(Number);
   // We should be using the faster SDBcrc here as well.
   // Just in case there is a bug in SDBcrc, write using slower SFIcrc for now
   // TODO: change to SDBcrc for 5x improvement in the future
   COLuint32 Checksum = SFIcrc::calculate(Content.data(), Content.size());
   COL_VAR(Checksum);
   pBuffer->append((char *)&Checksum, sizeof(COLuint32));
   pBuffer->append(Content);
}

// these functions assume a complete envelope and passed crc check
const char* SDBenvelopeContent(const char* pEnvelope, int BufferSize){
   COLuint64 Size = 0;
   const char* pPayload = pEnvelope + SDBmarkerSize();
   int Amount = SDBreadNumber(&Size, pPayload, BufferSize - SDBmarkerSize());
   if (Amount < 0) { return NULL; }
   return pPayload + Amount + SDBcrcSize();
}

COLuint64 SDBenvelopeContentSize(const char* pEnvelope, int BufferSize){
   COLuint64 Size = 0;
   const char* pPayload = pEnvelope + SDBmarkerSize();
   int Amount = SDBreadNumber(&Size, pPayload, BufferSize - SDBmarkerSize());
   if (Amount < 0) { return 0; }
   return Size - SDBcrcSize();;
}

// returns true if we have a complete envelope and false if we don't
bool SDBenvelopeComplete(const char* pBuffer, int BufferSize){
   COL_FUNCTION(SDBenvelopeComplete);
   // make sure we have enough for marker and payload_size
   if (BufferSize < SDBmarkerSize() + 5) { return false; }
   const char* pPayload = pBuffer + SDBmarkerSize();
   COLuint64 Size = 0;
   int Amount = SDBreadNumber(&Size, pPayload, BufferSize - SDBmarkerSize());
   COL_VAR3(Amount, Size, BufferSize);
   if (Amount < 0) { return false; }
   return BufferSize >= SDBmarkerSize() + Amount + Size;
}

// returns 0 if we have a valid and complete envelope
//        -1 if more data is needed
//   -ve int if errors
int SDBenvelopeInvalid(const char* pBuffer, int BufferSize){
   COL_FUNCTION(SDBenvelopeInvalid);
   // envelope structure: | marker | payload_size | CRC | content |, where payload_size = crc_size + content_size
   if (BufferSize < 2)        { return -1; } // more data
   if (!SDBisMarker(pBuffer)) { return -2; } // marker fail
   pBuffer += SDBmarkerSize();               // advance to payload_size
   BufferSize -= SDBmarkerSize();
   // payload_size is variable length, the largest content is a SDBmessage with max-sized (2^31-1) Data so max payload_size length is 5
   // also, even if we have a 1B payload_size, it's followed by 4B CRC plus content, there must be 5+ bytes for any valid envelope
   if (BufferSize < 5)        { return -1; } // more data
   COLuint64 Size = 0;
   int Amount = SDBreadNumber(&Size, pBuffer, BufferSize);
   COL_VAR(Amount);
   if (0 > Amount)            { return -2; } // read error
   COL_VAR(Size);
   if (Size > 2147483647)     { return -3; } // big size fail
   if (Size < SDBcrcSize())   { return -4; } // small size fail
   pBuffer += Amount;                        // advance to CRC
   BufferSize -= Amount;
   if (BufferSize < Size)     { return -1; } // more data
   COLuint32 Checksum = 0;
   memcpy(&Checksum, pBuffer, SDBcrcSize());
   COL_VAR(Checksum);
   pBuffer += SDBcrcSize();                  // advance to content
   BufferSize -= SDBcrcSize();
   COLuint32 Crc = SDBslowCRC() ?
      SFIcrc::calculate(pBuffer, Size - SDBcrcSize()) :
      SDBcrc::calculate(pBuffer, Size - SDBcrcSize());
   if (Checksum != Crc)       { return -5; } // crc fail
   COL_TRC("Valid");
   return 0;                                 // valid envelope
}

const char* SDBfindNextEnvelope(const char* pBuffer, int BufferSize){
   COL_FUNCTION(SDBfindNextEnvelope);
   COL_TRC_DUMP("buf", pBuffer, BufferSize);
   const char* pStart = pBuffer;
   const char* pEnd = pBuffer + BufferSize - SDBmarkerSize();
   COL_VAR3((void*)pBuffer, BufferSize, (void*)pEnd);
   if (BufferSize <= SDBmarkerSize()) {
      COL_TRC("Buffer too small to contain the next envelope's marker");
      return NULL;
   }
   do {
      pBuffer++;     // start from the 2nd byte to avoid finding the current envelope
      if (0 == memcmp(pBuffer, SDB_ENVELOPE_MARKER, SDBmarkerSize())) {
         COLASSERT(pBuffer != pStart);
         return pBuffer;
      }
   } while (pBuffer < pEnd);
   return NULL;   // not found
}

// Assume we have a complete envelope
const char* SDBenvelopeNext(const char* pBuffer, int BufferSize){
   COL_FUNCTION(SDBenvelopeNext);
   const char* pPayload = pBuffer + SDBmarkerSize();
   COLuint64 Size = 0;
   int Amount = SDBreadNumber(&Size, pPayload, BufferSize - SDBmarkerSize());
   if (Amount < 0) { return NULL; }
   if (BufferSize < SDBmarkerSize() + Amount + Size ) { return NULL; }
   return pPayload + Amount + Size;
}

COLuint64 SDBenvelopeContentType(const char* pEnvelopeCursor, int BufferSize){
   COL_FUNCTION(SDBenvelopeContentType);
   COLASSERT(SDBenvelopeComplete(pEnvelopeCursor, BufferSize));
   const char* pContent = SDBenvelopeContent(pEnvelopeCursor, BufferSize);
   COLuint64 Type;
   int Amount = SDBreadNumber(&Type, pContent, BufferSize);
   return Amount < 0 ? SDB_EOT : Type;
}

void SDBserializeContent(const SDBcontent* pContent, COLstring* pBuffer) {
   COL_FUNCTION(SDBserializeContent);
   COLstring ContentBuffer;
   ContentBuffer.setCapacity(SDBcontentSize(pContent));
   int Amount = SDBwriteContent(pContent, ContentBuffer.get_buffer(), ContentBuffer.capacity()-1);
   ContentBuffer.setSize(Amount);
   SDBenvelopeWrite(pBuffer, ContentBuffer);
}
