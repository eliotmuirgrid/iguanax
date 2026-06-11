#ifndef __LOGE_ENCRYPTION_SET_H__
#define __LOGE_ENCRYPTION_SET_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LOGEencryptionSet
//
// Description:
//
// Api to set log encryption on or off
//
// Author: Vismay Shah 
// Date:   Tuesday 29 October 2024 - 01:36PM
// ---------------------------------------------------------------------------
#include <COL/COLweb.h>
class BLUAinstanceManager;
class SCKloop;

// this is the web api handler
void LOGEencryptSet(const COLwebRequest& Request, bool* pUseEncryption, BLUAinstanceManager* pManager, SCKloop* pLoop);

#endif // end of defensive include
