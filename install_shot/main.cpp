//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Application: install_shot 
//
// Description:
//
// install_shot is a simple utility designed for use in installing/uninstalling
// applications.  It will generate log files during installations for the use
// of the corresponding uninstallers.
// 
// Author: Kevin Senn
// Date:   Wednesday, September 14th, 2005 @ 10:39:15 AM
//
//---------------------------------------------------------------------------
#include "Precomp.h"
#pragma hdrstop

#include <windows.h>
#include <COL/COLref.h>
#include <CMD/CMDlineParser.h>
#include <FIL/FILutils.h>
#include <REX/REXmatcher.h>
#include <REG/REGregistryWindows.h>
#include <DOT/DOTassembly.h>
#include <install_shot/AppVersion.h>
#include <install_shot/SHOTsnapshot.h>

int main(int argc, const char** argv)
{
   try
   {
      CMDlineParser Parser;

      Parser.addFlagWithArgument("cleanPATH","Path","Remove the specified path from the environment variable PATH.  The specified Path can (but doesn't have to) be in the DOS short path format.  This will also removed the DelphiPackages subdirectory from PATH, if it exists.  The argument to --cleanPATH must not end with \\.  When --cleanPATH is used, no <Logfile> argument is required");

      Parser.addFlagWithArgument("dotnetRegister","Dll Path","Register the .NET object specified by Dll Path in the global assembly cache");
      Parser.addFlagWithArgument("dotnetUnregister","Assembly Name","Unregister the .NET object specified by Assembly Name from the global assembly cache");
      Parser.addFlagWithArgument("fusion","Fusion Path","The path of the fusion.dll file to use (if not specified, the most current version will be used).  If --dotnetRegister and --version flags are used, <LogFile> is not required");

      Parser.addFlagWithArgument("before", "Filename", "The snapshot of the directory before installing a section of files.  Must be used in combination with --after");
      Parser.addFlagWithArgument("after", "Filename", "The snapshot of the directory after installing a section of files.  Must be used in combination with --before.  Differences will be appended to Logfile");
      Parser.addExtraParamDescriptionRequired("Logfile", "The snapshot file to write output to");

      Parser.setDescription("iNTERFACEWARE Install Snapshot Utility - recursively takes snapshots of all files the current directory.  Can also find differences between two snapshots, and output the differences to another snapshot file.");

      Parser.parseArgs(argc, argv);

      if (Parser.isFlagPresent("cleanPATH"))
      {
         // Warning: this requires that the argument to cleanPATH does
         // not end with "\".
         COLstring PathRegex = Parser.flagArgument("cleanPATH");
         PathRegex.replace("\\", "\\\\");
         // get rid of all ~1's, ~2's, and so on
         REXmatcher ExpandShortDOSPathMatcher("~[1-9]");
         // we'll replace them with the regular expression below
         // what it means is "anything that's not a '\' or a ';'
         // this will allow "directory completion' for dos short
         // path format.
         ExpandShortDOSPathMatcher.regexReplace(PathRegex, "[^(\\\\|;)]*");
         REGregistry Environment(HKEY_LOCAL_MACHINE);
         Environment.openKey("SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment", true);
         
         COLstring ClassPathValue = Environment.readString("CLASSPATH", "");
         if ("" != ClassPathValue)
         {
            COLstring JavaMatcherRegex = ";" + PathRegex + "\\\\java\\\\classes.jar";
            REXmatcher PATHCleanerMatcherJava(JavaMatcherRegex.c_str(), REXmatcher::CaseInsensitive);
            PATHCleanerMatcherJava.regexReplace(ClassPathValue, "");
            Environment.writeString("CLASSPATH", ClassPathValue, REGregistry::REGdefaultString);
         }
         
         COLstring PATHValue;
         PATHValue = Environment.readString("PATH", "");
         if ("" == PATHValue)
         {
            COLcerr << "Could not find PATH environment variable" << newline;
            return EXIT_FAILURE;
         }
         
         COLstring DelphiMatcherRegex = ";" + PathRegex + "\\\\DelphiPackages[^;]*";
         REXmatcher PATHCleanerMatcherDelphi(DelphiMatcherRegex.c_str(), REXmatcher::CaseInsensitive);
         PATHCleanerMatcherDelphi.regexReplace(PATHValue, "");
         
         COLstring RootMatcherRegex = ";" + PathRegex;
         REXmatcher PATHCleanerMatcher(RootMatcherRegex.c_str(), REXmatcher::CaseInsensitive);
         PATHCleanerMatcher.regexReplace(PATHValue, "");
         Environment.writeString("PATH", PATHValue, REGregistry::REGexpandString);
         return EXIT_SUCCESS;
      }

      if (Parser.isFlagPresent("dotnetRegister"))
      {
         COLstring DllPath = Parser.flagArgument("dotnetRegister");
         COLstring FusionPath;
         if (!Parser.isFlagPresent("fusion"))
         {
            FusionPath = DOTassembly::DOTfindFusionDll();
            if (FusionPath == "")
            {
               COLcout << "Could not find fusion.dll" << newline;
               return EXIT_FAILURE;
            }
         }
         else
         {
            FusionPath = Parser.flagArgument("fusion");
         }
         DOTassembly::DOTregisterObject(DllPath, FusionPath);
         COLcout << "Registered " << DllPath << newline;
         return EXIT_SUCCESS;
      }

      if (Parser.isFlagPresent("dotnetUnregister"))
      {
         COLstring AssemblyName = Parser.flagArgument("dotnetUnregister");
         COLstring FusionPath;
         if (!Parser.isFlagPresent("fusion"))
         {
            FusionPath = DOTassembly::DOTfindFusionDll();
            if (FusionPath == "")
            {
               COLcout << "Could not find fusion.dll" << newline;
               return EXIT_FAILURE;
            }
         }
         else
         {
            FusionPath = Parser.flagArgument("fusion");
         }

         DOTassembly::DOTunregisterObject(AssemblyName, FusionPath);
         COLcout << "Unregistered " << AssemblyName << newline;
         return EXIT_SUCCESS;
      }

      if
      (
         Parser.unknownFlagPresent(COLcout) ||
         Parser.countOfExtraArgument() != 1
      )
      {
         Parser.showUsage(COLcout);
         return EXIT_FAILURE;
      }

      if
      (
         Parser.isFlagPresent("before") &&
         Parser.isFlagPresent("after")
      )
      {
         // we are "diffing"
         SHOTsnapshot Snapshot(Parser.extraArgument(0));
         Snapshot.diffSnapshot
         (
            Parser.flagArgument("before"),
            Parser.flagArgument("after")
         );
      }
      else if
      (
         !Parser.isFlagPresent("before") &&
         !Parser.isFlagPresent("after")
      )
      {
         // take a simple snapshot
         SHOTsnapshot Snapshot(Parser.extraArgument(0));
         Snapshot.takeSnapshot();
      }
      else
      {
         Parser.showUsage(COLcout);
         return EXIT_FAILURE;
      }


      return EXIT_SUCCESS;
   }
   catch (const COLerror& Error)
   {
      COLcerr << Error.description() << newline;
      return EXIT_FAILURE;
   }
   catch (...)
   {
      COLcerr << "Unhandled Exception" << newline;
      return EXIT_FAILURE;
   }
   return EXIT_SUCCESS;
}
