//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Application: ipack
//
// Description:
//
// A command line too that appends a payload file to a target file
// 
// Author: John Qi
// Date:   Thu 21 Jun 2019 10:07:16 EDT
//
//---------------------------------------------------------------------------

#include <INS/INSpackage.h>

#include <FIL/FILutils.h>

#include <CMD/CMDlineParser.h>

#include <COL/COLerror.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;


int main(int argc, const char** argv)
{
   try {
      CMDlineParser Parser;

      Parser.addExtraParamDescription("Payload", "The payload to be appended to the target file.");
      Parser.addExtraParamDescription("Target", "The file that will carry the payload.");
      Parser.addFlagWithoutArgument("dir","Must be set if the payload is a directory.");
      Parser.addFlagWithArgument("out","file_out","Append the payload to a copy of the target file");

      Parser.parseArgs(argc, argv);
      if ( Parser.parsingErrorsPresent(COLcerr) || Parser.countOfExtraArgument() != 2) {
         Parser.showUsage(COLcerr);
         return EXIT_FAILURE;
      }

      COLstring TargetFile = Parser.extraArgument("Target");
      if ( Parser.isFlagPresent("out") ) {
         const COLstring& Filename= Parser.flagArgument("out");
         FILcopyFile(TargetFile, Filename, true);
         TargetFile = Filename;
      }

      const COLstring& Payload = Parser.extraArgument("Payload");
      
      COL_VAR2(TargetFile, Payload);
      INSpack(TargetFile, Payload, Parser.isFlagPresent("dir"));

      return EXIT_SUCCESS;
   }
   catch (const COLerror& Error) {
      COLcerr << Error.description() << newline;
      return EXIT_FAILURE;
   }
   catch (...) {
      COLcerr << "Unhandled Exception" << newline;
      return EXIT_FAILURE;
   }

   return EXIT_SUCCESS;
}
