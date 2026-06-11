// ---------------------------------------------------------------------------
// Copyright (C) 2024 Eliot Muir.  All Rights Reserved
//
// Module: cloner 
//
// Description:
//
// Implementation of conversion utility.
//
// Author: Eliot Muir 
// Date:   Sunday 15 December 2024 - 04:58PM
// ---------------------------------------------------------------------------


#include <stdlib.h>

#include <CMD/CMDlineParser.h>
#include <COL/COLerror.h>

#include <CNV/CNVgit.h>
#include <CNV/CNVchannel.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

int main(int argc, const char** ppArg){
   try{
      CMDlineParser Parser;
      Parser.parseArgs(argc, ppArg);
      COLcout << "Cloner utility..." << newline;
      
      CNVclean(COLcout);
      CNVclone("/Users/eliotmuir/Library/iNTERFACEWARE-Iguana/working_dir/", COLcout);
      CNVchannelIterate(COLcout);
   } catch (COLerror& Error){
      COLcout << Error.description() << newline;
      return EXIT_FAILURE;
   }
   return EXIT_SUCCESS;
}
