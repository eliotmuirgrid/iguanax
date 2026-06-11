//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNCexample
//
// Description:
//
// Unit test for the TRNC library
//
// Author: Eliot Muir
// Date:   Thu  8 Sep 2022 10:11:52 EDT
//---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <UNIT/UNITapp.h>
#include "testCleanup.h"
#include "testEditTags.h"
COL_LOG_MODULE;

int main(int argc, const char** argv){
   UNITapp App;
   testCleanup(&App);
   testEditTags(&App);
   COL_TRC("Made test collection.");
   return App.run(argc, argv);
}
