// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DELCdeleteComponent
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Friday 06 October 2023 - 03:20PM
// ---------------------------------------------------------------------------

#include <DELC/DELCdeleteComponent.h>

#include <COL/COLthreadPool.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static COLauto<COLclosure3<const COLarray<COLstring>&, const COLstring&, COLclosure0*>> DELConComponentDelete;

void DELCsetOnComponentDelete(
    COLauto<COLclosure3<const COLarray<COLstring>&, const COLstring&, COLclosure0*>> pCallback) {
   COL_FUNCTION(DELCsetOnComponentDelete);
   DELConComponentDelete = pCallback;
}

void DELCrunComponentDelete(const COLarray<COLstring>& ToDelete, const COLstring& User, COLclosure0* pCallback) {
   COL_FUNCTION(DELCrunComponentDelete);
   if (DELConComponentDelete.get()){
      COL_TRC("Running component delete callback");
      DELConComponentDelete->run(ToDelete, User, pCallback);
   } else {
      COL_TRC("No component delete Callback");
   }
}