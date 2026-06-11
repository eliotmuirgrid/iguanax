#ifndef __BUILD_WATCH_H__
#define __BUILD_WATCH_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: BUILDwatch
//
// Description: Function to detect what the latest state of the GIT repo is that needs to be built.
//
// Author: Eliot Muir
// Date:   Tue 17 Aug 2021 14:33:50 EDT
//---------------------------------------------------------------------------
class COLstring;

void BUILDwatch(const COLstring& Dir, COLstring* pGuid, COLstring* pDescription);
void BUILDloop(const COLstring& Dir);

#endif
