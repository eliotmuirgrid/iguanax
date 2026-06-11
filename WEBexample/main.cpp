//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: WEB2example 
//
// Description:
//
// Unit test for the WEB library
//
// Author: Eliot Muir
// Date:   Wed 03/15/2023 
//---------------------------------------------------------------------------

#include <UNIT/UNITapp.h>

#include <CMD/CMDlineParser.h>

#include "testClient.h"
#include "testServer.h"
#include "testSession.h"
#include "testFileIndirection.h"
#include "testParse.h"
#include "testRestart.h"

#include <FIL/FILutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <SCK/SCKutils.h>

int main(int argc, const char** argv){
   FILinitExecutableName(argv[0]);
   SCKinitWinsock();
   UNITapp App;
   COL_TRC("Made test collection.");
   testClient(&App);
   testServer(&App);
   testFileIndirection(&App);
   testParse(&App);
   testRestart(&App);
   testSession(&App);
   return App.run(argc, argv);
}
