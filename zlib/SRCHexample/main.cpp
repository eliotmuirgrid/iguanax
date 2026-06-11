//---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SRCHexample
//
// Description:
//
// Unit test for the SRCH library
//
// Author: Vismay Shah
// Date:   Tue 28 Nov 2023 13:47:13 EST
//---------------------------------------------------------------------------
#include "testFilter.h"
#include "testFind.h"
#include "testReplaceAll.h"
#include <COL/COLlog.h>
#include <UNIT/UNITapp.h>
COL_LOG_MODULE;

// Run with as little boiler plate as possible.
int main(int argc, const char** argv){
   UNITapp App;
   testFilter(&App);
   testFind(&App);
   testReplaceAll(&App);
   return App.run(argc, argv);
}
