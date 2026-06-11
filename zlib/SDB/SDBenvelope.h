#ifndef __SDB_ENVELOPE_H__
#define __SDB_ENVELOPE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBenvelope
//
// Description:
//
// We write an envelope around each message - that has the information about
// the size at the start and the end - to enable forward and backward navigation.
//
// Envelope format:
//     | marker | payload_size | CRC | message |
// Where payload_size = checksum size + message size
//
// Author: Eliot Muir
// Date:   Thursday 23 February 2023 - 03:08PM
// ---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

class SDBmessage;
class SDBcontent;
class COLstring;

#define SDB_ENVELOPE_MARKER ":)"

// useful for allocation a large enough buffer for SDBwriteEnvelope()
int SDBenvelopeSizeFromContent(SDBcontent* pContent);
void SDBenvelopeWrite(COLstring* pBuffer, const COLstring& Content);

// these functions assume a complete envelope
const char* SDBenvelopeContent(const char* pEnvelope, int BufferSize);
COLuint64   SDBenvelopeContentSize(const char* pEnvelope, int BufferSize);

bool SDBenvelopeComplete(const char* pBuffer, int BufferSize);
// returns 0 if we have a valid and complete envelope
//        -1 if more data is needed
//   -ve int if errors
int SDBenvelopeInvalid(const char* pBuffer, int BufferSize);
// used to search buffer for the next envelope when current envelope is corrupted
const char* SDBfindNextEnvelope(const char* pBuffer, int BufferSize);
// Assumes complete envelope
// return the buffer position of next envelope assuming the current envelope is valid
const char* SDBenvelopeNext(const char* pBuffer, int BufferSize);

// Reads the number from the envelope in a safe way
// By safe we mean that it acknowledges the assumptions of the envelope design with respect to numbers
// returns SDB_EOT on error
COLuint64 SDBenvelopeContentType(const char* pEnvelopeCursor, int BufferSize);

void SDBserializeContent(const SDBcontent* pContent, COLstring* pBuffer);

// unit test
int SDBmarkerSize();
bool SDBisMarker(const char* pBuffer);
int SDBcrcSize();

#endif // end of defensive include