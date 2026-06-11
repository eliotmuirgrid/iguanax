//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DB2example 
//
// Description:
//
// Unit test for the DB library done with UNIT library.
//
// Author: Eliot Muir
// Date:   Mon  7 Nov 2022 15:05:17 EST
//---------------------------------------------------------------------------

#include <UNIT/UNITapp.h>

#include "testSchema.h"
#include "testParser.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

int main(int argc, const char** argv){
   UNITapp App;
   COL_TRC("Made test collection.");
   testSchema(&App);
   testParser(&App);
   return App.run(argc, argv);
}
