//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: main.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   05/04/24 1:43 PM
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <UNIT/UNITapp.h>

#include "testHighlight.h"
#include "testSearch.h"
COL_LOG_MODULE;

int main(int argc, const char** argv) {
   UNITapp App;
   testHighlight(&App);
   testSearch(&App);
   return App.run(argc, argv);
}
