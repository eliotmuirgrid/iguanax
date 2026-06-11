// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBAlogQueueSources
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Monday 25 September 2023 - 02:20PM
// ---------------------------------------------------------------------------
#include <SDBU/SDBUutils.h>
#include <COL/COLlog.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <SDB/SDBcapi.h>
#include <SDBA/SDBAlogQueueSources.h>
COL_LOG_MODULE;

static void SDBAlogQueueSourcesImp(const COLstring& ComponentId, COLvar& Result){
   COL_FUNCTION(SDBAlogQueueSourcesImp);
   COL_VAR(ComponentId);
   COLarray<COLstring> Sources = SDBIgetQueueSources(ComponentId);
   Result.setArrayType();
   for (int i = 0; i < Sources.size(); ++i) {
      COLvar Temp;
      Temp["guid"] = Sources[i];
      Temp["name"] = SDBIgetComponentName(Sources[i]);
      Result.push_back(Temp);
   }
}

// /log/queue/sources
void SDBAlogQueueSources(const COLwebRequest& Request){
   COL_FUNCTION(SDBAlogQueueSources);
   COL_VAR(Request.Data.json(1));
   const COLvar& Data = Request.Data;
   COLstring ComponentId, Error;
   if(!SDBUcomponentId(COLvarGetString(Request.Data, "component", ""), &ComponentId, &Error)){ 
      return COLrespondError(Request.Address, Error); 
   }
   COLvar Result;
   SDBAlogQueueSourcesImp(ComponentId, Result);
   COLrespondSuccess(Request.Address, Result);
}