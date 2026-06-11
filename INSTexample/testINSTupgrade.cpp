// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testINSTupgrade
//
// Description:
//
// Implementation of tests
//
// Author: Eliot Muir 
// Date:   Sunday 23 July 2023 - 12:09PM
// ---------------------------------------------------------------------------
#include "testINSTupgrade.h"

#include <UNIT/UNITapp.h>   

#include <INST/INSTupgrade.h>
#include <INST/INSTdownload.h>

#include <FIL/FILutils.h>
#include <COL/COLsplit.h>
#include <PRO/PROlaunch.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static void testDownload(){
   COL_FUNCTION(testDownload);
   COLstring Guid = "8b75df5bf";
  
   COLstring Error;
   bool Result;
   if (!INSTdownloaded(Guid)){
      Result = INSTdownload(Guid, &Error);
      COL_VAR2(Result, Error);
   }
}

static void testAt(){
   COL_FUNCTION(testAt);
   INSTrunAt("echo Cheese > " + FILworkingDir()+"cheese.txt");
}

static void testInplaceUpgrade(){
   COL_FUNCTION(testInplaceUpgrade);
   COLstring Garbage = "some-garbage-data";
   COLstring TestFileName;
   COLstring CopyFileName;
#ifdef _WIN_32
   TestFileName = "test.exe";
   CopyFileName = "test_copy.exe";
#else
   TestFileName = "test";
   CopyFileName = "test_copy";
#endif   
   if (FILfileExists("size.txt")){
      COLstring OldFileSize;
      FILreadFile("size.txt", &OldFileSize);
      UNIT_ASSERT_EQUALS(FILfileSize(TestFileName) - atoi(OldFileSize.c_str()), Garbage.size());
      FILremove("size.txt");
      return;
   }
   int OriginalSize = FILfileSize(TestFileName);
   FILappendFile("size.txt", COLintToString(OriginalSize) + "\n");
   FILcopyFile(TestFileName, CopyFileName);
   FILappendFile(CopyFileName, Garbage);

   COLstring ShellCommand;
   ShellCommand = "sh -c \"cp " + CopyFileName + " " + TestFileName + " && ./" + TestFileName + " --test replace\"";
   COL_VAR(ShellCommand);
   int ProcessId = PROlaunch(FILworkingDir(), ShellCommand);
   COL_VAR(ProcessId);
}
static void testReleaseVersion(){
   COL_FUNCTION(testReleaseVersion);
   COLstring HtmlData;
   FILreadFile("ReleaseServerDownloads.html", &HtmlData);
   COL_VAR(HtmlData);
   COLvar Parts;
   COLsplit(&Parts, HtmlData, "\r\n");
   COL_VAR(Parts);
   COLstring VersionLine;
   for (int i  = 0; i < Parts.size(); i++){
      if (Parts[i].asString().substr(0, 2) == "<a"){
         VersionLine = Parts[i].asString();
         break;
      }
   } 
   COL_VAR(VersionLine);
   VersionLine = VersionLine.substr(VersionLine.find("href=\"") + 6);
   COLstring Garbage;
   VersionLine.split(VersionLine, Garbage, "/");
   COL_VAR(VersionLine);

}


void testINSTupgrade(UNITapp* pApp) {
   pApp->add("download/test", &testDownload);
   // pApp->add("release/version", &testReleaseVersion);
   // pApp->add("replace", testInplaceUpgrade); // DO NOT RUN THIS TEST!!!
}
