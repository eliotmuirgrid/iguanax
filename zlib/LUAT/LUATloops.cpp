//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LUATloops
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Wednesday, October 12th, 2011 @ 03:57:36 PM
//
//---------------------------------------------------------------------------
#include "LUATprecomp.h"
#pragma hdrstop

#include "LUATloops.h"

#include <TST/TSThelpers.h>
#include <TST/TSTtestCollection.h>
#include <TST/TSTtestFunction.h>

#include <LUACX/LUAXloops.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

// LUAXfindLoops() shouldn't really be called in this case, but let's make sure
// it doesn't crash.
static void LUATnoFunction(){
   LUAXscope OutScope;
   COLstring LuaCode = 
      "local A = 1\n"
      "local B = 2\n"
      "local C = 3";

   LUAXfindLoops(LuaCode, 0, LuaCode.size(), OutScope);
   TST_ASSERT_EQUALS(-1, OutScope.m_Start);
   TST_ASSERT_EQUALS(-1, OutScope.m_End);
   TST_ASSERT_EQUALS(-1, OutScope.m_StartTokenPosition);
   TST_ASSERT_EQUALS(-1, OutScope.m_EndTokenPosition);
   TST_ASSERT_EQUALS(0,  OutScope.m_Scopes.size());
}

static void LUATsimpleFunction(){
   LUAXscope OutScope;
   COLstring LuaCode = 
      "function myFunc()\n"
      "   -- insert function body\n"
      "end";

   LUAXfindLoops(LuaCode, 0, LuaCode.size(), OutScope);
   TST_ASSERT_EQUALS(1, OutScope.m_Start);
   TST_ASSERT_EQUALS(3, OutScope.m_End);
   TST_ASSERT_EQUALS(8, OutScope.m_StartTokenPosition);
   TST_ASSERT_EQUALS(LuaCode.size()-3, OutScope.m_EndTokenPosition);
   TST_ASSERT_EQUALS(LUAXscope::FUNCTION, OutScope.m_Type);
   TST_ASSERT_EQUALS(0, OutScope.m_Scopes.size());
}

static void LUATfirstFunction(){
   LUAXscope OutScope;
   COLstring LuaCode = 
      "function myFunc()\n"
      "   -- insert function body\n"
      "end\n"
      "local function mySecondFunc()\n"
      "   -- insert function body\n"
      "end";

   LUAXfindLoops(LuaCode, 0, LuaCode.find("local"), OutScope);
   TST_ASSERT_EQUALS(1, OutScope.m_Start);
   TST_ASSERT_EQUALS(3, OutScope.m_End);
   TST_ASSERT_EQUALS(LUAXscope::FUNCTION, OutScope.m_Type);
   TST_ASSERT_EQUALS(0, OutScope.m_Scopes.size());
}

static void LUATlastFunction(){
   LUAXscope OutScope;
   COLstring LuaCode = 
      "function myFunc()\n"
      "   -- insert function body\n"
      "end\n"
      "local function mySecondFunc()\n"
      "   -- insert function body\n"
      "end";

   LUAXfindLoops(LuaCode, LuaCode.find("local"), LuaCode.size(), OutScope);
   TST_ASSERT_EQUALS(4, OutScope.m_Start);
   TST_ASSERT_EQUALS(6, OutScope.m_End);
   TST_ASSERT_EQUALS(LUAXscope::FUNCTION, OutScope.m_Type);
   TST_ASSERT_EQUALS(0, OutScope.m_Scopes.size());
}

static void LUATsingleLineFunction(){
   LUAXscope OutScope;
   COLstring LuaCode = 
      "function myFunc() end";

   LUAXfindLoops(LuaCode, 0, LuaCode.size(), OutScope);
   TST_ASSERT_EQUALS(1, OutScope.m_Start);
   TST_ASSERT_EQUALS(1, OutScope.m_End);
   TST_ASSERT_EQUALS(LUAXscope::FUNCTION, OutScope.m_Type);
   TST_ASSERT_EQUALS(0, OutScope.m_Scopes.size());
}

