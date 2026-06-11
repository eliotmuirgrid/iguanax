#ifndef __INST_UPGRADE_MAC_OS_H__
#define __INST_UPGRADE_MAC_OS_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INSTupgradeMacOs
//
// Description:
//
// Upgrade Mac OS X routine.
//
// Author: Vismay Shah 
// Date:   Monday 28 August 2023 - 10:11AM
// ---------------------------------------------------------------------------
class COLstring;

bool INSTlaunchNewController(const COLstring& AppDir);

bool INSTupgradeMacOs(const COLstring& Version, COLstring* pError);

COLstring INSTmyPackageName();

#endif // end of defensive include
