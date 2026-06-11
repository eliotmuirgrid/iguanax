#ifndef __ACT_SERIAL_H__
#define __ACT_SERIAL_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: ACTserial
//
// Description:
//
// Utility to allow calling asyncronous actions with an event loop, in serial.
// Each action needs to call back that it is done.
//
// Author: Eliot Muir 
// Date:   Monday 19 September 2022 - 08:56AM
// ---------------------------------------------------------------------------

#include <COL/COLlist.h>
#include <COL/COLclosure.h>

class SCKloop;

class ACTactionSet{
public:
   COLlist< COLclosure1<COLclosure0* >* > List;
   COLclosure0* pDone=NULL;
};

// All closures and the ACTaction set will be destroyed by the routine.  
void ACTserial(ACTactionSet* pActions, SCKloop* pLoop);

#endif // end of defensive include
