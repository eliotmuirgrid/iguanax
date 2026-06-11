#ifndef __LOGE_API_H__
#define __LOGE_API_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LOGEapi
//
// Description:
//
// Api for setting the encryption password
//
// Author: Vismay Shah 
// Date:   Tuesday 29 October 2024 - 09:42AM
// ---------------------------------------------------------------------------
#include <COL/COLhashmap.h>
#include <COL/COLweb.h>

#include <COL/COLauto.h>

class BLUAinstanceManager;
class SCKloop;

void LOGEapiMain(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>>>& ApiMap, bool* pUseEncryption, 
                 BLUAinstanceManager* pManager, SCKloop* pLoop);

#endif // end of defensive include
