//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: BLUArunList
//
// Description:
//    APIs to get list of running components
//    Implementation
//
// Author: Matthew Nitsopoulos
// Date:   Tuesday August 23 2022
//---------------------------------------------------------------------------
#include "BLUArunList.h"
#include <BLUA/BLUAmanager.h>

#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLlog.h>
#include <COL/COLweb.h>
COL_LOG_MODULE;

static void BLUArunListImp(COLvar* pResult, BLUAinstanceManager* pBluaManager) {
   COL_FUNCTION(BLUArunListImp);
   pResult->setMapType();
   for (auto i = pBluaManager->m_Instances.begin(); i != pBluaManager->m_Instances.end(); ++i) {
      (*pResult)[i->first] = true;
   }
   COL_TRC(pResult->size() << " components running");
}

// /component/run_list
void BLUArunList(const COLwebRequest& Request, BLUAinstanceManager* pBluaManager) {
   COL_FUNCTION(BLUArunList);
   COLvar Result;
   BLUArunListImp(&Result, pBluaManager);
   COL_VAR(Result.json(1));
   COLrespondSuccess(Request.Address, Result);
}