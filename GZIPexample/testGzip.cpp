//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testGzip
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Tue 11 Nov 2014 14:03:22 EST
//---------------------------------------------------------------------------

#include "testGzip.h"

#include <UNIT/UNITapp.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

#include "GZIP/GZIPutils.h"
#include "GZIP/GZIPcompressFilter.h"

#include "FIL/FILutils.h"
#include "FIL/FILfile.h"

static void testSimple(){
   COL_FUNCTION(testSimple);
   COLstring Input;
   COL_TRC("Read file.");
   FILreadFile("./testGzip.cpp", &Input);
   COL_TRC("Loaded file.");
   COLstring Out2;
   COLsinkString Sink(Out2);
   GZIPcompressFilter Filter(&Sink);
   Filter.write(Input.c_str(), Input.size());
   Filter.onEndStream();
   COL_TRC("Done first compression with filter.");
   COLstring Out;
   GZIPcompress(Input, &Out);
   COL_VAR3(Input.size(), Out.size(), Out2.size());
   COLstring Uncompressed;
   GZIPuncompress(Out2, &Uncompressed);
   UNIT_ASSERT_EQUALS(Input, Uncompressed);
}

static void testSmall(){
   COL_FUNCTION(testSmall);
   // small strings are likely to be larger after gzip compression.
   COLstring Orig, Compressed, Uncompressed;
   Orig = "ABC";
   GZIPcompress(Orig, &Compressed);
   GZIPuncompress(Compressed, &Uncompressed);
   COL_VAR3(Orig.size(), Compressed.size(), Uncompressed.size());
   UNIT_ASSERT_EQUALS(Orig, Uncompressed);
}

static void testFile(){
   COL_FUNCTION(testFile);
   GZIPcompressFile("testGzip.cpp", "testGzip.cpp.gz");
   GZIPuncompressFile("testGzip.cpp.gz", "testGzip.cpp.copy");
   COLstring Orig, Copy;
   FILreadFile("testGzip.cpp", &Orig);
   FILreadFile("testGzip.cpp.copy", &Copy);
   UNIT_ASSERT_EQUALS(Orig,Copy);
}

void  testGzip(UNITapp* pApp){
   pApp->add("gzip/simple", &testSimple);
   pApp->add("gzip/small",  &testSmall);
   pApp->add("gzip/file", &testFile);
}