static void LUATinnerFunction(){
   LUAXscope OutScope;
   COLstring LuaCode = 
      "function myFunc()\n"
      "   local function innerFunc()\n"
      "      -- insert function body\n"
      "   end\n"
      "end";

   LUAXfindLoops(LuaCode, LuaCode.find("local"), LuaCode.find("end")+4, OutScope);
   TST_ASSERT_EQUALS(2, OutScope.m_Start);
   TST_ASSERT_EQUALS(4, OutScope.m_End);
   TST_ASSERT_EQUALS(LUAXscope::FUNCTION, OutScope.m_Type);
   TST_ASSERT_EQUALS(0, OutScope.m_Scopes.size());
}

static void LUATignoreInnerFunction(){
   LUAXscope OutScope;
   COLstring LuaCode = 
      "function myFunc()\n"
      "   local function innerFunc()\n"
      "      -- insert function body\n"
      "   end\n"
      "end";

   LUAXfindLoops(LuaCode, 0, LuaCode.size(), OutScope);
   TST_ASSERT_EQUALS(1, OutScope.m_Start);
   TST_ASSERT_EQUALS(5, OutScope.m_End);
   TST_ASSERT_EQUALS(LUAXscope::FUNCTION, OutScope.m_Type);
   TST_ASSERT_EQUALS(0, OutScope.m_Scopes.size());
}

static void LUATignoreIfBlock(){
   LUAXscope OutScope;
   COLstring LuaCode = 
      "function myFunc()\n"
      "   if true then\n"
      "      -- insert function body\n"
      "   end\n"
      "end";

   LUAXfindLoops(LuaCode, 0, LuaCode.size(), OutScope);
   TST_ASSERT_EQUALS(1, OutScope.m_Start);
   TST_ASSERT_EQUALS(5, OutScope.m_End);
   TST_ASSERT_EQUALS(LUAXscope::FUNCTION, OutScope.m_Type);
   TST_ASSERT_EQUALS(0, OutScope.m_Scopes.size());
}

static void LUATsingleWhileLoop(){
   LUAXscope OutScope;
   COLstring LuaCode = 
      "function myFunc()\n"
      "   while true do\n"
      "      -- insert loop body\n"
      "   end\n"
      "end";

   LUAXfindLoops(LuaCode, 0, LuaCode.size(), OutScope);
   TST_ASSERT_EQUALS(1, OutScope.m_Start);
   TST_ASSERT_EQUALS(5, OutScope.m_End);
   TST_ASSERT_EQUALS(8, OutScope.m_StartTokenPosition);
   TST_ASSERT_EQUALS(LuaCode.size()-3, OutScope.m_EndTokenPosition);
   TST_ASSERT_EQUALS(LUAXscope::FUNCTION, OutScope.m_Type);
   TST_ASSERT_EQUALS(1, OutScope.m_Scopes.size());
   TST_ASSERT_EQUALS(2, OutScope.m_Scopes[0].m_Start);
   TST_ASSERT_EQUALS(4, OutScope.m_Scopes[0].m_End);
   TST_ASSERT_EQUALS(34, OutScope.m_Scopes[0].m_StartTokenPosition);
   TST_ASSERT_EQUALS(64, OutScope.m_Scopes[0].m_EndTokenPosition);
   TST_ASSERT_EQUALS(LUAXscope::LOOP, OutScope.m_Scopes[0].m_Type);
   TST_ASSERT_EQUALS(0, OutScope.m_Scopes[0].m_Scopes.size());
}

