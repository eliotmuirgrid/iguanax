//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: BLUAexample
//
// Description:
//
// Unit test for the BLUA library
//
// Author: Vismay Shah
// Date:   Wed 12/05/2021
//---------------------------------------------------------------------------
#include "testTranslatorHeadless.h"

#include <COL/COLerror.h>
#include <COL/COLlog.h>
#include <FIL/FILutils.h>
#include <UNIT/UNITapp.h>
COL_LOG_MODULE;

// Run with as little boiler plate as possible.
int main(int argc, const char** argv){
   FILinitExecutableName(argv[0]);
   UNITapp App;
   testTranslatorHeadless(&App);
   // return App.run(argc, argv);
   return App.runSingleThreaded(argc, argv);
}
