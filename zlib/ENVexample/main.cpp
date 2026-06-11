//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: main.cpp
//
//  Description:
//
//  Implementation
//
//  Author: Matthew Sobkowski
//  Date:   3/24/23 10:21 AM
//  ---------------------------------------------------------------------------
#include <UNIT/UNITapp.h>

#include <ENVexample/testlocal.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

int main(int argc, const char** argv){
   UNITapp App;
   testENVParser(&App);
   return App.run(argc, argv);
}
