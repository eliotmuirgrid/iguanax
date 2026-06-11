#ifndef __TUN_RECORD_H__
#define __TUN_RECORD_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNrecord
//
// Description:
//
// Routines to write and read records.
//
// Author: Eliot Muir 
// Date:   Wednesday 14 June 2023 - 11:44AM
// ---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

class COLstring;
class TUNrequestItem;
class SCKloop;

// Write a record of data.  Data records are prefixed with the length of the data and type.
// These numbers are written in variable length encoding using TUNnumber.  This means small
// messages and types in the range of 0-127 only take one byte.  But the system scales up
// to 2^32-1 types of records.
void TUNrecordWrite(TUNrequestItem* pOut, COLuint32 Type, const char* pData, COLuint32 SizeOfData);
void TUNrecordWrite(TUNrequestItem* pOut, COLuint32 Type, const COLstring& Data);

void TUNrecordWriteString(COLstring* pOut, COLuint32 Type, const char* pData, COLuint32 SizeOfData);
void TUNrecordWriteString(COLstring* pOut, COLuint32 Type, const COLstring& Data);

void TUNrecordConnectionWrite(SCKloop* pLoop, COLuint64 ConnectionId, COLuint32 Type, const COLstring& Data);

int TUNrecordRead(const unsigned char* pBuffer, int SizeOfBuffer, COLuint32* pType, COLstring* pRecord);

void TUNtruncate(COLstring* pBuffer, int ChunkToRemove);

#endif // end of defensive include
