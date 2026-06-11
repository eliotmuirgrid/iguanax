//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: EDIexample 
//
// Description:
//
// Unit test for the EDI library
//
// Author: Eliot Muir
// Date:   Tue 26 Jul 2022 14:39:56 EDT
//---------------------------------------------------------------------------

#include <UNIT/UNITapp.h>


#include "testCopy.h"
#include "testFormatter.h"
#include "testJsonFormat.h"
#include "testJsonLoad.h"
#include "testParser.h"
#include "testParser2.h"
#include "testPreprocess.h"
#include "testRandom.h"
#include "testvmdParser.h"
#include "testX12Parser.h"
#include "testX12PreProcess.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

int main(int argc, const char** argv){
   UNITapp App;
   COL_TRC("Made test collection.");
   testFormatter(&App);
   testJsonFormat(&App);
   testJsonLoad(&App);
   testPreprocess(&App);
   testVmdParser(&App);
   testParser(&App);
   testParser2(&App);
   testX12Parser(&App);
   testX12PreProcess(&App);
   testCopy(&App);
   testRandom(&App);
   return App.run(argc, argv);
}
