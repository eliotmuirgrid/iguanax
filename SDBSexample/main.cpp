//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: main.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   2025-08-27, 12:14p.m.
//  ---------------------------------------------------------------------------
#include <FIL/FILutils.h>
#include <UNIT/UNITapp.h>
#include <SDB/SDBcontent.h>
#include "testRetrieveParams.h"
#include "testRetrieveSync.h"

int main(int argc, const char** argv){
   FILinitExecutableName(argv[0]);
   SDBcontentMapInit();
   UNITapp App;
   testRetrieveParams(&App);
   testRetrieveSync(&App);
   return App.run(argc, argv);
}