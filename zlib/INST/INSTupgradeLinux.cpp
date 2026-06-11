// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INSTupgradeLinux
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Friday 25 August 2023 - 02:56PM
// ---------------------------------------------------------------------------

#include <INST/INSTupgradeLinux.h>
#include <INST/INSTupgrade.h>
#include <INST/INSTdownload.h>
#include <INST/INSTunzip.h>

#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <SDM/SDMshutdown.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

bool INSTupgradeLinux(const COLstring& Version, COLstring* pError){
   COL_FUNCTION(INSTupgradeLinux);
   COLstring UnzipLocation = INSTdownloadDir() + FILpathNameWithoutLastExt(INSTdownloadFilePath(Version)); 
   if (!INSTunzip(INSTdownloadFilePath(Version), UnzipLocation)) {
      COL_TRC("We failed to extract");
      *pError = "Failed to extract " + INSTdownloadFilePath(Version);
      INSTlogUpgradeStartFailure();
      return false;
   }
   COLstring Dest = FILexecutableDir() + FILexecutableFileName();
   COL_TRC("Rename old exe");
   FILrenameFile(Dest, Dest + ".tmp");
   COL_TRC("Copying " << UnzipLocation << " to " << Dest);
   FILcopyFile(FILpathAppend(UnzipLocation, "iguana"), Dest, true); 
   COL_TRC("Make " << Dest << " executable.");
   FILsetPermission(Dest, FIL_USER_RWX);
   COL_TRC("Remove old exe and extracted file");
   FILremove(Dest + ".tmp");
   COLstring RemoveError;
   FILremoveFullDirNewSafe(UnzipLocation, &RemoveError);
   COL_TRC("Exit the service and rely on auto start to get us back");
   INSTlogUpgradeStartSuccess();
   SDMshutdown();
   return true;
}

