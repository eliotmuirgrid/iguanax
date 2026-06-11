#ifndef __TUN_RECORD_PROCESS_H__
#define __TUN_RECORD_PROCESS_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNrecordProcess
//
// Description:
//
// Process a record
//
// Author: Eliot Muir
// Date:   Wednesday 14 June 2023 - 02:02PM
// ---------------------------------------------------------------------------

#include <TUN/TUNsetup.h>

class TUNrequest;

// returning false is a fatal error
bool TUNrecordProcess(TUNrequest* pRequest, COLhashmap<COLuint32, COLauto<COLclosure1<TUNrequestItem*>>>& HandlerTable);

#endif // end of defensive include
