// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITUsetup
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Wednesday 29 March 2023 - 12:23PM
// ---------------------------------------------------------------------------

#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <GITU/GITUfullDetect.h>
#include <GITU/GITUsetup.h>
COL_LOG_MODULE;

void GITUsetupImpl(const COLstring& Username, const COLaddress& Address){
   COL_FUNCTION(GITUsetupImpl);
   COLstring Error;
   if (!GITUfullDetect(Username, &Error)){
      COL_VAR(Error);
      return COLrespondError(Address, Error);
   } else {
      COL_TRC("Success");
      return COLrespondSuccess(Address);
   }
}

// /git/install/status
void GITUsetup(const COLwebRequest& Request){
   COL_FUNCTION(GITUsetup);
   const COLvar& Data = Request.Data;
   COLstring Username = Request.User;
   GITUsetupImpl(Username, Request.Address);
}
