//---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITexample
//
// Description:
//
// Unit test for the GIT library
//
// Author: Eliot Muir
// Date:   Mon 13 Feb 2023 10:00:05 EST
//---------------------------------------------------------------------------
#include "testClone.h"
#include "testCommitInfo.h"
#include "testGlobal.h"
#include "testPath.h"
#include "testSshkeygenDetect.h"
#include "testUrl.h"
#include "testVersion.h"
#include "testRemote.h"
#include "testGITcache.h"
#include "testGITlist.h"
#include "testGITmodules.h"
#include "testKnownHosts.h"

#include <FIL/FILutils.h>
#include <GSV/GSVmap.h>
#include <UNIT/UNITapp.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

// Run with as little boiler plate as possible.
int main(int argc, const char** argv){
   FILinitExecutableName(argv[0]);
   COLstring Error;
   UNITapp App;
   // GSVmapInit();
   testClone(&App);
    testGlobal(&App);
   // testCommitInfo(&App);
   // testKnownHosts(&App);
   // testPath(&App);
   // testSshkeygenDetect(&App);
   // testUrl(&App);
   // testVersion(&App);
   // testRemote(&App);
   // testGITcache(&App);
   // testGITUlist(&App);
   // testGITUmodules(&App);
   return App.run(argc, argv);
}
