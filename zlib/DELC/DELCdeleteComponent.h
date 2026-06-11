#ifndef __DELC_DELETE_COMPONENT_H__
#define __DELC_DELETE_COMPONENT_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DELCdeleteComponent
//
// Description:
//
// Library storing closure to be run when a component is deleted.
//
// Author: Vismay Shah 
// Date:   Friday 06 October 2023 - 03:20PM
// ---------------------------------------------------------------------------
#include <COL/COLauto.h>
#include <COL/COLclosure.h>
#include <COL/COLstring.h>
#include <COL/COLarray.h>

void DELCsetOnComponentDelete(
    COLauto<COLclosure3<const COLarray<COLstring>&, const COLstring&, COLclosure0*>> pCallback);
void DELCrunComponentDelete(const COLarray<COLstring>& ToDelete, const COLstring& User, COLclosure0* pCallback);

#endif // end of defensive include
