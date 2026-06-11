#ifndef __INST_UPGRADE_H__
#define __INST_UPGRADE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INSTupgrade
//
// Description:
//
// Routines to install new version
//
// Author: Eliot Muir 
// Date:   Sunday 23 July 2023 - 02:21PM
// ---------------------------------------------------------------------------
class COLstring;
class COLwebRequest;
class DBDworld;
class SCKloop;

// This one runs on main thread as it can potentially modify pWorld
void INSTupgradeReplace(const COLwebRequest& Request, SCKloop* pLoop, DBDworld* pWorld);
// This one runs on worker thread
bool INSTreplace(const COLstring& Guid, COLstring* pError);

void INSTrunAt(const COLstring& Command);

void INSTlogUpgradeStartFailure();
void INSTlogUpgradeStartSuccess();

#endif // end of defensive include