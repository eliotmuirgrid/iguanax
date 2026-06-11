#ifndef __EDI_PRUNE_H__
#define __EDI_PRUNE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: EDIprune
//
// Description:
//
// Routine which checks the top level message nodes and finds all the unused
// nodes and removes them.
//
// Author: Eliot Muir 
// Date:   Thursday 10 October 2024 - 09:30AM
// ---------------------------------------------------------------------------

class EDIcollection;

void EDIprune(EDIcollection* pCollection);

#endif // end of defensive include
