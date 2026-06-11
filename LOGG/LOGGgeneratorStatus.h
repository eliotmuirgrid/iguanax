#ifndef __LOGG_GENERATOR_STATUS_H__
#define __LOGG_GENERATOR_STATUS_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LOGGgeneratorStatus
//
// Description:
//
// POD status object for the log generator. Functions to populate members of 
// the structure and one to convert it to COLvar for web queries.
//
// Author: Vismay Shah
// Date:   Wednesday 20 September 2023 - 04:34PM
// ---------------------------------------------------------------------------
#include <COL/COLstring.h>
class COLvar;

class LOGGeneratorStatus{ 
public:
   bool DownloadDone   = false;
   bool GenerationDone = false;
   int CurrentMonths   = 0;
   COLstring Error;
};

void LOGGeneratorStatusReset();
void LOGGstatusVar(COLvar* pOut);

// These functions update the LOGGeneratorStatus on the main thread
void LOGGreportError(COLstring Error);
void LOGGdownloadDone();
void LOGGgenerationDone();
void LOGGgenerationMonthDone();

#endif // end of defensive include
