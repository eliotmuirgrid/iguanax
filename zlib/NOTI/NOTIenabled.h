#ifndef __NOTI_ENABLED_H__
#define __NOTI_ENABLED_H__
// ---------------------------------------------------------------------------
//  Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: NOTIenabled.h
//
//  Description: Set notifications on/off
//
//  Author: Matthew Nitsopoulos
//  Date:   9/3/22, 10:53 AM
// ---------------------------------------------------------------------------
class COLwebRequest;
class BLUAinstanceManager;
class NOTImodel;
struct CFGmap;

void NOTIenabled(const COLwebRequest& Reqeust, NOTImodel* pNoti, CFGmap* pMap, BLUAinstanceManager* pManager);

#endif // end of defensive include