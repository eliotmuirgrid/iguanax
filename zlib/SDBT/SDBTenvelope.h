#ifndef __SDBT_ENVELOPE_H__
#define __SDBT_ENVELOPE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBTenvelope
//
// Description:
//
// Test functions for envelope
//
// Author: John Qi
// Date:   Friday 31 March 2023 - 01:23PM
// ---------------------------------------------------------------------------

#include <time.h>

class COLstring;
class SDBmessage;
class SDBposition;
class SDBpositionMap;

void SDBTcreateEnvelopedTestMessage    (COLstring* pBuffer, const SDBmessage* pMessage);
void SDBTcreateEnvelopedTestMessage    (COLstring* pBuffer, const COLstring& ComponentId, int Index = 1, int Size = 1000, time_t Date = time(NULL));
void SDBTcreateEnvelopedTestPosition   (COLstring* pBuffer, const SDBposition& Position);
void SDBTcreateEnvelopedTestPositionMap(COLstring* pBuffer, const SDBpositionMap& Map);
void SDBTcreateEnvelopedTestPositionMap(COLstring* pBuffer, int Components = 4, int Index = 1, time_t Date = time(NULL));

#endif // end of defensive include
