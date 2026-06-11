//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: HELPexample
//
// Description:
//
// Unit test for the HELP library
//
// Author: Orkhan
// Date:   Thu  8 Sep 2022 09:37:20 EDT
//---------------------------------------------------------------------------

#include <UNIT/UNITapp.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

#include "testApi.h"

int main(int argc, const char** argv){
   UNITapp App;
   testApi(&App);
   COL_TRC("Made test collection.");
   return App.run(argc, argv);
}
