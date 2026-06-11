#ifndef __SDBA_RESUBMIT_CUSTOM_H__
#define __SDBA_RESUBMIT_CUSTOM_H__
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SDBAresubmitCustom
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   19/12/24 10:21 AM
//  ---------------------------------------------------------------------------

class BLUAinstancemanager;
struct CFGmap;
class COLwebReqeust;
class SCKloop;

void SDBAresubmitCustom(const COLwebRequest& Request, CFGmap* pMap, BLUAinstanceManager* pManager, SCKloop* pLoop);

#endif // end of defensive include
