//---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITEexample
//
// Description:
//
// Unit test for the GITE library
//
// Author: Eliot Muir
// Date:   Mon 13 Feb 2023 10:00:05 EST
//---------------------------------------------------------------------------
#include "testBitbucket.h"
#include "testGithub.h"
#include "testGitlab.h"
#include "testGeneral.h"

#include <UNIT/UNITapp.h>

#include <SCK/SCKutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

// Run with as little boiler plate as possible.
int main(int argc, const char** argv){
   SCKinitWinsock();
   UNITapp App;
   testGeneral(&App);
   // Set up configurations and run these tests locally.
   // testBitbucket(&App); // Warning: these tests create/affect the interfaceware repository.
   // testGithub(&App);
   // testGitlab(&App);
   return App.run(argc, argv);
}
