#ifndef __NOTIFICATION_API_H__
#define __NOTIFICATION_API_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NOTIapi
//
// Description: Logic to control creating a notification
//
// Author: Matthew Sobkowski
// Date:   Tuesday Jun 1 2021
//---------------------------------------------------------------------------
#include <COL/COLweb.h>
#include <COL/COLhashmap.h>
class SCKloop;

class NOTImodel;
class BLUAinstanceManager;
struct CFGmap;

void NOTIapi(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >& ApiMap, SCKloop* pLoop, CFGmap* pMap, BLUAinstanceManager* pManager, NOTImodel* pNoti);

#endif // end of defensive include
