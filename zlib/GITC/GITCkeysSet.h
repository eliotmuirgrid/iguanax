#ifndef __GITC_KEYS_SET_H__
#define __GITC_KEYS_SET_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITCkeysSet
//
// Description:
//
// Api that takes a private and public key in string form and writes them
// to the ssh key file location.
//
// Author: Vismay Shah 
// Date:   Thursday 04 December 2025 - 01:41PM
// ---------------------------------------------------------------------------
#include <COL/COLmap.h>
#include <USR/USRuser.h>
class COLstring;
class COLwebRequest;
class SCKloop;

void GITCkeysSet(const COLwebRequest& Request, COLmap<COLstring, USRuser>* pUserMap, SCKloop* pLoop);

#endif // end of defensive include
