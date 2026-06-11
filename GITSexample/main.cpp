//---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITSexample
//
// Description:
//
// Unit test for the GITS library
//
// Author: Matthew Sobkowski
// Date:   Monday 10 April 2023
//---------------------------------------------------------------------------
#include <GITSexample/testGitInstall.h>

#include <UNIT/UNITapp.h>

#include <SCK/SCKutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

int main(int argc, const char** argv){
   SCKinitWinsock();
   UNITapp App;
   testGitInstall(&App);
   return App.run(argc, argv);
}
