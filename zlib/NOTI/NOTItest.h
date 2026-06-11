#ifndef __NOTI_TEST_H__
#define __NOTI_TEST_H__
// ---------------------------------------------------------------------------
//  Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: NOTItest
//
//  Description: test notification config
//
//  Author: Matthew Sobkowski
//  Date:   8/31/22, 10:53 AM
// ---------------------------------------------------------------------------
class  COLwebRequest;
class  BLUAinstanceManager;
class  NOTImodel;
struct CFGmap;

void NOTItest(const COLwebRequest& Request, NOTImodel* pNoti, CFGmap* pMap, BLUAinstanceManager* pManager);

#endif // end of defensive include