static void LUATsingleForLoop(){
   LUAXscope OutScope;
   COLstring LuaCode = 
      "function myFunc()\n"
      "   \n"
      "   for i=1,4 do\n"
      "      -- insert loop body\n"
      "   end\n"
      "end";

   LUAXfindLoops(LuaCode, 0, LuaCode.size(), OutScope);
   TST_ASSERT_EQUALS(1, OutScope.m_Start);
   TST_ASSERT_EQUALS(6, OutScope.m_End);
   TST_ASSERT_EQUALS(LUAXscope::FUNCTION, OutScope.m_Type);
   TST_ASSERT_EQUALS(1, OutScope.m_Scopes.size());
   TST_ASSERT_EQUALS(3, OutScope.m_Scopes[0].m_Start);
   TST_ASSERT_EQUALS(5, OutScope.m_Scopes[0].m_End);
   TST_ASSERT_EQUALS(LUAXscope::LOOP, OutScope.m_Scopes[0].m_Type);
   TST_ASSERT_EQUALS(0, OutScope.m_Scopes[0].m_Scopes.size());
}

static void LUATsingleRepeatLoop(){
   LUAXscope OutScope;
   COLstring LuaCode = 
      "function myFunc()\n"
      "   repeat\n"
      "      -- insert loop body\n"
      "   until false\n"
      "end";

   LUAXfindLoops(LuaCode, 0, LuaCode.size(), OutScope);
   TST_ASSERT_EQUALS(1, OutScope.m_Start);
   TST_ASSERT_EQUALS(5, OutScope.m_End);
   TST_ASSERT_EQUALS(LUAXscope::FUNCTION, OutScope.m_Type);
   TST_ASSERT_EQUALS(1, OutScope.m_Scopes.size());
   TST_ASSERT_EQUALS(2, OutScope.m_Scopes[0].m_Start);
   TST_ASSERT_EQUALS(4, OutScope.m_Scopes[0].m_End);
   TST_ASSERT_EQUALS(LUAXscope::LOOP, OutScope.m_Scopes[0].m_Type);
   TST_ASSERT_EQUALS(0, OutScope.m_Scopes[0].m_Scopes.size());
}

static void LUATmultiLoops(){
   LUAXscope OutScope;
   COLstring LuaCode = 
      "function myFunc()\n"
      "   repeat\n"
      "      -- insert loop body\n"
      "   until false\n"
      "   for i=1,4 do\n"
      "      while true do\n"
      "         -- insert loop body\n"
      "      end\n"
      "   end\n"
      "end";

   LUAXfindLoops(LuaCode, 0, LuaCode.size(), OutScope);
   TST_ASSERT_EQUALS(1, OutScope.m_Start);
   TST_ASSERT_EQUALS(10, OutScope.m_End);
   TST_ASSERT_EQUALS(LUAXscope::FUNCTION, OutScope.m_Type);
   TST_ASSERT_EQUALS(2, OutScope.m_Scopes.size());
   TST_ASSERT_EQUALS(2, OutScope.m_Scopes[0].m_Start);
   TST_ASSERT_EQUALS(4, OutScope.m_Scopes[0].m_End);
   TST_ASSERT_EQUALS(LUAXscope::LOOP, OutScope.m_Scopes[0].m_Type);
   TST_ASSERT_EQUALS(0, OutScope.m_Scopes[0].m_Scopes.size());
   TST_ASSERT_EQUALS(5, OutScope.m_Scopes[1].m_Start);
   TST_ASSERT_EQUALS(9, OutScope.m_Scopes[1].m_End);
   TST_ASSERT_EQUALS(LUAXscope::LOOP, OutScope.m_Scopes[1].m_Type);
   TST_ASSERT_EQUALS(1, OutScope.m_Scopes[1].m_Scopes.size());
   TST_ASSERT_EQUALS(6, OutScope.m_Scopes[1].m_Scopes[0].m_Start);
   TST_ASSERT_EQUALS(8, OutScope.m_Scopes[1].m_Scopes[0].m_End);
   TST_ASSERT_EQUALS(LUAXscope::LOOP, OutScope.m_Scopes[1].m_Scopes[0].m_Type);
   TST_ASSERT_EQUALS(0, OutScope.m_Scopes[1].m_Scopes[0].m_Scopes.size());
}

