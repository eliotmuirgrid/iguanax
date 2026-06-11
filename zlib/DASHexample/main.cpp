//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DASHexample
//
// Description:
//
// Unit test for the DASH library
//
// Author: Matthew Sobkowski
// Date:   08/07/25 2:19 PM
//---------------------------------------------------------------------------
#include "testApi.h"
#include "testCompare.h"
#include <COL/COLlog.h>
#include <UNIT/UNITapp.h>
COL_LOG_MODULE;

int main(int argc, const char** argv){
   UNITapp App;
   testApi(&App);
   testCompare(&App);
   COL_TRC("Made test collection.");
   return App.run(argc, argv);
}
