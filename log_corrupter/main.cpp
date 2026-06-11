//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Application: log_corrupter 
//
// Description:
//
// Utility to test corrupting Iguana log files.
// 
// Author: Eliot Muir
// Date:   Wednesday, February 18th, 2009 @ 02:07:35 PM
//
//---------------------------------------------------------------------------
#include "Precomp.h"
#pragma hdrstop

#include <CMD/CMDlineParser.h>

#include "AppVersion.h"

#include <FIL/FILutils.h>
#include <FIL/FILfile.h>

#include <COL/COLdateTime.h>
#include <COL/COLutils.h>
#include <COL/COLmath.h>

COLuint64 LOGeliotsCrappyRand64(COLuint64 Max)
{
   COLmath::resetRandomGenerator();

   if (Max < 0x7FFF)
   {
      return COLmath::randomNumber((COLuint32)Max);
   }
   else
   {
      return (COLmath::randomNumber((COLuint32)((Max >> 12) & 0xFFF)) << 12) + COLmath::randomNumber(0xFFF);
   }
}



int main(int argc, const char** argv)
{
   try
   {
      COLcout << "log_corrupter " << APP_VERSION_MAJOR << "." << APP_VERSION_MINOR << "." << APP_BUILD_NUMBER << newline;
      CMDlineParser Parser;

      Parser.addFlagWithoutArgument("boundary", "Corrupt a byte on a 1 MB boundary");
      Parser.addFlagWithoutArgument("first",    "Corrupt the first byte");
      Parser.addFlagWithoutArgument("last",    "Corrupt the last byte");
      Parser.addFlagWithoutArgument("random",   "Corrupt a random byte (default)");
      Parser.addFlagWithoutArgument("run",   "Corrupt a random byte (default)");


      Parser.addExtraParamDescription("logfile", "Log file to corrupt.");
      Parser.setDescription("Utility for testing fault tolerance Iguana has for corrupt log files.");

      
      Parser.parseArgs(argc, argv);
      if (Parser.parsingErrorsPresent(COLcout) || Parser.countOfExtraArgument() > 1)
      {
         Parser.showUsage(COLcout);
         return EXIT_FAILURE;
      }

      // Find log file
      COLstring LogFile;

      LogFile = COLdateTime::currentDate().format("%Y%m%d") + ".log";
      
      if (Parser.countOfExtraArgument() == 1)
      {
         LogFile = Parser.extraArgument("logfile");
      }

      if (!FILfileExists(LogFile))
      {
         COLcout << "File " << LogFile << " not found." << newline;
         return EXIT_FAILURE;
      }

      // Find byte to kill
      COLuint64 SizeOfLogFile = FILfileSize(LogFile.c_str());   
      COLuint64 BytePostion = 0;
      COLmath::resetRandomGenerator();
      BytePostion = LOGeliotsCrappyRand64(SizeOfLogFile);
      if (BytePostion > SizeOfLogFile)
      {
         BytePostion = SizeOfLogFile - 1;
      }
   
      if (Parser.isFlagPresent("boundary"))
      {
         BytePostion = 1024 * 1024;
      }
      else if (Parser.isFlagPresent("last"))
      {
         BytePostion = SizeOfLogFile-1;   
      }
      else if (Parser.isFlagPresent("first"))
      {
         BytePostion = 0;
      }

      unsigned char NewChar = COLmath::randomNumber(0xFF);
      unsigned char OldChar=0;


      FILfile File(LogFile, FILfile::Write);
      File.setPosition64(BytePostion);
      File.read(&OldChar, 1);
      COLcout << "Corrupting " << LogFile << newline;
      COLcout << "Modifying byte " << hex << BytePostion << " of " << SizeOfLogFile << newline
              << "from " << (int)OldChar << " to " << (int)NewChar << newline;
      File.setPosition64(BytePostion);
      if (Parser.isFlagPresent("run"))
      {
         File.write(&NewChar ,1);
      }
      else
      {  
         COLcout << "Use --run to actually do it." << newline;
      }
      
      File.close();

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
