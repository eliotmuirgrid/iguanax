//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: UNITexample
//
// Description:
//
// Unit test for INFO library
//
// Author: Matthew Sobkowski
// Date:   Thu 05/13/2021
//---------------------------------------------------------------------------
#include <UNIT/UNITapp.h>
#include "testExecute.h"
#include "testSetup.h"
#include "testEnv.h"

#include <FIL/FILutils.h>
#include <COL/COLutils.h>
#include <COL/COLmath.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

int generateErrOut(int Count){
   for (int i=0; i < Count; i++){
      COLcout << "O"; 
      COLcerr << "E";
   }
   return 3;
}

int generateTimeout(int Delay){
   for (int i=1; i<=5; i++){
      COLsleep(Delay);
      COLcout << "Tick " << i << newline << flush;
   }
   return 0;
}

int main(int argc, const char** argv){
   if (argc == 3){
      if (strcmp(argv[1], "test") == 0)   { return generateErrOut (COLmath::atoi64(argv[2]));  }
      if (strcmp(argv[1], "timeout") == 0){ return generateTimeout(COLmath::atoi64(argv[2])); }
   }

   // Run with as little boiler plate as possible.
   UNITapp App;
   testSetup(&App);
   testExecute(&App);
   testEnv(&App);
   return App.run(argc, argv);
}
