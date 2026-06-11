#ifndef __DRX_RATE_H__
#define __DRX_RATE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DRXrate
//
// Description:
//
// Iguana-wide counter for limiting message throughput
//
// Author: Vismay Shah 
// Date:   Wednesday 24 January 2024 - 12:29PM
// ---------------------------------------------------------------------------

void DRXincrementCount();
void DRXresetCount();
bool DRXpushCountOverLimit();

#endif // end of defensive include
