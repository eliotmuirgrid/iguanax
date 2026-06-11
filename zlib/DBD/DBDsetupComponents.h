#ifndef __DBD_SETUP_COMPONENTS_H__
#define __DBD_SETUP_COMPONENTS_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBDsetupComponents
//
// Description:
//
// This routine loads components.  It's blocking and should be run on worker thread.
//
// Author: Eliot Muir 
// Date:   Wednesday 21 September 2022 - 07:03PM
// ---------------------------------------------------------------------------

#include <CFG/CFGmap.h>

class IWEBconfig;

void DBDsetupComponents(CFGmap* pMap, IWEBconfig* pWebConfig);

#endif // end of defensive include
