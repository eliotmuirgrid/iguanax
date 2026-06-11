//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: main.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   20/03/25 1:18 PM
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <UNIT/UNITapp.h>
COL_LOG_MODULE;

#include "testSort.h"
#include "testComponent.h"

int main(int argc, const char** argv) {
   UNITapp App;
   testSort(&App);
   testComponent(&App);
   COL_TRC("Made test collection.");
   return App.run(argc, argv);
}
