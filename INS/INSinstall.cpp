//---------------------------------------------------------------------------
// Copyright (C) 1997-2019 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INSinstall
//
// Description:
//
// See header file.
//
// Author: John Qi
// Date:   Mon 24 Jun 2019 16:07:56 EDT
//
//---------------------------------------------------------------------------

#include "INSpackage.h"

#include <MZIP/MZIPunzip.h>

#include <FIL/FILutils.h>
#include <FIL/FILpathUtils.h>

#include <COL/COLsinkNull.h>
#include <COL/COLerror.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static const COLstring TEMP_FILE = "installer.zip.tmp";


static void cleanUpTempFile() {
   try {
      FILremoveWithThrow(TEMP_FILE);
   }
   catch (const COLerror& Error) {
      throw COLerror("Failed to remove temp file '" + TEMP_FILE + "'. Please remove it manually.", __LINE__, __FILE__, COLerror::FileError);
   }
}

void INSinstallProgramFiles(const COLstring& Installer, const COLstring& ZipRootDir, const COLstring& AppDir) {
   COL_FUNCTION(INSinstallProgramFiles);
   COL_VAR3(Installer, ZipRootDir, AppDir);
   FILmakeFullDir(AppDir, FIL_USER_RWX);
   COLostream NullStream(new COLsinkNull(), true);
   // extract attached zip
   COLstring SourceFile = Installer;

   INSunpack(SourceFile, TEMP_FILE);
   try {
      // unzip
      COL_TRC("About to unzip");
      MZIPunzipUnpackZipFile(TEMP_FILE, AppDir, true, NULL, &NullStream); 
      // move files to AppDir
      //FILaddPathSeparator(ParentDir);
      //COL_TRC("Rename " << ParentDir+ZipRootDir << " to " << AppDir);
      //FILrenameFile(ParentDir + ZipRootDir, AppDir);
      cleanUpTempFile();
   }
   catch (const COLerror& Error) {
      cleanUpTempFile();
      throw;
   }
}