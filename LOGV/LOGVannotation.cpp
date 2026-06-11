//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: LOGVannotation.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   23/04/25 1:35 PM
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLweb.h>
#include <LOGV/LOGVannotation.h>
#include <LOGV/LOGVstate.h>
#include <TRN/TRNannotationState.h>
COL_LOG_MODULE;

// /log/viewer/annotation
void LOGVannotation(const COLwebRequest& Request) {
   COL_FUNCTION(LOGVannotation);
   COL_VAR(Request.Data.json(1));
   TRNannotationState State;
   COLstring          Err;
   COLvar             Result;
   bool Success = LOGVinitState(Request.Data, &State, &Result, &Err);
   COL_VAR3(Success, Err, Result.json(1));
   Success ? COLrespondSuccess(Request.Address, Result) : COLrespondError(Request.Address, Err);
}