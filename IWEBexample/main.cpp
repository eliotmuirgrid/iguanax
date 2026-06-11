//---------------------------------------------------------------------------
// Copyright (C) 1997-2004 iNTERFACEWARE Corp.  All Rights Reserved
//
// Module: main.cpp
//
// Description:
//
// An example of using the IWEB library.
//
// Author: Shuo Hou
// Date:   March 12, 2021
//---------------------------------------------------------------------------
#include "IWEBexampleTests.h"

#include <COL/COLlog.h>
#include <UNIT/UNITapp.h>
COL_LOG_MODULE;

// Run with as little boiler plate as possible.
int main(int argc, const char** argv){
   UNITapp App;
   IWEBtestPlace(&App);
   return App.run(argc, argv);
}
