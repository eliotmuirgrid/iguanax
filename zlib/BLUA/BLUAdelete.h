#ifndef __BLUA_DELETE_H__
#define __BLUA_DELETE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: BLUAdelete
//
// Description:
//    APIs to delete components
//
// Author: Vismay Shah by Nits
// Date:   Friday May 14 2021
//---------------------------------------------------------------------------
class COLwebRequest;
class BLUAinstanceManager;
struct CFGmap;

void BLUAdeleteComponent(const COLwebRequest& Request, BLUAinstanceManager* pBluaManager, CFGmap* pMap);

#endif // end of defensive include