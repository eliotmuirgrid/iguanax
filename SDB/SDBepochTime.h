#ifndef __SDB_EPOCH_TIME_H__
#define __SDB_EPOCH_TIME_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBepochTime
//
// Description:
//
// Experiment in trying out formatting a file path using hex with unix epoch time.
//
// Author: Eliot Muir
// Date:   Sunday 02 April 2023 - 09:47AM
// ---------------------------------------------------------------------------

#include <time.h>
#include <COL/COLminimumInclude.h>

class COLstring;

// returns the current epoch timestamp in milliseconds
COLuint64 SDBtime();

COLstring SDBepochTimePath(time_t Time);
time_t SDBepochTime(const COLstring& EpochTimePath);

void SDBformatHexByte(int Byte, COLstring* pOut);

bool SDBepochRollover(time_t Current, time_t Base);

#endif // end of defensive include
