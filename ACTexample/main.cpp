//---------------------------------------------------------------------------
// Copyright (C) 1997-2020 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: ACKexample 
//
// Description:
//
// Unit test for the ACK library
//
// Author: Eliot Muir
// Date:   Mon 19 Sep 2022 10:49:44 EDT
//---------------------------------------------------------------------------

#include <UNIT/UNITapp.h>

#include <CMD/CMDlineParser.h>

#include <stdlib.h>

#include <COL/COLerror.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#include "testSerial.h"

int main(int argc, const char** argv){
   UNITapp App;
   testSerial(&App);
   return App.run(argc, argv);
}
