#ifndef __GIT_CLONE_PERMISSION_H__
#define __GIT_CLONE_PERMISSION_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITclonePermission
//
// Description:
//
// TODO
//
// Author: Tyler Brown 
// Date:   Friday 05 May 2023 - 12:50PM
// ---------------------------------------------------------------------------

class COLstring;

bool GITclonePermission(const COLstring& Username, const COLstring& GitUrl, COLstring* pError);

#endif // end of defensive include
