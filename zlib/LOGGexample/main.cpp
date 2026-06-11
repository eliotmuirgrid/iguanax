//---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LOGGexample
//
// Description:
//
// Unit test for the LOGG library
//
// Author: John Q
// Date:   Mon  6 Nov 2023 11:44:49 EST
//---------------------------------------------------------------------------
#include "testMemory.h"

#include <UNIT/UNITapp.h>

#include <SDB/SDBcontent.h>

#include <FIL/FILutils.h>

#include <COL/COLerror.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

// Run with as little boiler plate as possible.
int main(int argc, const char** argv){
   FILinitExecutableName(argv[0]);
   SDBcontentMapInit();
   UNITapp App;
   testMemory(&App);
   return App.run(argc, argv);
}