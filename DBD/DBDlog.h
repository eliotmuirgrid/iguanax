#ifndef __DBD_LOG_H__
#define __DBD_LOG_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBDlog
//
// Description:
//
// Set log callback
//
// Author: Eliot Muir
// Date:   Thursday 18 May 2023 - 02:56PM
// ---------------------------------------------------------------------------
#include <COL/COLminimumInclude.h>
class BLUAinstanceManager;
class COLstring;
class LOGlogInfo;
class SCKloop;

void DBDlogMessage(const LOGlogInfo& LogInfo, const COLstring& Data, SCKloop* pLoop, BLUAinstanceManager* pManager);
#endif // end of defensive include
