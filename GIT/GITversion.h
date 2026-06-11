#ifndef __GIT_VERSION_H__
#define __GIT_VERSION_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITversion
//
// Description:
//
// C API to detect the version of GIT installed
//
// Author: Orkhan Amikishiyev 
// Date:   Thursday 02 February 2023 - 05:50AM
// ---------------------------------------------------------------------------
class COLstring;

bool GITversion(COLstring* pVersion, COLstring* pVersionError);


#endif // end of defensive include