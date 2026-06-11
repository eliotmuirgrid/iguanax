//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: UNITexample 
//
// Description:
//
// Unit test for INFO library
//
// Author: Matthew Sobkowski
// Date:   Thu 05/13/2021 
//---------------------------------------------------------------------------
#include <UNIT/UNITapp.h>
#include "testApp.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

// Run with as little boiler plate as possible.   
int main(int argc, const char** argv){
   UNITapp App;
   COL_TRC("INFO test");
   testApp(&App);
   return App.run(argc, argv);  
}
