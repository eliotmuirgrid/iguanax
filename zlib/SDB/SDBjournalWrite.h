#ifndef __SDB_JOURNAL_WRITE_H__
#define __SDB_JOURNAL_WRITE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBjournalWrite
//
// Description:
//
// Everything related to writing to journal file.
//
// Author: John Qi
// Date:   Tuesday 25 April 2023 - 05:09PM
// ---------------------------------------------------------------------------

#include <COL/COLclosure.h>

class COLstring;
class SDBsystem;

void SDBwriteJournalFile(int& FD, const COLstring* pData, const COLstring& EncryptionKey, const COLstring& LogRootDir);

void SDBwriteJournal(SDBsystem* pSystem, COLclosure0* pSwapped);

#endif // end of defensive include
