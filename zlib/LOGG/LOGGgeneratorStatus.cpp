// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LOGGgeneratorStatus
//
// Description:
//
// Implementation
//
// Author: Vismay Shah
// Date:   Wednesday 20 September 2023 - 04:34PM
// ---------------------------------------------------------------------------

#include <LOGG/LOGGgeneratorStatus.h>

#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static LOGGeneratorStatus GeneratorStatus;
LOGGeneratorStatus* LOGGgetGeneratorStatus(){ return &GeneratorStatus; }

void LOGGstatusVar(COLvar* pOut){
   COL_FUNCTION(LOGGstatusVar);
   COLvar& Out = *pOut;
   LOGGeneratorStatus* pState = LOGGgetGeneratorStatus();
   Out["download"]   = pState->DownloadDone;
   Out["generation"] = pState->GenerationDone;
   Out["progress"]   = pState->CurrentMonths;
   Out["error"]      = pState->Error;
   Out["complete"]   = pState->DownloadDone && pState->GenerationDone;
}

void LOGGeneratorStatusReset(){
   COL_FUNCTION(LOGGeneratorStatusReset);
   LOGGeneratorStatus* pState = LOGGgetGeneratorStatus();
   pState->CurrentMonths  = 0;
   pState->DownloadDone   = false;
   pState->GenerationDone = false;
   pState->Error = "";
}

void LOGGreportError(COLstring Error){
   COL_FUNCTION(LOGGreportError);
   LOGGeneratorStatus* pState = LOGGgetGeneratorStatus();
   pState->Error = Error;
}

void LOGGdownloadDone(){
   COL_FUNCTION(LOGGdownloadDone);
   LOGGeneratorStatus* pState = LOGGgetGeneratorStatus();
   pState->DownloadDone = true;
}

void LOGGgenerationDone(){
   COL_FUNCTION(LOGGgenerationDone);
   LOGGeneratorStatus* pState = LOGGgetGeneratorStatus();
   pState->GenerationDone = true;
}

void LOGGgenerationMonthDone(){
   COL_FUNCTION(LOGGgenerationMonthDone);
   LOGGeneratorStatus* pState = LOGGgetGeneratorStatus();
   pState->CurrentMonths++;
}
