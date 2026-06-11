//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: IDBexample
//
// Description:
//
// Unit test for the IDB library
//
// Author: Julian Muir
//---------------------------------------------------------------------------
#include <UNIT/UNITapp.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

#include "testApi.h"

int main(int argc, const char** argv){
   UNITapp App;
   testApi(&App);
   COL_TRC("Added  tests");
   return App.run(argc,argv);
}