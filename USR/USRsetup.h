#ifndef __USR_SETUP_H__
#define __USR_SETUP_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: USRsetup
//
// Description:
//
// Initialization and shutdown routines.
//
// Author: Eliot Muir 
// Date:   Sunday 18 September 2022 - 05:01PM
// ---------------------------------------------------------------------------

#include <USR/USRmap.h>
struct PRMpermission;

COLmap<COLstring, USRuser>*  USRcreate();

void USRdestroy(COLmap<COLstring, USRuser>* * ppMap, COLmap<COLstring, PRMpermission>** ppPermissionMap);

#endif // end of defensive include
