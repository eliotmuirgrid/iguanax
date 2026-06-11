//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: testPack.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   6/5/23 2:00 PM
//  ---------------------------------------------------------------------------
#include <PACKexample/testPack.h>

#include <CACHE/CACHEfile.h>

#include <PACK/PACKwebPack.h>
#include <PACK/PACKattach.h>
#include <PACK/PACKbinToC.h>

#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <UNIT/UNITapp.h>

#include <COL/COLvar.h>
#include <COL/COLlog.h>
#include "BZP/bzlib_private.h"
COL_LOG_MODULE;

static void printVar(const COLvar& Result) {
   COLstring Bin;
   Result.toBinary(&Bin);
   COL_VAR2(Result["hash"].asString(), Bin.size());
   COLvar CacheCopy = Result;
   CACHEsetCache(&CacheCopy);
   COLvar HelpVar;
   CACHEgetHelpList(&HelpVar);
}

static void testPacker() {
   COLstring IguanaAppBin   = FILpathAppend("Iguana", FILpathAppend("app", "web"));
   COLstring IguanaAppHlp   = FILpathAppend("Iguana", FILpathAppend("app", "help"));
   COLstring IguanaAppVmd   = FILpathAppend("Iguana", FILpathAppend("app", "vmd"));
   COLstring PathToWebFiles = FILpathAppend(FILparentDir(FILworkingDir()), IguanaAppBin);
   COLstring PathToHlpFiles = FILpathAppend(FILparentDir(FILworkingDir()), IguanaAppHlp);
   COLstring PathToVmdFiles = FILpathAppend(FILparentDir(FILworkingDir()), IguanaAppVmd);
   COLarray<COLstring> DirList;
   DirList.push_back(PathToWebFiles);
   DirList.push_back(PathToHlpFiles);
   DirList.push_back(PathToVmdFiles);
   PACKwebPack(DirList, COLnewClosure1(printVar));
}

static void testFind() {
   COL_FUNCTION(testFind);
   // base test case
   COLstring SubStr = PACK_MAGIC_STRING;
   COLstring Test = "h" + SubStr + "world";
   int Position;
   UNIT_ASSERT(PACKfind(Test, SubStr, &Position));
   UNIT_ASSERT_EQUALS(1, Position);

   // buffer has null bytes
   Test.get_buffer()[0] = '\0';
   COL_TRC_DUMP("test", Test.data(), Test.size());
   UNIT_ASSERT(PACKfind(Test, SubStr, &Position));
   UNIT_ASSERT_EQUALS(1, Position);
}

static void PACKTcreateTestData(COLvar* pData) {
   COLvar& Data = *pData;
   Data["number"] = 42;
   Data["boolean"] = true;
   Data["string"] = "hello";
   Data["array"][0] = 3.14;
   Data["array"][1] = false;
   Data["map"]["key"] = "value";
}

static void testAttach() {
   COL_FUNCTION(testAttach);
   COLstring SourceBin = FILexecutableDir() + FILexecutableFileName();
   COLstring CarrierBin = SourceBin + "pack";
   FILcopyFile(SourceBin, CarrierBin);
   COLstring Error;
   COLvar DataIn, DataOut;
   PACKTcreateTestData(&DataIn);
   PACKattachToExe(DataIn, CarrierBin);
   UNIT_ASSERT(PACKextractFromExe(CarrierBin, &DataOut, &Error));
   UNIT_ASSERT_EQUALS(DataIn, DataOut);
   UNIT_ASSERT(Error.is_null());
   FILremoveWithThrow(CarrierBin, true);
}

static void testBinToC(){
   COL_FUNCTION(testBinToC);
   COLstring Data = "The quick brown fox jumped over the lazy dog.";
   COLstring Out;
   PACKbinToC(Data, &Out);
   COL_VAR(Out);
}

void testPack(UNITapp* pApp) {
   // pApp->add("pack", &testPacker); // only use to ensure packer works correctly locally
   pApp->add("pack/find", &testFind);
   pApp->add("pack/attach", &testAttach);
   pApp->add("pack/bintoc", &testBinToC);
}
