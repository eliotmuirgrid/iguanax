//---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNexample 
//
// Description:
//
// Unit test for TUN library
//
// Author: Eliot Muir
// Date:   Wed 14 Jun 2023 08:49:12 EDT
//---------------------------------------------------------------------------
#include <UNIT/UNITapp.h>
#include "testOpenSsl.h"
#include "testTUNnumber.h"
#include "testTUNrecord.h"
#include "testTUNpkcs7Pad.h"
#include "testTUNaes.h"
#include "testTUNhash.h"
#include "testTUNclientLoad.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

// Run with as little boiler plate as possible.   
int main(int argc, const char** argv){
   UNITapp App;
   testOpenSsl(&App);
   testTUNnumber(&App);
   testTUNrecord(&App);
   testTUNpkcs7Pad(&App);
   testTUNaes(&App);
   testTUNhash(&App);
   testTUNclientLoad(&App);
   return App.run(argc, argv);  
}
