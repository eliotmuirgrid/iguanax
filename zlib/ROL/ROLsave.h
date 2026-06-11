#ifndef __ROL_SAVE_H__
#define __ROL_SAVE_H__
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: ROLsave.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   25/01/24 2:27 PM
//  ---------------------------------------------------------------------------
#include <COL/COLauto.h>
#include <COL/COLclosure.h>

class COLstring;
class COLvar;
class ROLmap;
class SCKloop;

void ROLsave(const COLstring& User, const COLvar& RoleMapVar, const COLstring& File, SCKloop* pLoop,
							  COLauto<COLclosure1<COLvar>> pCallback);

#endif // end of defensive include