static void LUATsingleLineLoops(){
   LUAXscope OutScope;
   COLstring LuaCode = 
      "function myFunc()\n"
      "   repeat until false\n"
      "   for i=1,4 do\n"
      "      while true do end\n"
      "   end\n"
      "end";

   LUAXfindLoops(LuaCode, 0, LuaCode.size(), OutScope);
   TST_ASSERT_EQUALS(1, OutScope.m_Start);
   TST_ASSERT_EQUALS(6, OutScope.m_End);
   TST_ASSERT_EQUALS(LUAXscope::FUNCTION, OutScope.m_Type);
   TST_ASSERT_EQUALS(2, OutScope.m_Scopes.size());
   TST_ASSERT_EQUALS(2, OutScope.m_Scopes[0].m_Start);
   TST_ASSERT_EQUALS(2, OutScope.m_Scopes[0].m_End);
   TST_ASSERT_EQUALS(LUAXscope::LOOP, OutScope.m_Scopes[0].m_Type);
   TST_ASSERT_EQUALS(0, OutScope.m_Scopes[0].m_Scopes.size());
   TST_ASSERT_EQUALS(3, OutScope.m_Scopes[1].m_Start);
   TST_ASSERT_EQUALS(5, OutScope.m_Scopes[1].m_End);
   TST_ASSERT_EQUALS(LUAXscope::LOOP, OutScope.m_Scopes[1].m_Type);
   TST_ASSERT_EQUALS(1, OutScope.m_Scopes[1].m_Scopes.size());
   TST_ASSERT_EQUALS(4, OutScope.m_Scopes[1].m_Scopes[0].m_Start);
   TST_ASSERT_EQUALS(4, OutScope.m_Scopes[1].m_Scopes[0].m_End);
   TST_ASSERT_EQUALS(LUAXscope::LOOP, OutScope.m_Scopes[1].m_Scopes[0].m_Type);
   TST_ASSERT_EQUALS(0, OutScope.m_Scopes[1].m_Scopes[0].m_Scopes.size());
}

static void LUATsingleLineLoopsOnSameLine(){
   LUAXscope OutScope;
   COLstring LuaCode = 
      "function myFunc()\n"
      "   for i=1,4 do while true do end end\n" // yuck... who would do this?
      "end";

   LUAXfindLoops(LuaCode, 0, LuaCode.size(), OutScope);
   TST_ASSERT_EQUALS(1, OutScope.m_Start);
   TST_ASSERT_EQUALS(3, OutScope.m_End);
   TST_ASSERT_EQUALS(LUAXscope::FUNCTION, OutScope.m_Type);
   TST_ASSERT_EQUALS(1, OutScope.m_Scopes.size());
   TST_ASSERT_EQUALS(2, OutScope.m_Scopes[0].m_Start);
   TST_ASSERT_EQUALS(2, OutScope.m_Scopes[0].m_End);
   TST_ASSERT_EQUALS(LUAXscope::LOOP, OutScope.m_Scopes[0].m_Type);
   TST_ASSERT_EQUALS(1, OutScope.m_Scopes[0].m_Scopes.size());
   TST_ASSERT_EQUALS(2, OutScope.m_Scopes[0].m_Scopes[0].m_Start);
   TST_ASSERT_EQUALS(2, OutScope.m_Scopes[0].m_Scopes[0].m_End);
   TST_ASSERT_EQUALS(LUAXscope::LOOP, OutScope.m_Scopes[0].m_Scopes[0].m_Type);
   TST_ASSERT_EQUALS(0, OutScope.m_Scopes[0].m_Scopes[0].m_Scopes.size());
}

