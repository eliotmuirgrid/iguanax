#ifndef __SDB_PURGE_TEST_H__
#define __SDB_PURGE_TEST_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBpurgeTest
//
// Description:
//
// Returns which logs will be purged along with relevant information like size
// of logs freed. Also returns a list of non-component folders found in the log
// directory that will be archived during a purge.
//
// Author: Vismay Shah 
// Date:   Thursday 28 September 2023 - 01:32PM
// ---------------------------------------------------------------------------
#include <COL/COLclosure.h>
class SDBsystem;
class COLvar;

void SDBpurgeTest(SDBsystem* pSystem, COLclosure1<COLvar>* pCallback);
void SDBpurgeTestOther(SDBsystem* pSystem, COLclosure1<COLvar>* pCallback);
#endif // end of defensive include
