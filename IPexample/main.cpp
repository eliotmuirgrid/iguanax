//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: IPexample 
//
// Description:
//
// Unit test for the IP library
//
// Author: Eliot Muir
// Date:   Wed 03/15/2023 
//---------------------------------------------------------------------------

#include <UNIT/UNITapp.h>

#include "testIPmine.h"

#include <SCK/SCKutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;


int main(int argc, const char** argv){
   SCKinitWinsock();
   UNITapp App;
   COL_TRC("Made test collection.");
   testIPmine(&App);
   return App.run(argc, argv);
}
