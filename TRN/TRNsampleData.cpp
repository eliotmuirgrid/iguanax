//---------------------------------------------------------------------------
// Copyright (C) 1997-2020 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNsampleData
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Tuesday, November 16th, 2010 @ 02:58:17 PM
//---------------------------------------------------------------------------
#include "TRNsampleData.h"
#include "TRNannotationJob.h"
#include "TRNsampleGet.h"

#include <LUA/LUAutils.h>

#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void TRNsampleDataPopulate(const COLvar& Params, const COLstring& ComponentId, COLstring* pData, COLstring* pFunction){
   COL_FUNCTION(TRNsampleDataPopulate);
   COL_VAR(Params.json(1));
   if (Params.exists("sample_data_index")) {
      COL_TRC("The parameter 'sample_data_index' exists, fetching sample data");
      COLstring SampleFile, Error;
      if (Params.exists("sample_file")) { SampleFile = TRNsampleGet(ComponentId, Params["sample_file"].asString(), pData, &Error);    } 
      else                              { SampleFile = TRNsampleGet(ComponentId, Params["sample_data_index"].asInt(), pData, &Error); }
      COL_VAR(*pData);
      if(!Error.is_null()) { COL_ERR(Error); }
      if (!SampleFile.is_null()){
         COL_TRC("Check for a .meta file to see if we should call a specific function instead of main");
         TRNsampleGetMeta(SampleFile, pFunction);
      }
   } else {
      COL_TRC("The parameter 'sample_data_index' doesn't exist, or the index was -1. Setting up empty sample data struct");
      pData->clear();
   }
}