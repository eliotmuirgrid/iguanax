// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DOCsourceQuery
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Thursday 11 September 2025 - 10:53AM
// ---------------------------------------------------------------------------

#include <DOC/DOCsourceQuery.h>

#include <BLUA/BLUAmanager.h>
#include <BLUA/BLUAcall.h>

#include <COL/COLweb.h>
#include <COL/COLvarUtils.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void DOCsourceQueryResult(COLvar Result, COLaddress Address){
   COLrouteResponse(Result[0], Address);
}

void DOCsourceQuery(const COLwebRequest& Request, BLUAinstanceManager* pManager){
   COL_FUNCTION(DOCsourceQuery);
   COLstring Id = COLvarRequiredString(Request.Data, "component");


   // TODO - we can look at the configuration to see if it is available.
   if (pManager->m_Instances.find(Id) == pManager->m_Instances.cend()){
      return COLrespondError(Request.Address, "Component is not running: " + Id);
   }
   COLvar CallData = Request.Data;
   CallData.erase("component");
   BLUAcallbackCall(pManager->m_Instances[Id].get(), "DOC_source", CallData, COLnewClosure1(&DOCsourceQueryResult, Request.Address));
   return;
}