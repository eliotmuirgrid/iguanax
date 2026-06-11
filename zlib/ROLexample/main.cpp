//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: main.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   24/01/24 4:14 PM
//  ---------------------------------------------------------------------------
#include <UNIT/UNITapp.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

#include "testRoles.h"


int main(int argc, const char** argv){
   UNITapp App;
   testRoles(&App);
   return App.run(argc, argv);
}
