// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBTenvelope
//
// Description:
//
// Implementation
//
// Author: John Qi
// Date:   Friday 31 March 2023 - 01:23PM
// ---------------------------------------------------------------------------

#include <SDBT/SDBTenvelope.h>
#include <SDBT/SDBTmessage.h>
#include <SDBT/SDBTpositionMap.h>

#include <SDB/SDBenvelope.h>
#include <SDB/SDBposition.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void SDBTcreateEnvelopedTestMessage(COLstring* pBuffer, const SDBmessage* pMessage) {
   COL_FUNCTION(SDBTcreateEnvelopedTestMessage);
   SDBenvelopeWrite(pBuffer, SDBTcreateSerializedTestMessage(pMessage));
}

void SDBTcreateEnvelopedTestMessage(COLstring* pBuffer, const COLstring& ComponentId, int Index, int Size, time_t Date) {
   COL_FUNCTION(SDBTcreateEnvelopedTestMessage);
   SDBenvelopeWrite(pBuffer, SDBTcreateSerializedTestMessage(ComponentId, Index, Size, Date));
}

void SDBTcreateEnvelopedTestPosition(COLstring* pBuffer, const SDBposition& Position) {
   COL_FUNCTION(SDBTcreateEnvelopedTestPosition);
   COLstring Buffer;
   Buffer.setCapacity(SDBpositionSize(&Position));
   int Amount = SDBwritePosition(&Position, Buffer.get_buffer(), Buffer.capacity()-1);
   Buffer.setSize(Amount);
   SDBenvelopeWrite(pBuffer, Buffer);
}

void SDBTcreateEnvelopedTestPositionMap(COLstring* pBuffer, const SDBpositionMap& Map) {
   COL_FUNCTION(SDBTcreateEnvelopedTestPositionMap);
   SDBenvelopeWrite(pBuffer, SDBTcreateSerializedTestPositionMap(Map));
}

void SDBTcreateEnvelopedTestPositionMap(COLstring* pBuffer, int Components, int Index, time_t Date) {
   COL_FUNCTION(SDBTcreateEnvelopedTestPositionMap);
   SDBenvelopeWrite(pBuffer, SDBTcreateSerializedTestPositionMap(Components, Index, Date));
}
