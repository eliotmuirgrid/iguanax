#ifndef __EDI_COPY_H__
#define __EDI_COPY_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: EDIcopy
//
// Description:
//
// Routine to copy a node and all it's dependencies.
//
// Author: Eliot Muir 
// Date:   Wednesday 09 October 2024 - 12:44PM
// ---------------------------------------------------------------------------

class EDIcollection;
class COLstring;

void EDIcopy(const EDIcollection& Source, EDIcollection* pDestination, const COLstring& NodeName);

#endif // end of defensive include
