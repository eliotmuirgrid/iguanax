//---------------------------------------------------------------------------
// Copyright (C) 1997-2026 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: ENVCexample
//
// Description:
//
// Unit test for the ENVC library
//
// Author: John Q
// Date:   Fri May  8 15:17:09 EDT 2026
//---------------------------------------------------------------------------
#include "testCache.h"

// #include <COL/COLerror.h>
// #include <COL/COLlog.h>
#include <FIL/FILutils.h>
#include <UNIT/UNITapp.h>
// COL_LOG_MODULE;

// Run with as little boiler plate as possible.
int main(int argc, const char** argv){
   FILinitExecutableName(argv[0]);
   UNITapp App;
   testCache(&App);
   return App.run(argc, argv);
}
