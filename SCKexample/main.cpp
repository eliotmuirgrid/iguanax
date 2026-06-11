//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SCKexample 
//
// Description:
//
// Unit test for the SCK library
//
// Author: Eliot Muir
// Date:   Fri 01/16/2004 
//---------------------------------------------------------------------------

#include <UNIT/UNITapp.h>

#include <CMD/CMDlineParser.h>

#include <stdlib.h>

#include <COL/COLerror.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#include "testCallMain.h"
#include "testIp.h"
#include "testSimpleNetwork.h"
#include "testSet2.h"
#include "testKqueue.h"
#include "testTimer.h"
#include "testThreading.h"
#include "testWorker.h"

#include <SCK/SCKutils.h>
#include <SCK/SCKerror.h>

int main(int argc, const char** argv){
   SCKinitWinsock();
   UNITapp App;
   COL_TRC("Made test collection.");
   testSet2(&App);
   testThreading(&App);
#ifdef __APPLE__
   testKqueue(&App);
#endif
   testSimpleNetwork(&App);
   testIp(&App);
   testTimer(&App);
   testWorker(&App);
   testCallMain(&App);
   return App.run(argc, argv);
}
