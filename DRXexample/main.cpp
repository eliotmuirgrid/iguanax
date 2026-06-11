//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DRXexample
//
// Description:
//
// Unit test for the DRX library using unit framework
//
// Author: Vismay Shah
// Date:   Tuesday 26 Jan 2024 09:37:20 EDT
//---------------------------------------------------------------------------

#include <UNIT/UNITapp.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

#include "testIguanaId.h"
#include "testLicense.h"

int main(int argc, const char** argv){
   UNITapp App;
   testIguanaId(&App);
   testLicense(&App);
   COL_TRC("Made test collection.");
   return App.run(argc, argv);
}
