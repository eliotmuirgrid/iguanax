//---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBAexample
//
// Description:
//
// Unit test for the SDB library
//
// Author: Eliot Muir
// Date:   Thu 16 Feb 2023 15:58:31 EST
//---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <FIL/FILutils.h>
#include <SDB/SDBcontent.h>
#include <UNIT/UNITapp.h>

#include "testInput.h"
#include "testMemory.h"
#include "testDirCheck.h"
#include "testTwoLogDirs.h"
COL_LOG_MODULE;

// Run with as little boiler plate as possible.
int main(int argc, const char** argv) {
   FILinitExecutableName(argv[0]);
   SDBcontentMapInit();
   UNITapp App;
   testInput(&App);
   // testMemory(&App); // After the rework, this test no longer makes sense.
   testDirCheck(&App);
   testTwoLogDirs(&App);
   return App.run(argc, argv);
}
