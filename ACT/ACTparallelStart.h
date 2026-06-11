#ifndef __ACT_PARALLEL_START_H__
#define __ACT_PARALLEL_START_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: ACTparallelStart
//
// Description:
//
// Simple bit of code to allow one to fire off a few threads and wait until they
// are all completed.  I use this code to start up things which can be started
// in parallel but must be complete before other actions happen.
//
// Author: Eliot Muir 
// Date:   Sunday 18 September 2022 - 12:37PM
// ---------------------------------------------------------------------------

#include <COL/COLlist.h>
#include <COL/COLclosure.h>

class SCKloop;


// COLlist can go out scope after this call is made.
// This routine will take ownership of all the COLclosure0 objects passed in.
void ACTparallelStart(SCKloop* pLoop, COLlist<COLclosure0*>* pActions, COLclosure1<const COLstring&>*  pCallback);


#endif // end of defensive include
