//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNexample
//
// Description:
//
// Unit test for TRN library
//
// Author: Vismay Shah
// Date:   Fri 04/08/2022
//---------------------------------------------------------------------------
#include <UNIT/UNITapp.h>
#include "testApp.h"
#include "testTRNblockCompletion.h"
#include "testTRNideAnnotation.h"
#include "testTRNideIntellisense.h"
#include "testTRNcompile.h"
#include "testTRNbinaryFile.h"

#include <SEC/SECaccess.h>

#include <FIL/FILutils.h>
#include <PASS/PASSword.h>

#include <DB/DBdatabaseFactory.h>

#include <COL/COLhashmap.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

// Run with as little boiler plate as possible.
int main(int argc, const char** argv){
   FILinitExecutableName(argv[0]);
   UNITapp App;
   PASSsetKeyIV("test", "test");
   COL_TRC("Made test collection.");
   testApp(&App);
   testTRNblockCompletion(&App);
   testTRNideAnnotation(&App);
   testTRNideIntellisense(&App);
   testTRNcompile(&App);
   testTRNbinaryFile(&App);
   DBdatabaseFactory DbFactory;
   DbFactory.initialize(COLcout);
   COLhashmap<COLstring, COLstring> HelpMap;
   loadHelp(&HelpMap);

   LUAsetOnLuaStateCreated(COLnewClosure2(&testRegister, &DbFactory, &HelpMap));

   SECinit(true);

   return App.run(argc, argv);
}
