//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LUATreturn
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Tuesday, May 17th, 2011 @ 03:42:14 PM
//
//---------------------------------------------------------------------------
#include "LUATprecomp.h"
#pragma hdrstop

#include "LUATreturn.h"

#include <TST/TSThelpers.h>
#include <TST/TSTtestCollection.h>
#include <TST/TSTtestFunction.h>

#include <LUACX/LUAXreturn.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;


static void LUATtestBasicChunkNoReturn(){
   TST_ASSERT_EQUALS(false, LUAXdoesChunkReturn("A = 'foo'\nB = 'bar'"));
}

static void LUATtestBasicChunkReturnNoValue(){
   TST_ASSERT_EQUALS(true, LUAXdoesChunkReturn("A = 'foo'\nB = 'bar'\nreturn"));
}

static void LUATtestBasicChunkReturnOneValue(){
   TST_ASSERT_EQUALS(true, LUAXdoesChunkReturn("A = 'foo'\nB = 'bar'\nreturn A..B"));
}

static void LUATtestBasicChunkReturnMultiValue(){
   TST_ASSERT_EQUALS(true, LUAXdoesChunkReturn("A = 'foo'\nB = 'bar'\nreturn A,B"));
   TST_ASSERT_EQUALS(true, LUAXdoesChunkReturn("A = 'foo'\nB = 'bar'\nreturn A , B ,C"));
}

static void LUATtestFunctionReturnChunkNoReturn(){
   TST_ASSERT_EQUALS(false, LUAXdoesChunkReturn("function concat(A,B) return A..B end"));
}

static void LUATtestFunctionNoReturnChunkReturn(){
   TST_ASSERT_EQUALS(true, LUAXdoesChunkReturn("function concat(A,B) print(A,B) end\n return concat"));
}

static void LUATtestReturnFunctionNoReturn(){
   TST_ASSERT_EQUALS(true, LUAXdoesChunkReturn("return function(A,B) print(A,B) end"));
}

static void LUATtestReturnMultiFunction(){
   TST_ASSERT_EQUALS(true, LUAXdoesChunkReturn("return function(A) print(A) end, function(A,B) print(A,B) end"));
}

static void LUATtestDefineMultiFunction(){
   TST_ASSERT_EQUALS(false, LUAXdoesChunkReturn("function(A) print(A) end\nfunction(A,B) print(A,B) end"));
}

static void LUATtestMayReturn(){
   // The following chunk may return, but it is still valid to put another "return" statement
   // afterwards, which is what we care about.
   TST_ASSERT_EQUALS(false, LUAXdoesChunkReturn("if A then return else return end"));
}

static const char* LUAT_ALL_SCOPES_NO_RETURN = "\
while true do\n\
   for v = e1, e2, e3 do\n\
      function foo() return 'foo' end\n\
      repeat\n\
         if true then return else return end\n\
      until false\n\
      return foo()\n\
   end\n\
   return\n\
end";

static const char* LUAT_ALL_SCOPES_RETURN = "\
return function()\n\
   while true do\n\
      for v = e1, e2, e3 do\n\
         function foo() return 'foo' end\n\
         repeat\n\
            if true then return else return end\n\
         until false\n\
         return foo()\n\
      end\n\
      return\n\
   end\n\
end";

static void LUATtestAllScopes(){
   TST_ASSERT_EQUALS(false, LUAXdoesChunkReturn(LUAT_ALL_SCOPES_NO_RETURN));
   TST_ASSERT_EQUALS(true, LUAXdoesChunkReturn(LUAT_ALL_SCOPES_RETURN));
}

static const char* LUAT_ELSE_IF = "\
RunFunctionsInTable = function(T, NamePrefix)\n\
   return true\n\
   for K, V in _G.pairs(T) do\n\
      if _G.type(K) == 'string' then\n\
         if _G.type(V) == 'function' then\n\
            RunFunction(K, V, NamePrefix)\n\
         elseif _G.type(V) == 'table' then\n\
            RunFunctionsInTable(V, NamePrefix..'.'..K)\n\
         end\n\
      end\n\
   end\n\
end";

static void LUATtestElseIf(){
   TST_ASSERT_EQUALS(false, LUAXdoesChunkReturn(LUAT_ALL_SCOPES_NO_RETURN));
}

TSTtestable* LUATdoesChunkReturn(){
   TST_START_COLLECTION("LUATdoesChunkReturn")
      TST_ADD_TEST_FUNCTION(LUATtestBasicChunkNoReturn);
      TST_ADD_TEST_FUNCTION(LUATtestBasicChunkReturnNoValue);
      TST_ADD_TEST_FUNCTION(LUATtestBasicChunkReturnOneValue);
      TST_ADD_TEST_FUNCTION(LUATtestBasicChunkReturnMultiValue);
      TST_ADD_TEST_FUNCTION(LUATtestFunctionReturnChunkNoReturn);
      TST_ADD_TEST_FUNCTION(LUATtestFunctionNoReturnChunkReturn);
      TST_ADD_TEST_FUNCTION(LUATtestReturnFunctionNoReturn);
      TST_ADD_TEST_FUNCTION(LUATtestReturnMultiFunction);
      TST_ADD_TEST_FUNCTION(LUATtestDefineMultiFunction);
      TST_ADD_TEST_FUNCTION(LUATtestMayReturn);
      TST_ADD_TEST_FUNCTION(LUATtestAllScopes);
      TST_ADD_TEST_FUNCTION(LUATtestElseIf);
   TST_END_FIXTURE
};
