//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: FILexample 
//
// Description:
//
// Unit test for FIL library
//
// Author: Eliot Muir
// Date:   Thu 18 Mar 2021 17:15:21 EDT
//---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <FIL/FILutils.h>
#include <UNIT/UNITapp.h>

#include "testSecurity.h"
COL_LOG_MODULE;

int main(int argc, const char** argv){
   FILinitExecutableName(argv[0]);
   UNITapp App;
	testDirectoryAccess(&App);
   return App.run(argc, argv);
}
