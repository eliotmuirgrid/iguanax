//---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INSTexample
//
// Description:
//
// Unit test for INST library
//
// Author: Eliot Muir
// Date:   Sun 23 Jul 2023 12:08:08 EDT
//---------------------------------------------------------------------------
#include <UNIT/UNITapp.h>
#include "testINSTupgrade.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

// Run with as little boiler plate as possible.   
int main(int argc, const char** argv){
   UNITapp App;
   testINSTupgrade(&App);
   return App.run(argc, argv);  
}
