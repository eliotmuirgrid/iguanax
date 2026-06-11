#ifndef __TRNC_CLEAR_QUEUE_H__
#define __TRNC_CLEAR_QUEUE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: TRNCclearQueue
//
// Author: John Q
//
// Clears messages in a specific component's queue
//---------------------------------------------------------------------------
class COLwebRequest;
struct CFGmap;

void TRNCcomponentClearQueue(const COLwebRequest& Request, CFGmap* pMap);

#endif