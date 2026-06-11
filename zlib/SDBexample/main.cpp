//---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBexample
//
// Description:
//
// Unit test for the SDB library
//
// Author: Eliot Muir
// Date:   Thu 16 Feb 2023 15:58:31 EST
//---------------------------------------------------------------------------
#include "testCache.h"
#include "testComponent.h"
#include "testContent.h"
#include "testCrypto.h"
#include "testDateTime.h"
#include "testDequeue.h"
#include "testDir.h"
#include "testEnvelope.h"
#include "testFile.h"
#include "testFilter.h"
#include "testFind.h"
#include "testInit.h"
#include "testLog.h"
#include "testMagicMarker.h"
#include "testMessage.h"
#include "testPosition.h"
#include "testPositionMap.h"
#include "testPurge.h"
#include "testReader.h"
#include "testRecovery.h"
#include "testServiceLog.h"
#include "testStringNumber.h"
#include "testSystem.h"
#include "testSDBepochTime.h"

#include <SDB/SDBcontent.h>

#include <FIL/FILutils.h>

#include <UNIT/UNITapp.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

// Run with as little boiler plate as possible.
int main(int argc, const char** argv){
   FILinitExecutableName(argv[0]);
   SDBcontentMapInit();
   COLstring Error;
   UNITapp App;
   testCache(&App);
   testComponent(&App);
   testContent(&App);
   testCrypto(&App);
   testDateTime(&App);
   testDequeue(&App);
   testDequeueMultiple(&App);
   testDir(&App);
   testEnvelope(&App);
   testFind(&App);
   testFile(&App);
   testFilter(&App);
   testInit(&App);
   testLog(&App);
   testMagicMarker(&App);
   testMessage(&App);
   testPosition(&App);
   testPositionMap(&App);
   testPurge(&App);
   testReader(&App);
   testRecovery(&App);
   testStringNumber(&App);
   testServiceLog(&App);
   testSystem(&App);
   testSDBepochTime(&App);
   return App.run(argc, argv);
}
