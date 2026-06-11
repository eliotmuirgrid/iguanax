// ---------------------------------------------------------------------------
// Copyright (C) 2025 Eliot Muir.  All Rights Reserved
//
// Module: analyse 
//
// Description:
//
// Utility to analyse code
//
// Author: Eliot Muir 
// Date:   Wed 15 Jan 2025 12:12:41 EST
// ---------------------------------------------------------------------------

#include <stdlib.h>

#include <CMD/CMDlineParser.h>
#include <COL/COLerror.h>

#include <CAN/CANanalysis.h>

#include <FIL/FILutils.h>
#include <FIL/FILpathUtils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

int main(int argc, const char** ppArg){
   try{
      CMDlineParser Parser;
      Parser.parseArgs(argc, ppArg);
      COLstring RootDir = FILpathResolve("../");
      COLcout << "Analysing: " << RootDir << newline;
      CANloadList(RootDir, "dirlist");    
   } catch (COLerror& Error){
      COLcout << Error.description() << newline;
      return EXIT_FAILURE;
   }
   return EXIT_SUCCESS;
}
