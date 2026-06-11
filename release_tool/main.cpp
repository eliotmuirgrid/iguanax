// ---------------------------------------------------------------------------
//  Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: main.cpp
//
//  Description: Command line utility to generate version.json for releases
//
//  Author: Matthew Sobkowski
//  Date:   Wednesday August 16, 2023
// ---------------------------------------------------------------------------
#include <cstdlib>
#include <cstdio>

#include "bump.h"
#include "constants.h"
#include "parallel.h"
#include "serverUpload.h"
#include "utils.h"
#include "version.h"

#include <CMD/CMDlineParser.h>
#include <FIL/FILutils.h>
#include <FIL/FILpathUtils.h>

#include <SCK/SCKutils.h>

#include <PRO/PROexecute.h>

#include <COL/COLerror.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

/**
 * IMPORTANT: you must change the PEM_FILE global in constants.cpp to the location of your pem file
 * constants.cpp provides links to all 3 download servers as well as the aws ec2 instance
 * @param --hash the hash of the commit you want to create a release for (must exist on all 3 servers)
 * @param --notes can provide a path to an html file that will be uploaded along with the release (OPTIONAL)
 * @return exit code
 * @example_command: ./release_tool --hash 9639522925 --notes changelog.html
 */

static bool verifyPemFile() {
   COL_FUNCTION(verifyPemFile);
   COLstring Expanded = FILexpand(PEM_FILE);
   if(!FILfileExists(Expanded)) { return reportMessage("Could not find " + COLstring(Expanded), false); }
   COLstring Out, Err;
   int ExitCode = PROexecuteString("", "chmod 600 " + COLstring(Expanded), &Out, &Err);
   COL_VAR3(ExitCode, Out, Err);
   return ExitCode== 0 || reportMessage(Err, false);
}

int main(int argc, const char** argv) {
   SCKinitWinsock();
   CMDlineParser Parser;
   COLcout << "Iguana Release Tool 1.0" << newline;
   Parser.addFlagWithArgument("hash", "Hash of commit you want to build");
   Parser.addFlagWithArgument("notes", "Path to changelog.html which will be uploaded under the release");
   Parser.addFlagWithArgument("release_version", "Version to set release.json to");
   Parser.addFlagWithoutArgument("bump", "Increments build number in DBDVERversion.h");
   try {
      Parser.parseArgs(argc, argv);
      if(!verifyPemFile()) { return EXIT_FAILURE; }
      if(Parser.parsingErrorsPresent(COLcout)) {
         Parser.showUsage(COLcout);
         return EXIT_FAILURE;
      }
      if(Parser.isFlagPresent("bump")) { return bumpVersion(); }
      COLstring ChangelogFile = Parser.isFlagPresent("notes") ? Parser.flagArgument("notes") : "changelog.html";

      if(Parser.isFlagPresent("hash")) { beginParallelProcess(Parser.flagArgument("hash"), ChangelogFile); }

      if(Parser.isFlagPresent("release_version")) {
         COLstring Version = Parser.flagArgument("release_version");
         if(!createReleaseJson(Version)) { return EXIT_FAILURE; }
         if(!uploadReleaseJson()) { return EXIT_FAILURE; }
         return EXIT_SUCCESS;
      }
   } catch(const COLerror& Error) {
      COLcout << Error.description() << newline;
      return EXIT_FAILURE;
   }
   return EXIT_SUCCESS;
}
