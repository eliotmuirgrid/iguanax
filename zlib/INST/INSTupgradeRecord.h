#ifndef __INST_UPGRADE_RECORD_H__
#define __INST_UPGRADE_RECORD_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INSTupgradeRecord
//
// Description:
//
// TODO
//
// Author: Vismay Shah 
// Date:   Wednesday 08 January 2025 - 12:28PM
// ---------------------------------------------------------------------------
class COLstring;

bool INSTcreateUpgradeRecord(COLstring* pError);
void INSThandleUpgradeRecord();

#endif // end of defensive include
