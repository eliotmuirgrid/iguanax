//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: UNITexample 
//
// Description:
//
// Unit test for UNIT library
//
// Author: Eliot Muir
// Date:   Fri 01/16/2004 
//---------------------------------------------------------------------------
#include <UNIT/UNITapp.h>
#include "testApp.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

// Run with as little boiler plate as possible.   
int main(int argc, const char** argv){
   UNITapp App;
   COL_TRC("Made test collection.");
   testApp(&App);
   return App.run(argc, argv);  
}
