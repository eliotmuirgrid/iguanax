//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INIexample
//
// Description:
//
// Unit test for INI library
//
// Author: Eliot Muir 
// Date:   Fri 24 Mar 2023 12:19:26 EDT
//---------------------------------------------------------------------------
#include <UNIT/UNITapp.h>
#include "testIni.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

int main(int argc, const char** argv){
   // Run with as little boiler plate as possible.
   UNITapp App;
   testIni(&App);
   return App.run(argc, argv);
}
