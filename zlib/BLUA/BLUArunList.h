#ifndef __BLUA_RUN_LIST_H__
#define __BLUA_RUN_LIST_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: BLUArunList
//
// Description:
//    APIs to get list of running components
//
// Author: Matthew Nitsopoulos
// Date:   Tuesday August 23 2022
//---------------------------------------------------------------------------
class BLUAinstanceManager;
class COLwebRequest;

void BLUArunList(const COLwebRequest& Request, BLUAinstanceManager* pBluaManager);

#endif // end of defensive include