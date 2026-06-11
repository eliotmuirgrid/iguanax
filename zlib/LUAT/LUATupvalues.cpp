//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LUATupvalues
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Friday, September 30th, 2011 @ 02:34:48 PM
//
//---------------------------------------------------------------------------
#include "LUATprecomp.h"
#pragma hdrstop

#include "LUATupvalues.h"

#include <TST/TSThelpers.h>
#include <TST/TSTtestCollection.h>
#include <TST/TSTtestFunction.h>

#include <LUACX/LUAXupvalues.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;


static void LUAToneScope(){
   COLlist<COLstring> Upvalues;
   COLstring LuaCode =
      "local Foo = 'foo'\n"
      "-- position is here";
   LUAXgetUpvalues(LuaCode, LuaCode.find("-- position is here"), &Upvalues);

   TST_ASSERT_EQUALS(0, Upvalues.size());
}

static void LUATbasicUpvalues(){
   COLlist<COLstring> Upvalues;
   COLstring LuaCode = 
      "local Foo = 'foo'\n"
      "function myFunc()\n"
      "   -- position is here\n"
      "end";
   LUAXgetUpvalues(LuaCode, LuaCode.find("-- position is here"), &Upvalues);

   TST_ASSERT_EQUALS(1, Upvalues.size());
   TST_ASSERT_EQUALS("Foo", Upvalues[Upvalues.first()]);
}

static void LUATtestFunctionUpvalue(){
   COLlist<COLstring> Upvalues;
   COLstring LuaCode = 
      "local function privateFunc(Arg) end\n"
      "function myFunc()\n"
      "   -- position is here\n"
      "end";
   LUAXgetUpvalues(LuaCode, LuaCode.find("-- position is here"), &Upvalues);

   TST_ASSERT_EQUALS(1, Upvalues.size());
   TST_ASSERT_EQUALS("privateFunc", Upvalues[Upvalues.first()]);
}

static void LUATnoUpvalues(){
   COLlist<COLstring> Upvalues;
   COLstring LuaCode = 
      "Foo = 'foo'\n"
      "function myFunc()\n"
      "   -- position is here\n"
      "end";
   LUAXgetUpvalues(LuaCode, LuaCode.find("-- position is here"), &Upvalues);

   TST_ASSERT_EQUALS(0, Upvalues.size());
}

static void LUATupvaluesAndLocals(){
   COLlist<COLstring> Upvalues;
   COLstring LuaCode = 
      "local Foo\n"
      "function myFunc()\n"
      "   local Foo2\n"
      "   -- position is here\n"
      "   local Bar2 = 2\n"
      "end\n"
      "local Bar = 'bar'";
   LUAXgetUpvalues(LuaCode, LuaCode.find("-- position is here"), &Upvalues);

   TST_ASSERT_EQUALS(2, Upvalues.size());
   COLlistPlace Place = Upvalues.first();
   TST_ASSERT_EQUALS("Foo", Upvalues[Place]);
   Place = Upvalues.next(Place);
   TST_ASSERT_EQUALS("Bar", Upvalues[Place]);
}

static void LUATnestedScopes(){
   COLlist<COLstring> Upvalues;
   COLstring LuaCode = 
      "local Foo = 'foo'\n"
      "function myFunc()\n"
      "   local function innerFunc()\n"
      "      -- position is here\n"
      "   end\n"
      "   local Bar = 'bar'\n"
      "end";
   LUAXgetUpvalues(LuaCode, LuaCode.find("-- position is here"), &Upvalues);

   TST_ASSERT_EQUALS(3, Upvalues.size());
   COLlistPlace Place = Upvalues.first();
   TST_ASSERT_EQUALS("Foo", Upvalues[Place]);
   Place = Upvalues.next(Place);
   TST_ASSERT_EQUALS("innerFunc", Upvalues[Place]);
   Place = Upvalues.next(Place);
   TST_ASSERT_EQUALS("Bar", Upvalues[Place]);
}

static void LUATignoredScopes(){
   COLlist<COLstring> Upvalues;
   COLstring LuaCode = 
      "local Foo = 'foo'\n"
      "function myFunc()\n"
      "   local function innerFunc()\n"
      "      local FirstIgnoredLocal = nil\n"
      "      return 'Hello world'\n"
      "   end\n"
      "   local Bar = 'bar'\n"
      "   -- position is here\n"
      "   local function secondInnerFunc()\n"
      "      local SecondIgnoredLocal = nil\n"
      "      return 'Ignore this function'\n"
      "   end\n"
      "end";
   LUAXgetUpvalues(LuaCode, LuaCode.find("-- position is here"), &Upvalues);

   TST_ASSERT_EQUALS(1, Upvalues.size());
   TST_ASSERT_EQUALS("Foo", Upvalues[Upvalues.first()]);
}

static void LUATmultipleDeclaration(){
   COLlist<COLstring> Upvalues;
   COLstring LuaCode = 
      "local Foo, Bar = 'foo'\n"
      "local Hello,There,World\n"
      "function myFunc()\n"
      "   -- position is here\n"
      "end";
   LUAXgetUpvalues(LuaCode, LuaCode.find("-- position is here"), &Upvalues);

   TST_ASSERT_EQUALS(5, Upvalues.size());
   COLlistPlace Place = Upvalues.first();
   TST_ASSERT_EQUALS("Foo", Upvalues[Place]);
   Place = Upvalues.next(Place);
   TST_ASSERT_EQUALS("Bar", Upvalues[Place]);
   Place = Upvalues.next(Place);
   TST_ASSERT_EQUALS("Hello", Upvalues[Place]);
   Place = Upvalues.next(Place);
   TST_ASSERT_EQUALS("There", Upvalues[Place]);
   Place = Upvalues.next(Place);
   TST_ASSERT_EQUALS("World", Upvalues[Place]);
}

TSTtestable* LUATupvalues(){
   TST_START_COLLECTION("LUATupvalues")
      TST_ADD_TEST_FUNCTION(LUAToneScope);
      TST_ADD_TEST_FUNCTION(LUATbasicUpvalues);
      TST_ADD_TEST_FUNCTION(LUATtestFunctionUpvalue);
      TST_ADD_TEST_FUNCTION(LUATnoUpvalues);
      TST_ADD_TEST_FUNCTION(LUATupvaluesAndLocals);
      TST_ADD_TEST_FUNCTION(LUATnestedScopes);
      TST_ADD_TEST_FUNCTION(LUATignoredScopes);
      TST_ADD_TEST_FUNCTION(LUATmultipleDeclaration);
   TST_END_FIXTURE
};

