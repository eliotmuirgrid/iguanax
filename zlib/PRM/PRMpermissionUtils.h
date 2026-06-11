#ifndef __PRM_PERMISSION_UTILS_H__
#define __PRM_PERMISSION_UTILS_H__
#include "COL/COLarray.h"
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: PRMpermissionUtils
//
// Description:
//
// Collection of helper functions for the PRM library
//
// Author: Tyler Brown 
// Date:   Tuesday 02 January 2024 - 02:52PM
// ---------------------------------------------------------------------------
class COLstring;
struct CFGmap;
class USRuser;

#include <COL/COLmap.h>

bool PRMagentTags(bool FromTranslator, const COLstring& Agent, const COLmap<COLstring, USRuser>* pUserMap, const CFGmap* pComponentMap, COLarray<
   COLstring>* pAgentTags, COLstring* pError);
COLarray<COLstring> PRMtargetComponentTags(const CFGmap* pComponentMap, const COLstring& Target);

#endif // end of defensive include
