//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: tesCOLdll.cpp
//
// Description:
//
// 
// Author: Guillaume Couture-Levesque
// Date:   June 3rd, 2008
//
//
//---------------------------------------------------------------------------
#include "COLexamplePrecomp.h"
#pragma hdrstop

#include <COL/COLdll.h>
#include <TST/TSTtestCollection.h>
#include <TST/TSTtestFunction.h>
#include <TST/TSThelpers.h>

void testCOLdllRealFunction(COLdll& Dll)
{
  void* RealFunction = NULL;
  try
  {
      RealFunction = Dll.getProcAddress("mysql_init");
      TST_ASSERT_MESSAGE(RealFunction != NULL, "Unable to load function mysql_init.");
  }
  catch(...)
  {
      TST_ASSERT_MESSAGE(false, "Unable to load function mysql_init.");
  }
}

void testCOLdllFakeFunction(COLdll& Dll)
{
  void* FakeFunction = NULL;
  try
  {
     FakeFunction = Dll.getProcAddress("thisisafakefunctionasdfasdf");
     TST_ASSERT_MESSAGE(false, "Managed to load fake function.");
  }
  catch(...)
  {
     TST_ASSERT_MESSAGE(FakeFunction == NULL, "Managed to load fake function.");
  }
}

void testCOLdllRealDll()
{
  COLdll RealDll;
  bool DllLoaded = false;

//there is unfortunately quite a few different ways of loading MySQL
#if defined(_WIN32)
  DllLoaded = (RealDll.tryLoadLibrary("libmySQL.dll") == COL_DLL_LOAD_OK);
#elif defined(__APPLE__)
  DllLoaded = (RealDll.tryLoadLibrary("libmysqlclient_r.dylib")  == COL_DLL_LOAD_OK) || (RealDll.tryLoadLibrary("libmysqlclient_r.so")  == COL_DLL_LOAD_OK);
#else 
  DllLoaded = (RealDll.tryLoadLibrary("libmysqlclient_r.so")  == COL_DLL_LOAD_OK);
#endif
  TST_ASSERT_MESSAGE(DllLoaded, "Unable to try loading MySQL library.");

  //next without try
  try
  {
     #if defined(_WIN32)
        RealDll.loadLibrary("libmySQL.dll");
     #elif defined(__APPLE__)
        bool failed = false;
        try
        {
           RealDll.loadLibrary("libmysqlclient_r.dylib");
        }
        catch(...)
        {
           failed = true;
        }
        if(failed)
        {
           RealDll.loadLibrary("libmysqlclient_r.so");
        }
     #else 
        RealDll.loadLibrary("libmysqlclient_r.so");
     #endif
  }
  catch(...)
  {
     TST_ASSERT_MESSAGE(false, "Unable to load MySQL library.");
  }

  //while we've got these loaded, test the function loading
  testCOLdllRealFunction(RealDll);
  testCOLdllFakeFunction(RealDll);
}

void testCOLdllFakeDll()
{
   COLdll FakeDll;
   bool DllLoaded;
   DllLoaded = (FakeDll.tryLoadLibrary("thisisafakedll.dll") == COL_DLL_LOAD_OK);
   TST_ASSERT_MESSAGE(!DllLoaded, "Managed to load fake dll: thisisafakedll.dll");

   try
   {
      FakeDll.tryLoadLibrary("thisisafakedll.dll");
      TST_ASSERT_MESSAGE(false, "Managed to load fake dll: thisisafakedll.dll");
   }
   catch(...)
   {
   }
}

void testCOLdllNoDll()
{
   COLdll NoDll;
   bool DllLoaded;
   DllLoaded = (NoDll.tryLoadLibrary("thisdlldoesnotexist.dll") == COL_DLL_LOAD_OK);
   TST_ASSERT_MESSAGE(!DllLoaded, "Managed to load unknown dll: thisdlldoesnotexist.dll");

   try
   {
      NoDll.loadLibrary("thisdlldoesnotexist.dll");
      TST_ASSERT_MESSAGE(!DllLoaded, "Managed to load unknown dll: thisdlldoesnotexist.dll");
   }
   catch(...)
   {
   }
}

TSTtestable* COLdllTests() 
{
   TST_START_COLLECTION("COLdllTests")
      TST_ADD_TEST_FUNCTION(testCOLdllFakeDll);
      TST_ADD_TEST_FUNCTION(testCOLdllNoDll);
   TST_END_COLLECTION
}
