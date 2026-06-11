#ifndef __SDM_MANAGE_H__
#define __SDM_MANAGE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDMmanage
//
// Description:
//
// New granular API to manage services/launchd/systemd stuff - cross platform
// primitives.
//
// Author: Eliot Muir
// Date:   Wednesday 12 July 2023 - 03:18PM
// ---------------------------------------------------------------------------

class COLstring;

// Would return false if we don't have permissions to do this.
bool SDMserverExe    (const COLstring& Name, COLstring* pExe,  COLstring* pError);
bool SDMserverUser   (const COLstring& Name, COLstring* pUser, COLstring* pError);
bool SDMserverExists (const COLstring& Name, bool* pExists,    COLstring* pError);
bool SDMserverRunning(const COLstring& Name, bool* pRunning,   COLstring* pError);

// With apple the way we have set things up we either have Iguana running as a launchd server
// or it's dead.
#ifndef __APPLE__
bool SDMserverStart  (const COLstring& Name, COLstring* pError);
bool SDMserverStop   (const COLstring& Name, COLstring* pError);
#endif

bool SDMserverRemove (const COLstring& Name, COLstring* pError);  // Remove is like uninstall

// Because for some platforms AutoStart being true means the service starts immediately like Mac OS X.
// We should make the behavior consistent on all of them for that matter.
bool SDMserverInstall(const COLstring& Name, const COLstring& DisplayName, const COLstring& Description,
     const COLstring& Executable, const COLstring& User, const COLstring& Password, bool AutoStart, COLstring* pError);

#endif // end of defensive include
