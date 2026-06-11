//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBexample 
//
// Description:
//
// Unit test for DB library
//
// Author: Eliot Muir
// Date:   Wed 14 Apr 2021 15:16:53 EDT
//---------------------------------------------------------------------------
#include <UNIT/UNITapp.h>
#include "testInit.h"
#include "testMySql.h"
#include "testMariaDb.h"
#include "testOracleOci.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

// Run with as little boiler plate as possible.   
int main(int argc, const char** argv){
   UNITapp App;
   COL_TRC("Made test collection.");
   testInit(&App);
   testMySql(&App);
   testMariaDb(&App);
   testOracleOci(&App);
   return App.run(argc, argv);  
}
