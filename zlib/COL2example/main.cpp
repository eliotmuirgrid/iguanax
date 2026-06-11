//---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: COL2example
//
// Description:
//
// Unit test for the COL library using the new unit test framework
// I like it better that TST so writing new unit tests here for new COL code.
//
// Author: Eliot Muir
// Date:   Wed 29 Mar 2023 10:14:29 EDT
//---------------------------------------------------------------------------
#include "testVarUtils.h"
#include "testMath.h"
#include "testGlobMatch.h"
#include "testJsonParser.h"
#include "testEvent.h"

#include <UNIT/UNITapp.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

// Run with as little boiler plate as possible.
int main(int argc, const char** argv){
   UNITapp App;
   testVarUtils(&App);
   testMath(&App);
   testGlobMatch(&App);
   testJsonParser(&App);
   testEvent(&App);
   return App.run(argc, argv);
}
