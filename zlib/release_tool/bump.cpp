// ---------------------------------------------------------------------------
//  Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: bump.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   Friday, September 29, 2023
// ---------------------------------------------------------------------------
#include "bump.h"
#include "utils.h"

#include <FIL/FILutils.h>
#include <FIL/FILpathUtils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

static bool extractVersion(const COLstring& Content, const COLstring& ToFind, int* pNumber) {
   COL_FUNCTION(extractVersion);
   size_t FoundPos = Content.find(ToFind);
   if(FoundPos == npos) { return reportMessage(ToFind + " not found in file", false); }
   size_t    StartPos       = FoundPos + ToFind.size();
   size_t    EndPos         = Content.find('\n', StartPos);
   COLstring BuildNumberStr = Content.substr(StartPos, EndPos - StartPos);
   *pNumber = atoi(BuildNumberStr.c_str());
   return true;
}

static bool updateBuildVersion(COLstring* pContent) {
   COLstring SearchString = "#define DBD_BUILD_NUMBER ";
   size_t    FoundPos     = pContent->find(SearchString);
   if(FoundPos == npos) { return reportMessage("Bump failed -- could not find " + SearchString + " in DBDVERversion.h", false); }
   size_t    StartPos       = FoundPos + SearchString.size();
   size_t    EndPos         = pContent->find('\n', StartPos);
   COLstring BuildNumberStr = pContent->substr(StartPos, EndPos - StartPos);
   int       BuildNumber    = atoi(BuildNumberStr.c_str());
   BuildNumber++;
   COLstring NewBuildNumberStr = COLint32ToString(BuildNumber);
   pContent->replace(StartPos, EndPos - StartPos, NewBuildNumberStr);
   return true;
}

COLstring extractVersionData(const COLstring& Content) {
   COL_FUNCTION(extractVersionData);
   int Major = 0;
   int Minor = 0;
   int Build = 0;
   extractVersion(Content, "#define DBD_VERSION_MAJOR", &Major);
   extractVersion(Content, "#define DBD_VERSION_MINOR", &Minor);
   extractVersion(Content, "#define DBD_BUILD_NUMBER",  &Build);
   COL_VAR3(Major, Minor, Build);
   return COLint32ToString(Major) + "." + COLint32ToString(Minor) + "." + COLint32ToString(Build);
}

int bumpVersion() {
   COL_FUNCTION(bumpVersion);
   COLstring DBDVERversion = FILpathResolve("../DBDVER/DBDVERversion.h");
   if(!FILfileExists(DBDVERversion)) {
      reportMessage("Version bump failed: could not find " + DBDVERversion, false);
      return EXIT_FAILURE;
   }
   COLstring Content;
   FILreadFile(DBDVERversion, &Content);
   if(!updateBuildVersion(&Content)) { return EXIT_FAILURE; }
   FILwriteFile(DBDVERversion, Content);
   reportMessage("Updated IguanaX version to: " + extractVersionData(Content));
   return EXIT_SUCCESS;
}