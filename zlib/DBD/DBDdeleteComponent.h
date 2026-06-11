#ifndef __DBD_DELETE_COMPONENT_H__
#define __DBD_DELETE_COMPONENT_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBDdeleteComponent
//
// Description:
//
// Set callback to be run when a component is deleted
//
// Author: Vismay Shah 
// Date:   Friday 06 October 2023 - 03:00PM
// ---------------------------------------------------------------------------
#include <COL/COLauto.h>
#include <COL/COLclosure.h>
#include <COL/COLstring.h>
#include <COL/COLarray.h>

class SCKloop;
class SDBsystem;

struct CFGmap;

void DBDdeleteComponent(const COLarray<COLstring>& ToDelete, const COLstring& User, COLclosure0* pCallback,
                        SCKloop* pLoop, SDBsystem* pSystem, CFGmap* pMap);

#endif // end of defensive include
