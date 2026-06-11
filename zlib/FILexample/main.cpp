//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: FILexample 
//
// Description:
//
// Unit test for FIL library
//
// Author: Eliot Muir
// Date:   Thu 18 Mar 2021 17:15:21 EDT
//---------------------------------------------------------------------------
#include <UNIT/UNITapp.h>
#include "testFILutils.h"
#include "testFILpathUtils.h"
#include "testFILfile.h"
#include "testFILworkingDir.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <FIL/FILutils.h>

// Run with as little boiler plate as possible.   
int main(int argc, const char** argv){
   FILinitExecutableName(argv[0]);
   UNITapp App;
   COL_TRC("Made test collection.");
   testFILutils(&App);
   testFILpathUtils(&App);
   testFILfile(&App);
   testFILworkingDir(&App);
   return App.run(argc, argv);  
}
