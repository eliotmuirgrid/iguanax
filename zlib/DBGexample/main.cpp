//---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBGexample 
//
// Description:
//
// Unit test for DBG library
//
// Author: Eliot Muir
// Date:   Wed 14 Jun 2023 08:49:12 EDT
//---------------------------------------------------------------------------
#include <UNIT/UNITapp.h>
#include "testDBGpdbGet.h"

#include <FIL/FILutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

// Run with as little boiler plate as possible.   
int main(int argc, const char** argv){
   FILinitExecutableName(argv[0]);
   UNITapp App;
   testDBGpdbGet(&App);
   return App.run(argc, argv);  
}
