//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GZIPexample 
//
// Description:
//
// Unit test for the GZIP library which is just a bit of stuff around the
// zlib library.
//
// Author: Eliot Muir
// Date:   Fri 01/16/2004 
//---------------------------------------------------------------------------
#include <COL/COLlog.h>
COL_LOG_MODULE;

#include "testGzip.h"

#include <UNIT/UNITapp.h>

int main(int argc, const char** argv){
   UNITapp App;
   testGzip(&App);
   return App.run(argc, argv); 
}
