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
#include <UNIT/UNITapp.h>

#include "testHash.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

int main(int argc, const char** argv){
   UNITapp App;
   testHash(&App);
   return App.run(argc, argv);
}
