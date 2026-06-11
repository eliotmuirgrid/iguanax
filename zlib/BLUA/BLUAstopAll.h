#ifndef __BLUA_STOPALL_H__
#define __BLUA_STOPALL_H__
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: BLUAstopAll.h
//
//  Description: Code to stop all running components -- useful if not on dashbnoard (i.e. autoupgrade)
//
//  Author: Matthew Sobkowski
//  Date:   11/01/24 12:42 PM
//  ---------------------------------------------------------------------------
class BLUAinstanceManager;
class COLwebRequest;

void BLUAcomponentStopAll(const COLwebRequest& Request, BLUAinstanceManager* pManager);

#endif // end of defensive include