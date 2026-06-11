// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DRXrate
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Wednesday 24 January 2024 - 12:29PM
// ---------------------------------------------------------------------------

#include <DRX/DRXrate.h>
#include <DRX/DRXutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

static int DRXpushCount = 0;

static int DRXcount(){ 
   // COL_FUNCTION(DRXcount);
   return DRXpushCount; 
}

void DRXincrementCount(){
   // COL_FUNCTION(DRXincrementCount);
   DRXpushCount++;
}

void DRXresetCount(){
   // COL_FUNCTION(DRXresetCount);
   DRXpushCount = 0;
}

bool DRXpushCountOverLimit(){
   COL_FUNCTION(DRXpushCountOverLimit);
   return false; // IX-2935 Enable if we turn on message rate
   // int LicenseLimit = DRXmessageRateLimit();
   // if (0 == LicenseLimit) {
   //    COL_TRC("No limit on queue push per minute");
   //    return false;
   // }
   // COL_VAR2(DRXcount(), LicenseLimit);
   // return DRXcount() >= LicenseLimit;
}