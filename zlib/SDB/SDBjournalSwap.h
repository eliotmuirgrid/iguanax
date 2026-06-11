#ifndef __SDB_JOURNAL_SWAP_H__
#define __SDB_JOURNAL_SWAP_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBjournalSwap
//
// Description:
//
// Everything related to swapping journal file.
//
// Author: John Qi
// Date:   Tuesday 25 April 2023 - 05:09PM
// ---------------------------------------------------------------------------

#include <COL/COLclosure.h>
#include <COL/COLhashmap.h>
#include <SDB/SDBid.h>

class COLstring;
class SDBsystem;
class SDBpositionMap;

void SDBreplaceJournalFile(int& FD, const SDBpositionMap* pPositionMap, const COLstring& EncryptionKey, const COLstring& LogRootDir);

void SDBswapJournal(SDBsystem* pSystem, COLclosure0* pSwapped);

void SDBsetUpJournalFile(const COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>* pPositionMap, const COLstring& EncryptionKey, const COLstring& LogRootDir);

#endif // end of defensive include
