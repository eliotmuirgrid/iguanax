#ifndef __INST_UPGRADE_LINUX_H__
#define __INST_UPGRADE_LINUX_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INSTupgradeLinux
//
// Description:
//
// Logic to upgrade linux
//
// Author: Eliot Muir 
// Date:   Friday 25 August 2023 - 02:56PM
// ---------------------------------------------------------------------------

class COLstring;

bool INSTupgradeLinux(const COLstring& Version, COLstring* pError);

#endif // end of defensive include
