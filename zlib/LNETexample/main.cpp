//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LNETexample
//
// Description:
//
// Unit test for LNET library
//
// Author: Vismay Shah
// Date:   Tue 06/20/2023 
//---------------------------------------------------------------------------
#include <UNIT/UNITapp.h>
#include "testCurl.h"

#include <FIL/FILutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

// Run with as little boiler plate as possible.   
int main(int argc, const char** argv){
   FILinitExecutableName(argv[0]);
   UNITapp App;
   COL_TRC("Made test collection.");
   testCurl(&App);
   return App.run(argc, argv);  
}
