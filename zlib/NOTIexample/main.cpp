// ---------------------------------------------------------------------------
//  Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: main.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:  8/11/21, 1:47 PM
// ---------------------------------------------------------------------------
#include <UNIT/UNITapp.h>
#include "testApp.h"
#include "testDetect.h"


#include <COL/COLlog.h>
COL_LOG_MODULE;

int main(int argc, const char** argv){
   UNITapp App;
   testApp(&App);
   testDetect(&App);
   return App.run(argc, argv);
}
