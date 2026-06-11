#ifndef __LOGG_GENERATOR_H__
#define __LOGG_GENERATOR_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LOGGgenerator
//
// Description:
//
// The log_generator tool incorporated into Iguana. Generates log messages
// 1 kb in size using the public works of Shakespeare. Occasionally, a magic string
// is appended to the logs.
// The following is configurable:
//  * The number of months of log data generated
//  * The number of log messages per day generated
//  * The rarity of the magic string
//
// Author: Vismay Shah
// Date:   Wednesday 20 September 2023 - 03:24PM
// ---------------------------------------------------------------------------
class COLwebRequest;
class COLvar;
class SDBsystem;
class SCKloop;

void LOGGgeneratorStart (const COLwebRequest& Request, SDBsystem* pLogQueue, SCKloop* pLoop);
void LOGGgeneratorStatus(const COLwebRequest& Request);

// unit test
void LOGGeneratorWorker(COLvar Args, SDBsystem* pLogQueue, SCKloop* pLoop);

#endif // end of defensive include
