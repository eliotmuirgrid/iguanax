//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CAPIexample
//
// Description:
//
// Unit test for the CAPI library
//
// Author: Vismay Shah
// Date:   Wed 12/05/2021
//---------------------------------------------------------------------------
#include "testApi.h"

#include <COL/COLlog.h>
#include <CAPI/CAPIlog.h>
#include <UNIT/UNITapp.h>
COL_LOG_MODULE;

// Run with as little boiler plate as possible.
int main(int argc, const char** argv){
   UNITapp App;
   // set up stdout redirect for testApi()
   CAPIredirectStdout(&logToMemory);
   testApi(&App);
   return App.run(argc, argv);
}
