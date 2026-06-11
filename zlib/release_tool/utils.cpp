// ---------------------------------------------------------------------------
//  Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: utils.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   Thursday August 17, 2023
// ---------------------------------------------------------------------------
#include "utils.h"

#include <FIL/FILutils.h>

#include <PRO/PROexecute.h>

#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

bool reportMessage(const COLstring& Message, bool Success) { COLcout << Message << newline; return Success; }

void cleanupRelease(const COLstring& Location) {
   if(FILfileExists(Location))       {
      COLstring RemoveError;
      FILremoveFullDirNewSafe(Location, &RemoveError); 
   }
   if(FILfileExists("release.json")) { FILremove("release.json"); }
}

void generateJson(const COLstring& Hash, const COLstring& Version, const COLstring& Location) {
   COL_FUNCTION(generateJson);
   COLvar Json;
   Json["current"]   = Hash;
   Json["timestamp"] = getTimestamp(Hash);
   Json["version"]   = Version;
   COLstring Filename = Location + "version.json";
   COLcout << "Writing version.json" << newline;
   FILwriteFile(Filename, Json.json(1));
}

void generateReleaseJson(const COLstring& Version) {
   COL_FUNCTION(generateReleaseJson);
   COLvar Json;
   Json["version"]   = Version;
   COLcout << "Writing release.json" << newline;
   FILwriteFile("release.json", Json.json(1));
}

COLstring getTimestamp(const COLstring& Hash) {
   COL_FUNCTION(getTimestamp);
   COLstring Result;
   COLstring Err;
   int ExitCode = PROexecuteString(".", "git show -s --format=%ct " + Hash, &Result, &Err);
   Result.stripAll('\n');
   Result.stripAll('\r');
   return Result;
}
