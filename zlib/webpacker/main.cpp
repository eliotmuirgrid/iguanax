//---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: webpacker 
//
// Description: Packs our web files and attaches them to the end of the executable
//
// Webpacker utlity
//
// Author: Matthew Sobkowski
// Date:   Thu  8 Jun 2023 14:16:13 EDT
//---------------------------------------------------------------------------
#include <CMD/CMDlineParser.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>

#include <PACK/PACKattach.h>
#include <PACK/PACKattachC.h>
#include <PACK/PACKwebPack.h>

#include <SCK/SCKutils.h>

#include <WEBI/WEBImimeLookup.h>

#include <COL/COLarray.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>
#include "BZP/bzlib_private.h"
COL_LOG_MODULE;

int main(int argc, const char** argv) {
   WEBImimeLookupInitMap();
   FILinitExecutableName(argv[0]); // for Posix daemons
   SCKinitWinsock();
   CMDlineParser Parser;
   Parser.setDescription("webpacker utility");
   Parser.addFlagWithArgument("binary", "path to executable binary");
   Parser.addFlagWithArgument("c", "Name of C file to output to");
   Parser.parseArgs(argc, argv);
   if(Parser.parsingErrorsPresent(COLcout)) { 
      Parser.showUsage(COLcout); 
      return EXIT_FAILURE;
   }
   try{
      COLstring IguanaBinary = Parser.flagArgument("binary");
      COLarray<COLstring> DirList;
      bool Cpack   = Parser.isFlagPresent("c");
      for(int i = 0; i < Parser.countOfExtraArgument(); i++) {
         COLcout << "Processing dir " << Parser.extraArgument(i) << newline;
         DirList.push_back(Parser.extraArgument(i));
      }
      COLstring CFile;
      CFile = Parser.flagArgument("c");
      // command line option - use makefile to switch based on OS - write entirely different function for packing
      if(Cpack){
         PACKwebPack(DirList, COLnewClosure1(&PACKattachC, CFile));
      }else{
         PACKwebPack(DirList, COLnewClosure1(&PACKattachToExe, IguanaBinary));
      }
   } catch (COLerror& Error){
      COLcout << Error.description() << newline;
      return EXIT_FAILURE;
   }
   return EXIT_SUCCESS;
}
