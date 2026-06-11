//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: testEnvParser.cpp
//
//  Description:
//
//  Implementation
//
//  Author: Matthew Sobkowski
//  Date:   3/24/23 10:22 AM
//  ---------------------------------------------------------------------------
#include <ENVexample/testlocal.h>

#include <ENV/ENVparse.h>
#include <ENV/ENVfile.h>
#include <FIL/FILutils.h>

#include <UNIT/UNITapp.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static COLstring simpleString() { return "key1=value1;value2\nkey2=value3;value4;value5\n"; }
static COLstring chmString()    { return "CHM_PYTHON_LIB_PATH=C:\\Program Files\\iNTERFACEWARE\\Iguana\\lib;C:\\Program Files (x86)\\iNTERFACEWARE\\Chameleon\\lib\n"; }

static void testSerializeFile() {
   COL_FUNCTION(testSerializeFile);
   COLstring Env, Err;
   FILreadFile("testEnv.txt", &Env);
   ENVmap EnvMap;
   ENVparse(&Env, &EnvMap, &Err);
   COLvar Result;
   ENVfileToVar(EnvMap, &Result);
   UNIT_ASSERT_EQUALS(1520, Result["Path"].asString().size());
}

static void testSerializeChm() {
   COL_FUNCTION(testSerializeChm);
   COLstring Env = chmString();
   ENVmap    EnvMap;
   COLstring Err;
   ENVparse(&Env, &EnvMap, &Env);
   COLvar Result;
   ENVfileToVar(EnvMap, &Result);
   COLvar Expected;
   Expected["CHM_PYTHON_LIB_PATH"] = "C:\\Program Files\\iNTERFACEWARE\\Iguana\\lib;C:\\Program Files (x86)\\iNTERFACEWARE\\Chameleon\\lib";
   UNIT_ASSERT_EQUALS(Expected, Result);
}

static void testSerializeSimple() {
   COL_FUNCTION(testSerializeSimple);
   COLstring Env = simpleString();
   ENVmap    EnvMap;
   COLstring Err;
   ENVparse(&Env, &EnvMap, &Env);
   COLvar Result;
   ENVfileToVar(EnvMap, &Result);
   COLvar Expected;
   Expected["key1"] = "value1;value2";
   Expected["key2"] = "value3;value4;value5";
   UNIT_ASSERT_EQUALS(Expected, Result);
}

static void testSerializeEmpty() {
   COL_FUNCTION(testSerializeEmpty);
   COLstring Env = "";
   ENVmap    EnvMap;
   COLstring Err;
   ENVparse(&Env, &EnvMap, &Env);
   COLvar Result;
   ENVfileToVar(EnvMap, &Result);
   UNIT_ASSERT(Result.isMap());
}

static void testParseFile() {
   COL_FUNCTION(testParseFile);
   COLstring Env, Err;
   FILreadFile("testEnv.txt", &Env);
   ENVmap EnvMap;
   ENVparse(&Env, &EnvMap, &Err);
   UNIT_ASSERT(EnvMap.Map.count("Path"));
   UNIT_ASSERT_EQUALS(1520, EnvMap.Map["Path"].size());
}

static void testParseChm() {
   COL_FUNCTION(testParseChm);
   COLstring Env = chmString();
   COLvar Expected;
   Expected["CHM_PYTHON_LIB_PATH"] = "C:\\Program Files\\iNTERFACEWARE\\Iguana\\lib;C:\\Program Files (x86)\\iNTERFACEWARE\\Chameleon\\lib";
   ENVmap Result;
   COLstring Err;
   ENVparse(&Env, &Result, &Err);
}

static void testParseEmpty() {
   COL_FUNCTION(testParseEmpty);
   COLstring Env = "";
   ENVmap    EnvMap;
   COLstring Err;
   ENVparse(&Env, &EnvMap, &Err);
   UNIT_ASSERT_EQUALS(0, EnvMap.Map.size());
}

static void testParseSimple() {
   COL_FUNCTION(testParseSimple);
   COLstring Env = simpleString();
   ENVmap    EnvMap;
   COLstring Err;
   ENVparse(&Env, &EnvMap, &Err);
   UNIT_ASSERT_EQUALS(2, EnvMap.Map.size());
}

void testENVParser(UNITapp* pApp) {
   pApp->add("env/parse/empty",      &testParseEmpty);
   pApp->add("env/parse/simple",     &testParseSimple);
   pApp->add("env/parse/chm",        &testParseChm);
   pApp->add("env/parse/file",       &testParseFile);
   pApp->add("env/serialize/empty",  &testSerializeEmpty);
   pApp->add("env/serialize/simple", &testSerializeSimple);
   pApp->add("env/serialize/chm",    &testSerializeChm);
   pApp->add("env/serialize/file",   &testSerializeFile);
}
