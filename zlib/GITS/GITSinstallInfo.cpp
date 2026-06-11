// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITSinstallInfo
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Thursday 13 July 2023 - 11:54AM
// ---------------------------------------------------------------------------

#include <GITS/GITSinstallInfo.h>

#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

COLstring GITSinstallerFile(){
   COL_FUNCTION(GITSinstallerFile);
   COLstring InstallerPath = FILpathAppend(FILpathAppend(FILworkingDir(), "install"), "GitInstaller.exe");
   COL_VAR(InstallerPath);
   return InstallerPath;
}