static void LUATdoBlock(){
   LUAXscope OutScope;
   COLstring LuaCode = 
      "function myFunc()\n"
      "   do\n"
      "      -- insert loop body\n"
      "   end\n"
      "end";

   LUAXfindLoops(LuaCode, 0, LuaCode.size(), OutScope);
   TST_ASSERT_EQUALS(1, OutScope.m_Start);
   TST_ASSERT_EQUALS(5, OutScope.m_End);
   TST_ASSERT_EQUALS(LUAXscope::FUNCTION, OutScope.m_Type);
   TST_ASSERT_EQUALS(1, OutScope.m_Scopes.size());
   TST_ASSERT_EQUALS(2, OutScope.m_Scopes[0].m_Start);
   TST_ASSERT_EQUALS(4, OutScope.m_Scopes[0].m_End);
   TST_ASSERT_EQUALS(LUAXscope::DO_BLOCK, OutScope.m_Scopes[0].m_Type);
   TST_ASSERT_EQUALS(0, OutScope.m_Scopes[0].m_Scopes.size());
}

static void LUATbrokenUpLoop(){
   LUAXscope OutScope;
   COLstring LuaCode = 
      "function myFunc()\n"
      "   while\n"
      "      true\n"
      "   do\n"
      "      -- insert loop body\n"
      "   end\n"
      "end";

   LUAXfindLoops(LuaCode, 0, LuaCode.size(), OutScope);
   TST_ASSERT_EQUALS(1, OutScope.m_Start);
   TST_ASSERT_EQUALS(7, OutScope.m_End);
   TST_ASSERT_EQUALS(8, OutScope.m_StartTokenPosition);
   TST_ASSERT_EQUALS(LuaCode.size()-3, OutScope.m_EndTokenPosition);
   TST_ASSERT_EQUALS(LUAXscope::FUNCTION, OutScope.m_Type);
   TST_ASSERT_EQUALS(1, OutScope.m_Scopes.size());
   TST_ASSERT_EQUALS(2, OutScope.m_Scopes[0].m_Start);
   TST_ASSERT_EQUALS(6, OutScope.m_Scopes[0].m_End);
   TST_ASSERT_EQUALS(43, OutScope.m_Scopes[0].m_StartTokenPosition);
   TST_ASSERT_EQUALS(73, OutScope.m_Scopes[0].m_EndTokenPosition);
   TST_ASSERT_EQUALS(LUAXscope::LOOP, OutScope.m_Scopes[0].m_Type);
   TST_ASSERT_EQUALS(0, OutScope.m_Scopes[0].m_Scopes.size());
}

TSTtestable* LUATloops(){
   TST_START_COLLECTION("LUATloops")
      TST_ADD_TEST_FUNCTION(LUATnoFunction);
      TST_ADD_TEST_FUNCTION(LUATsimpleFunction);
      TST_ADD_TEST_FUNCTION(LUATfirstFunction);
      TST_ADD_TEST_FUNCTION(LUATlastFunction);
      TST_ADD_TEST_FUNCTION(LUATsingleLineFunction);
      TST_ADD_TEST_FUNCTION(LUATinnerFunction);
      TST_ADD_TEST_FUNCTION(LUATignoreInnerFunction);
      TST_ADD_TEST_FUNCTION(LUATignoreIfBlock);
      TST_ADD_TEST_FUNCTION(LUATsingleWhileLoop);
      TST_ADD_TEST_FUNCTION(LUATsingleForLoop);
      TST_ADD_TEST_FUNCTION(LUATsingleRepeatLoop);
      TST_ADD_TEST_FUNCTION(LUATmultiLoops);
      TST_ADD_TEST_FUNCTION(LUATsingleLineLoops);
      TST_ADD_TEST_FUNCTION(LUATsingleLineLoopsOnSameLine);
      TST_ADD_TEST_FUNCTION(LUATdoBlock);
      TST_ADD_TEST_FUNCTION(LUATbrokenUpLoop);
   TST_END_FIXTURE
};

