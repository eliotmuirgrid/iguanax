//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: main.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   6/5/23 1:59 PM
//  ---------------------------------------------------------------------------
#include <UNIT/UNITapp.h>
#include <FIL/FILutils.h>
#include <PACKexample/testPack.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

int main(int argc, const char** argv){
   FILinitExecutableName(argv[0]);
   UNITapp App;
   testPack(&App);
   return App.run(argc, argv);
}
