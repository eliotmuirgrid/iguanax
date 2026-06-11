//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBIMexample 
//
// Description:
//
// Unit test for DBIM library
//
// Author: Vismay Shah
// Date:   Mon 27 Nov 2023 5:44:17 pm
//---------------------------------------------------------------------------
#include <UNIT/UNITapp.h>
#include "testGenerate.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

// Run with as little boiler plate as possible.   
int main(int argc, const char** argv){
   UNITapp App;
   COL_TRC("Made test collection.");
   testGenerate(&App);
   return App.run(argc, argv);  
}
