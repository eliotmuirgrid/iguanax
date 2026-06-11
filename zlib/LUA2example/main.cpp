// ---------------------------------------------------------------------------
//  Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: main.cpp
//
//  Description: Unit tests for LUAL library
//
//  Author: Eliot Muir
//  Date:   Wed 31 Aug 2022 15:03:57 EDT
// ---------------------------------------------------------------------------
#include <UNIT/UNITapp.h>
#include "testTable.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

int main(int argc, const char** argv){
   UNITapp App;
   testTable(&App);
   return App.run(argc, argv);
}
