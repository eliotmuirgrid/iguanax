// ---------------------------------------------------------------------------
//  Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: serverUpload.cpp
//
//  Description: Contains all logic related to uploading files to release server
//
//  Author: Matthew Sobkowski
//  Date:   Thursday August 17, 2023
// ---------------------------------------------------------------------------
#include "serverUpload.h"
#include "constants.h"
#include "utils.h"

#include <FIL/FILutils.h>
#include <FIL/FILpathUtils.h>

#include <PRO/PROexecute.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

bool checkVersionExists(const COLstring& Version) {
   COL_FUNCTION(checkVersionExists);
   COLcout << "Checking if " << Version << " exists..." << newline;
   COLstring Out, Err;
   COLstring Command  = "ssh -i " + COLstring(PEM_FILE) + " " + EC2_ADD + " if [ -d downloads/" + Version + "/ ]; then echo \"0\"; else echo \"-1\"; fi";
   COL_VAR(Command);
   int ExitCode = PROexecuteString("", Command, &Out, &Err);
   COL_VAR(Out);
   return atoi(Out.c_str()) == 0;
}

bool changeFilePermission(const COLstring& FilePath) {
   COL_FUNCTION(changeFilePermission);
   COLcout << "Changing permission of " << FilePath << newline;
   COLstring Out, Err;
   COLstring Command  = "ssh -i " + COLstring(PEM_FILE) + " " + EC2_ADD + " chmod -R o+rx downloads/" + FilePath;
   COL_VAR(Command);
   int ExitCode = PROexecuteString("", Command, &Out, &Err);
   COL_VAR3(ExitCode, Out, Err);
   return ExitCode == 0;
}

bool uploadRelease(const COLstring& UploadDir) {
   COL_FUNCTION(uploadRelease);
   COLcout << "Beginning scp " << UploadDir << " to " << EC2_ADD << ":downloads/" << newline;
   COLstring Out, Err;
   COLstring Command  = "scp -v -r -o StrictHostKeyChecking=no -i " + COLstring(PEM_FILE) + " " + UploadDir + " " + EC2_ADD + ":downloads/";
   COL_VAR(Command);
   // 120 - scp upload can take a while.
   int ExitCode = PROexecuteString("", Command, &Out, &Err, 500);
   COL_VAR3(ExitCode, Out, Err);
   if (ExitCode != 0){
      COLcout << "Failed, command was: " << newline << Command << newline;
      COLcout << Out << newline << Err << newline;
      COLcout << "ExitCode = " << ExitCode << newline;
   }
   return ExitCode == 0;
}

bool uploadChangelog(const COLstring& Version, const COLstring& PathToChangelog) {
   COL_FUNCTION(uploadChangelog);
   COLcout << "Beginning scp " << PathToChangelog << " to " << EC2_ADD << ":downloads/" << Version << newline;
   COLstring Out, Err;
   COLstring Command  = "scp -o StrictHostKeyChecking=no -i " + COLstring(PEM_FILE) + " " + PathToChangelog + " " + EC2_ADD + ":downloads/" + Version;
   int ExitCode = PROexecuteString("", Command, &Out, &Err);
   COL_VAR3(ExitCode, Out, Err);
   return changeFilePermission(FILpathAppend(Version, "changelog.html"));
}

bool uploadReleaseJson(){
   COL_FUNCTION(uploadReleaseJson);   
   COLstring File = FILpathAppend(FILworkingDir(), "release.json");
   COLcout << "Beginning scp " << File << " to " << EC2_ADD << ":downloads/" << newline;
   COLstring Out, Err;
   COLstring Command  = "scp -o StrictHostKeyChecking=no -i " + COLstring(PEM_FILE) + " " + File + " " + EC2_ADD + ":downloads/";
   COL_VAR(Command);
   int ExitCode = PROexecuteString("", Command, &Out, &Err);
   COL_VAR3(ExitCode, Out, Err);
   return changeFilePermission("release.json");
}