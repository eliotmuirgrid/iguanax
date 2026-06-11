//---------------------------------------------------------------------------
// Copyright (C) 1997-2015 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SCKexample 
//
// Description: Unit test for the SCKS library
//
// Author: Kevin Cai
//---------------------------------------------------------------------------
#include <SCK/SCKutils.h>
#include <SCKS/SCKSsslUtils.h>
#include <UNIT/UNITapp.h>
#include "testSSL.h"

#include <FIL/FILutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

// Run with as little boiler plate as possible.   
int main(int argc, const char** argv){
   FILinitExecutableName(argv[0]);
   SCKinitWinsock();
   SCKSsslInit Init;
   UNITapp App;
   COL_TRC("Made test collection.");
   testSsl(&App);
   return App.run(argc, argv);  
}
