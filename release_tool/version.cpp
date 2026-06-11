// ---------------------------------------------------------------------------
//  Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: version.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   Thursday August 17, 2023
// ---------------------------------------------------------------------------
#include "version.h"
#include "constants.h"
#include "serverUpload.h"
#include "utils.h"

#include <FIL/FILutils.h>
#include <FIL/FILpathUtils.h>

#include <MZIP/MZIPunzip.h>

#include <PRO/PROexecute.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

COLstring parseVersion(const COLstring& VersionText) {
   COL_FUNCTION(parseVersion);
   COLstring Parsed, Out;
   VersionText.split(Parsed, Out, "\n");
   Parsed.split(Out, Parsed, ":");
   return COLremoveAllWhiteSpace(Parsed);
}

static void unzipArchive(const COLstring& ZipFile, const COLstring& Destination){
   COL_FUNCTION(unzipArchive);
   COL_VAR2(ZipFile, Destination);
   COLstring Blah;
   COLostream NullStream(Blah);
   COLarray<COLstring> ExtractedFiles;
   FILmakeFullDir(Destination, FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX);
   MZIPunzipUnpackZipFile(ZipFile, Destination, true, NULL, &NullStream, &ExtractedFiles);
   COL_VAR(ExtractedFiles.size());
}

static COLstring appleBinaryLocation(){
   return "IguanaX.app/Contents/MacOS/";
}

COLstring getVersion(const COLstring& Location) {
   COL_FUNCTION(getVersion);
   COLstring Filename = Location;
   COLstring Out, Err, RemoveError;
#ifdef _WIN32
   Filename = FILpathAppend(Filename, "windows") + FIL_DIR_SEPARATOR + WIN_EXE;
   int ExitCode = PROexecuteString(FILparentDir(Filename), Filename + " --version", &Out, &Err);
#endif
#ifdef __APPLE__
   COLstring Destination = FILpathAppend(Filename, "macosx") + FIL_DIR_SEPARATOR + "temp";
   Filename = FILpathAppend(Filename, "macosx") + FIL_DIR_SEPARATOR + POSIX_ZIP;
   unzipArchive(Filename, Destination);
   COL_VAR(appleBinaryLocation());
   int ExitCode = PROexecuteString(FILpathAppend(Destination, appleBinaryLocation()), "chmod +x iguana", &Out, &Err);
   ExitCode = PROexecuteString(FILpathAppend(Destination, appleBinaryLocation()), "./iguana --version", &Out, &Err);
   FILremoveFullDirNewSafe(Destination, &RemoveError);
#endif
#ifdef __linux__
   COLstring Destination = FILpathAppend(Filename, "macosx") + FIL_DIR_SEPARATOR + "temp";
   Filename = FILpathAppend(Filename, "linux") + FIL_DIR_SEPARATOR + POSIX_ZIP;
   unzipArchive(Filename, Destination);
   int ExitCode = PROexecuteString(Destination, "chmod +x iguana" , &Out, &Err);
   ExitCode = PROexecuteString(Destination, "./iguana --version", &Out, &Err);
   FILremoveFullDirNewSafe(Destination, &RemoveError);
#endif
   return parseVersion(Out);
}

bool createReleaseJson(const COLstring& Version) {
   COL_FUNCTION(createReleaseJson);
   char VersionIn[25]; // big for safety
   reportMessage("To confirm, please re-enter the version");
   if(!fgets(VersionIn, sizeof(VersionIn), stdin)) { return reportMessage("Nothing was entered -- exiting program"); }
   if(Version != COLremoveAllWhiteSpace(COLstring(VersionIn))) {
      return reportMessage(Version + " != " + Version + " -- exiting program", false);
   }
   if(!checkVersionExists(Version)) { return reportMessage(Version + " not found on release server", false); }
   generateReleaseJson(Version);
   return true;
}
