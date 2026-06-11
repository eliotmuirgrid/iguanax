//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CFGexample
//
// Description:
//
// Unit test for the CFG library
//
// Author: Orkhan
// Date:   Thu  8 Sep 2022 10:11:52 EDT
//---------------------------------------------------------------------------

#include <UNIT/UNITapp.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

#include "testApi.h"
#include "testCFGfields.h"

int main(int argc, const char** argv){
   UNITapp App;
   testApi(&App);
   testCFGfields(&App);
   COL_TRC("Made test collection.");
   return App.run(argc, argv);
}
