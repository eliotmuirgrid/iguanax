//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: FSBexample 
//
// Description:
//
// Unit test for FSB library
//
// Author: Eliot Muir
// Date:   Thu 18 Mar 2021 17:15:21 EDT
//---------------------------------------------------------------------------
#include <UNIT/UNITapp.h>
#include "testFSBextractPath.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

// Run with as little boiler plate as possible.   
int main(int argc, const char** argv){
   UNITapp App;
   COL_TRC("Made test collection.");
   testFSBextractPath(&App);
   return App.run(argc, argv);  
}
