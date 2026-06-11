//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Application: erase_machine_id 
//
// Description:
//
// Erases the secret cached machine id.
// 
// Author: Kevin Senn
// Date:   Friday, October 3rd, 2008 @ 04:33:21 PM
//
//---------------------------------------------------------------------------
#include "Precomp.h"
#pragma hdrstop

#include <CMD/CMDlineParser.h>

#include "AppVersion.h"

#include <REG/REGregistryWindows.h>

int main(int argc, const char** argv)
{
   try
   {
      COLcout << "erase_machine_id " << APP_VERSION_MAJOR << "." << APP_VERSION_MINOR << "." << APP_BUILD_NUMBER << newline;
      CMDlineParser Parser;
      
      Parser.parseArgs(argc, argv);
      if (Parser.unknownFlagPresent(COLcout))
      {
         Parser.showUsage(COLcout);
         return EXIT_FAILURE;
      }

      // Erase the secret cached machine id
      REGregistry Registry(HKEY_CLASSES_ROOT);
      Registry.deleteSubKey("Ld.Banner");

      COLcout << "Machine ID has been erased." << newline;

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
