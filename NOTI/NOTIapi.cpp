//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NotificationApi
//
// Description:
//
// Implementation
//
// Author: Matthew Sobkowski
// Date:   Tuesday Jun 1 2021
//---------------------------------------------------------------------------
#include <NOTI/NOTIapi.h>
#include <NOTI/NOTIload.h>
#include <NOTI/NOTIsave.h>
#include <NOTI/NOTItest.h>
#include <NOTI/NOTIenabled.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void NOTIapi(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >& ApiMap, SCKloop* pLoop, CFGmap* pMap, BLUAinstanceManager* pManager, NOTImodel* pNoti) {
   COL_FUNCTION(NOTIapi);
   ApiMap.add("/notifications",         COLnewClosure1(&NOTIload,    pNoti));
   ApiMap.add("/notifications/set",     COLnewClosure1(&NOTIsave,    pNoti, pMap, pLoop));
   ApiMap.add("/notifications/test",    COLnewClosure1(&NOTItest,    pNoti, pMap, pManager));
   ApiMap.add("/notifications/enabled", COLnewClosure1(&NOTIenabled, pNoti, pMap, pManager));
}