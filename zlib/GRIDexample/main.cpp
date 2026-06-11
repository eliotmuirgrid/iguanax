//---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GRID2example
//
// Description:
//
// Unit test for new GRID library
//
// Author: Eliot Muir
// Date:   Sun  9 Apr 2023 10:01:06 EDT
//---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <UNIT/UNITapp.h>

#include "testSecurity.h"
#include "testSort.h"
COL_LOG_MODULE;

int main(int argc, const char** argv){
   UNITapp App;
   testSecurity(&App);
   testSort(&App);
   return App.run(argc, argv);
}
