#ifndef __SDM_SHUTDOWN_H__
#define __SDM_SHUTDOWN_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDMshutdown
//
// Description:
//
// TODO
//
// Author: Vismay Shah 
// Date:   Monday 28 August 2023 - 02:55PM
// ---------------------------------------------------------------------------
class COLclosure0;

void SDMshutdownSet(COLclosure0* pShutdown); // takes ownership
void SDMshutdown();

#endif // end of defensive include
