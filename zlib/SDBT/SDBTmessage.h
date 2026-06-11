#ifndef __SDBT_MESSAGE_H__
#define __SDBT_MESSAGE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBTmessage
//
// Description:
//
// Message generation functions for SDB unit test
//
// Author: John Qi
// Date:   Wednesday 08 March 2023 - 10:52AM
// ---------------------------------------------------------------------------

#include <COL/COLauto.h>
#include <COL/COLarray.h>
#include <time.h>

class SDBmessage;

SDBmessage* SDBTcreateTestMessage(const COLstring& ComponentId, int Index = 1, int Size = 1000, time_t Date = time(NULL));
COLstring SDBTcreateSerializedTestMessage(const SDBmessage* pMessage);
COLstring SDBTcreateSerializedTestMessage(const COLstring& ComponentId, int Index = 1, int Size = 1000, time_t Date = time(NULL));

void SDBTcreateTestMessageList(COLarray<SDBmessage*>* pOut, int Count, const COLstring& ComponentId, int Index = 1, int Size = 1000, time_t Date = time(NULL));
void SDBTcreateTestMessageList(COLarray<COLauto<SDBmessage>>* pOut, int Count, const COLstring& ComponentId, int Index = 1, int Size = 1000, time_t Date = time(NULL));

#endif // end of defensive include
