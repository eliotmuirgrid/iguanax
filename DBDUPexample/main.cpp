//---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBDUPexample
//
// Description:
//
// Unit test for the DBDUP library
//
// Author: Vismay Shah
// Date:   Fri 17 Jan 2025 12:47pm
//---------------------------------------------------------------------------
#include <DBDUPexample/testVersion.h>

#include <UNIT/UNITapp.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

// Run with as little boiler plate as possible.
int main(int argc, const char** argv){
   UNITapp App;
   testVersion(&App);
   return App.run(argc, argv);
}
