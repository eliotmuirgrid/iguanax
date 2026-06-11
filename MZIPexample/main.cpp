//---------------------------------------------------------------------------
// Copyright (C) 1997-2014 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: main.cpp
//
// Description:
//
// Unit test for the MZIP library. 
//
// Author: Eliot Muir
// Date:   
//---------------------------------------------------------------------------
#include <UNIT/UNITapp.h>
#include "testMZIP.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

// Run with as little boiler plate as possible.   
int main(int argc, const char** argv){
   UNITapp App;
   COL_TRC("Made test collection.");
   testMZIP(&App);
   return App.run(argc, argv);  
}
