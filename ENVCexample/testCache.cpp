//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testCache
//
// Description:
//
// Implementation
//
// Author: John Q
// Date:   Fri May  8 15:17:09 EDT 2026
//---------------------------------------------------------------------------
#include <chrono>
#include "testCache.h"

#include <UNIT/UNITapp.h>
#include <SCK/SCKloop.h>
#include <ENVC/ENVCcache.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static void testWindows(){
   COL_FUNCTION(testWindows);

   ENVCcacheInitOS();

   auto Start = std::chrono::high_resolution_clock::now();
   char* Path = getenv("Path");
   auto End = std::chrono::high_resolution_clock::now();
   std::chrono::duration<double> Raw = End - Start;

   Start = std::chrono::high_resolution_clock::now();
   Path = ENVCgetEnv("Path");
   End = std::chrono::high_resolution_clock::now();
   std::chrono::duration<double> Cached = End - Start;

   COLcout << "[ getenv | ENVCgetEnv ] took [ " << Raw.count() * 1000 << "ms | " << Cached.count() * 1000 << "ms ]" <<  newline;
}

void testCache(UNITapp* pApp){
   pApp->add("cache/windows", &testWindows);
}