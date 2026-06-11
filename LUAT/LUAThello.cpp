//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LUAThello
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Friday, April 23rd, 2010 @ 12:00:25 PM
//---------------------------------------------------------------------------
#include "LUATprecomp.h"
#pragma hdrstop

#include "LUAThello.h"

#include <TST/TSThelpers.h>
#include <TST/TSTtestCollection.h>
#include <TST/TSTtestFunction.h>

#include <COL/COLvar.h>
#include <COL/COLlist.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <LUA/LUAheader.h>
#include <LUA/LUAvar.h>
#include <LUA/LUAutils.h>

static void LUATstackDump(lua_State* L) 
{
   int i;
   int top = lua_gettop(L);
   for (i = 1; i <= top; i++) 
   {  /* repeat for each level */
      int t = lua_type(L, i);
      switch (t) 
      {
      case LUA_TSTRING:  /* strings */
         printf("`%s'", lua_tostring(L, i));
         break;
      case LUA_TBOOLEAN:  /* booleans */
         printf(lua_toboolean(L, i) ? "true" : "false");
         break;
      case LUA_TNUMBER:  /* numbers */
         printf("%g", lua_tonumber(L, i));
         break;
      default:  /* other values */
         printf("%s", lua_typename(L, t));
         break;
      }
      printf("  ");  /* put a separator */
   }
   printf("\n");  /* end the listing */
}

static int LUATprint(lua_State *L)
{
   int n=lua_gettop(L);
   int i;
   for (i=1; i<=n; i++)
   {
      if (i>1) printf("\t");
      if (lua_isstring(L,i)) printf("%s",lua_tostring(L,i));
      else if (lua_isnil(L,i)) printf("%s","nil");
      else if (lua_isboolean(L,i)) printf("%s",lua_toboolean(L,i) ? "true" : "false");
      else printf("%s:%p",luaL_typename(L,i),lua_topointer(L,i));
   }
   printf("\n");
   return 0;
}

// TODO - how to register this function with Lua.
void LUATerror(lua_State *L, const char* fmt, ...)
{ 
   va_list argp;
   va_start(argp, fmt);
   vfprintf(stderr, fmt, argp);
   va_end(argp);
   lua_close(L);
   exit(EXIT_FAILURE);
}

lua_State* LUATgetInterpreter()
{
   lua_State* L = lua_open();
   //LUATstackDump(L);

   luaopen_base(L);
   luaopen_table(L);
   luaopen_string(L);
   luaopen_math(L);

   lua_register(L,"print",LUATprint);

   if (luaL_loadfile(L,"../LUAT/modules/hello.lua"))
   {
      int TopIndex = lua_gettop(L);
      COLcout << lua_tostring(L, TopIndex) << newline;
      lua_close(L);
      COL_ERROR_STREAM("Couldn't load hello.lua", COLerror::PreCondition);
   }
   if (lua_pcall(L, 0, 0, 0)) // we have to compile the module before we can call it
   {
      int TopIndex = lua_gettop(L);
      COLcout << lua_tostring(L, TopIndex) << newline;
      lua_close(L);
      COL_ERROR_STREAM("Couldn't execute hello.lua", COLerror::PreCondition);
   }
   return L;
   
}

void LUATpcall(lua_State* L, int CountOfArgument, int CountOfReturn, int Error)
{
   if (lua_pcall(L, CountOfArgument, CountOfReturn, Error))
   {
      int TopIndex = lua_gettop(L);
      COLcout << lua_tostring(L, TopIndex) << newline;      
   }
}

// TODO - could set lua_atpanic function which will be called if Lua runs out of memory.
void testSimple()
{
   lua_State* L = LUATgetInterpreter();

   //LUATstackDump(L);
   lua_getglobal(L, "HelloWorld");
   //LUATstackDump(L);
   lua_pushstring(L, "Bong!");
   //LUATstackDump(L);
   lua_pcall(L, 1, 0, 0);

   lua_close(L);
}

void testWithTable()
{
   lua_State* L = LUATgetInterpreter();

   //LUATstackDump(L);
   lua_getglobal(L, "OnError");
   //LUATstackDump(L);
   lua_newtable(L);
   int TableIndex = lua_gettop(L);
   lua_pushstring(L, "This is the body of the message.");
   lua_setfield(L, TableIndex, "Body");
   //LUATstackDump(L);
   lua_pcall(L, 1, 0, 0);
   lua_close(L);
}

static int LUATmyCallback(lua_State* L)
{
   int n = lua_gettop(L);
   COLcout << "Lua called C with " << lua_tostring(L, n) << newline;

   return 0; // everything is honky dorry.
}

void testWithFunction(){
   lua_State* L = LUATgetInterpreter();

   lua_getglobal(L, "Callback");
   lua_pushcfunction(L, &LUATmyCallback);
   
   LUATpcall(L, 1, 0, 0);
   lua_close(L);
}

static void testVarMap(){
   COL_FUNCTION(testVarMap);
   lua_State* L = LUATgetInterpreter();
   int Result = luaL_loadstring(L, "T = {life='fff', fred={user='fff', password='mysecret'}, array={1,false,3,nil,3.5}, live=true}");
   COLASSERT(Result ==0);
   lua_pcall(L, 0,0,0);

   lua_getglobal(L, "T");
   //LUATstackDump(L);
   COLvar Var;
   Var["bob"] = "should be cleared";
   Var["doug"] = 54321;
   Var.fromLua(L, -1);
   TST_ASSERT_EQUALS("{\"array\":[1,false,3,null,3.5],\"fred\":{\"password\":\"mysecret\",\"user\":\"fff\"},\"life\":\"fff\",\"live\":true}", Var.json());
   lua_close(L); L = 0;

   // push the newly parsed COLvar into a new Lua interpreter and re-read it.
   lua_State* L2 = LUATgetInterpreter();
   Var.toLua(L2);
   COLvar NewVar;
   NewVar.fromLua(L2, -1);
   TST_ASSERT_EQUALS(Var.json(), NewVar.json());
   TST_ASSERT_EQUALS("{\"array\":[1,false,3,null,3.5],\"fred\":{\"password\":\"mysecret\",\"user\":\"fff\"},\"life\":\"fff\",\"live\":true}", NewVar.json());
   lua_close(L2);
}

static void testVarArray(){
   COL_FUNCTION(testVarArray);
   lua_State* L = LUATgetInterpreter();
   int Result = luaL_loadstring(L, "T = {'Fred',123,nil,true,false,3.5,9}");
   COLASSERT(Result ==0);
   lua_pcall(L, 0,0,0);

   lua_getglobal(L, "T");
   //LUATstackDump(L);
   COLvar Var;
   Var[0] = "should be cleared";
   Var[1] = 54321;
   Var.fromLua(L, -1);
   TST_ASSERT_EQUALS("[\"Fred\",123,null,true,false,3.5,9]", Var.json());
   lua_close(L); L = 0;

   // push the newly parsed COLvar into a new Lua interpreter and re-read it.
   lua_State* L2 = LUATgetInterpreter();
   Var.toLua(L2);
   COLvar NewVar;
   NewVar.fromLua(L2, -1);
   TST_ASSERT_EQUALS(Var.json(), NewVar.json());
   TST_ASSERT_EQUALS("[\"Fred\",123,null,true,false,3.5,9]", NewVar.json());
   lua_close(L2);
}

static void testVarInt(){
   COL_FUNCTION(testVarInt);
   lua_State* L = LUATgetInterpreter();
   int Result = luaL_loadstring(L, "T = 123");
   COLASSERT(Result ==0);
   lua_pcall(L, 0,0,0);

   lua_getglobal(L, "T");
   //LUATstackDump(L);
   COLvar Var;
   Var.fromLua(L, -1);
   TST_ASSERT_EQUALS("123", Var.json());
   lua_close(L); L = 0;

   // push the newly parsed COLvar into a new Lua interpreter and re-read it.
   lua_State* L2 = LUATgetInterpreter();
   Var.toLua(L2);
   COLvar NewVar;
   NewVar.fromLua(L2, -1);
   TST_ASSERT_EQUALS(Var.json(), NewVar.json());
   TST_ASSERT_EQUALS("123",      NewVar.json());
   lua_close(L2);
}

static void testVarDouble(){
   COL_FUNCTION(testVarDouble);
   lua_State* L = LUATgetInterpreter();
   int Result = luaL_loadstring(L, "T = -987.654");
   COLASSERT(Result ==0);
   lua_pcall(L, 0,0,0);

   lua_getglobal(L, "T");
   //LUATstackDump(L);
   COLvar Var;
   Var.fromLua(L, -1);
   TST_ASSERT_EQUALS("-987.654", Var.json());
   lua_close(L); L = 0;

   // push the newly parsed COLvar into a new Lua interpreter and re-read it.
   lua_State* L2 = LUATgetInterpreter();
   Var.toLua(L2);
   COLvar NewVar;
   NewVar.fromLua(L2, -1);
   TST_ASSERT_EQUALS(Var.json(), NewVar.json());
   TST_ASSERT_EQUALS("-987.654", NewVar.json());
   lua_close(L2);
}

static void testVarString(){
   COL_FUNCTION(testVarString);
   lua_State* L = LUATgetInterpreter();
   int Result = luaL_loadstring(L, "T = 'Hello, Lua JSON'");
   COLASSERT(Result ==0);
   lua_pcall(L, 0,0,0);

   lua_getglobal(L, "T");
   //LUATstackDump(L);
   COLvar Var;
   Var.fromLua(L, -1);
   TST_ASSERT_EQUALS("\"Hello, Lua JSON\"", Var.json());
   lua_close(L); L = 0;

   // push the newly parsed COLvar into a new Lua interpreter and re-read it.
   lua_State* L2 = LUATgetInterpreter();
   Var.toLua(L2);
   COLvar NewVar;
   NewVar.fromLua(L2, -1);
   TST_ASSERT_EQUALS(Var.json(),            NewVar.json());
   TST_ASSERT_EQUALS("\"Hello, Lua JSON\"", NewVar.json());
   lua_close(L2);
}

static void testVarTrue(){
   COL_FUNCTION(testVarTrue);
   lua_State* L = LUATgetInterpreter();
   int Result = luaL_loadstring(L, "T = true");
   COLASSERT(Result ==0);
   lua_pcall(L, 0,0,0);

   lua_getglobal(L, "T");
   //LUATstackDump(L);
   COLvar Var;
   Var.fromLua(L, -1);
   TST_ASSERT_EQUALS("true", Var.json());
   lua_close(L); L = 0;

   // push the newly parsed COLvar into a new Lua interpreter and re-read it.
   lua_State* L2 = LUATgetInterpreter();
   Var.toLua(L2);
   COLvar NewVar;
   NewVar.fromLua(L2, -1);
   TST_ASSERT_EQUALS(Var.json(), NewVar.json());
   TST_ASSERT_EQUALS("true",     NewVar.json());
   lua_close(L2);
}

static void testVarFalse(){
   COL_FUNCTION(testVarFalse);
   lua_State* L = LUATgetInterpreter();
   int Result = luaL_loadstring(L, "T = false");
   COLASSERT(Result ==0);
   lua_pcall(L, 0,0,0);

   lua_getglobal(L, "T");
   //LUATstackDump(L);
   COLvar Var = 123.456;;
   Var.fromLua(L, -1);
   TST_ASSERT_EQUALS("false", Var.json());
   lua_close(L); L = 0;

   // push the newly parsed COLvar into a new Lua interpreter and re-read it.
   lua_State* L2 = LUATgetInterpreter();
   Var.toLua(L2);
   COLvar NewVar;
   NewVar.fromLua(L2, -1);
   TST_ASSERT_EQUALS(Var.json(), NewVar.json());
   TST_ASSERT_EQUALS("false",    NewVar.json());
   lua_close(L2);
}

static void testVarNull(){
   COL_FUNCTION(testVarInt);
   lua_State* L = LUATgetInterpreter();
   int Result = luaL_loadstring(L, "T = nil");
   COLASSERT(Result ==0);
   lua_pcall(L, 0,0,0);

   lua_getglobal(L, "T");
   //LUATstackDump(L);
   COLvar Var = 987654;
   Var.fromLua(L, -1);
   TST_ASSERT_EQUALS("null", Var.json());
   lua_close(L); L = 0;

   // push the newly parsed COLvar into a new Lua interpreter and re-read it.
   lua_State* L2 = LUATgetInterpreter();
   Var.toLua(L2);
   COLvar NewVar;
   NewVar.fromLua(L2, -1);
   TST_ASSERT_EQUALS(Var.json(), NewVar.json());
   TST_ASSERT_EQUALS("null",     NewVar.json());
   lua_close(L2);
}

static void testEmptyTable(){
   COL_FUNCTION(testVarInt);
   lua_State* L = LUATgetInterpreter();
   int Result = luaL_loadstring(L, "T = {}");
   COLASSERT(Result ==0);
   lua_pcall(L, 0,0,0);

   lua_getglobal(L, "T");
   //LUATstackDump(L);
   COLvar Var;
   Var.fromLua(L, -1);
   TST_ASSERT_EQUALS("{}", Var.json());
   lua_close(L); L = 0;

   // push the newly parsed COLvar into a new Lua interpreter and re-read it.
   lua_State* L2 = LUATgetInterpreter();
   Var.toLua(L2);
   COLvar NewVar;
   NewVar.fromLua(L2, -1);
   TST_ASSERT_EQUALS(Var.json(), NewVar.json());
   TST_ASSERT_EQUALS("{}",     NewVar.json());
   lua_close(L2);
}

static void testVarTableInvalid(){
   COL_FUNCTION(testVarTableInvalid);
   lua_State* L = LUATgetInterpreter();
   int Result = luaL_loadstring(L, "T={array={1, 2, subtable={}}}");
   COLASSERT(Result ==0);
   lua_pcall(L,  0,0,0);

   lua_getglobal(L, "T");
   COLvar Var;
   LUA::LUAconversionResult ConversionResult = LUAtoVar(L, -1, &Var, NULL);
   TST_ASSERT_EQUALS(LUA::ERROR_TABLE_MIXED_KEYS, ConversionResult);
   lua_close(L); L = 0;

   L = LUATgetInterpreter();
   Result = luaL_loadstring(L, "F = function() end;T = {};T[F] = 1");
   COLASSERT(Result ==0);
   lua_pcall(L,  0,0,0);

   lua_getglobal(L, "T");
   ConversionResult = LUAtoVar(L, -1, &Var, NULL);
   TST_ASSERT_EQUALS(LUA::ERROR_TABLE_INVALID_KEY, ConversionResult);
   lua_close(L); L = 0;
}

static void testVarArrayInvalid(){
   COL_FUNCTION(testVarArrayInvalid);

   lua_State* L = LUATgetInterpreter();
   int Result = luaL_loadstring(L, "T = {};T[-1] = 1");
   COLASSERT(Result ==0);
   lua_pcall(L,  0,0,0);

   lua_getglobal(L, "T");
   COLvar Var;
   LUA::LUAconversionResult ConversionResult = LUAtoVar(L, -1, &Var, NULL);
   TST_ASSERT_EQUALS(LUA::ERROR_ARRAY_INVALID_INDEX, ConversionResult);
   lua_close(L); L = 0;
}

static void testVarUserdataInvalid(){
   COL_FUNCTION(testVarUserdataInvalid);

   lua_State* L = LUATgetInterpreter();
   int Result = luaL_loadstring(L, "T = newproxy(true)");
   COLASSERT(Result ==0);
   lua_pcall(L,  0,0,0);

   lua_getglobal(L, "T");
   COLvar Var;
   LUA::LUAconversionResult ConversionResult = LUAtoVar(L, -1, &Var, NULL);
   TST_ASSERT_EQUALS(LUA::ERROR_UD_TO_STRING, ConversionResult);
   lua_close(L); L = 0;
}

static void testVarErrorTraversal(){
   COL_FUNCTION(testVarErrorTraversal);

   lua_State* L = LUATgetInterpreter();
   int Result = luaL_loadstring(L, "T={A={B={C={ {1, {2, 2, fail={}}} }}}}");
   COLASSERT(Result ==0);
   lua_pcall(L,  0,0,0);

   lua_getglobal(L, "T");
   COLvar Var;
   COLlist<COLstring> TraversalStack;
   COLstring ExpectedStack[] = {"A", "B", "C", "1", "2"};
   LUA::LUAconversionResult ConversionResult = LUAtoVar(L, -1, &Var, &TraversalStack);
   TST_ASSERT(ConversionResult != LUA::CONVERSION_SUCCESSFUL);

   COLlist<COLstring>::const_iterator it = TraversalStack.cbegin();
   for (int i = 0; i < 5; i++, it++) {
      TST_ASSERT_EQUALS(ExpectedStack[i], *it);
   }
   // operator== not defined for this iterator
   TST_ASSERT(!(it != TraversalStack.cend()));

   lua_close(L); L = 0;
}

TSTtestable* LUAThello(){
   TST_START_COLLECTION("LUAThello")
      TST_ADD_TEST_FUNCTION(testSimple);
      TST_ADD_TEST_FUNCTION(testWithTable);
      TST_ADD_TEST_FUNCTION(testWithFunction);
      TST_ADD_TEST_FUNCTION(testVarMap);
      TST_ADD_TEST_FUNCTION(testVarArray);
      TST_ADD_TEST_FUNCTION(testVarInt);
      TST_ADD_TEST_FUNCTION(testVarDouble);
      TST_ADD_TEST_FUNCTION(testVarString);
      TST_ADD_TEST_FUNCTION(testVarTrue);
      TST_ADD_TEST_FUNCTION(testVarFalse);
      TST_ADD_TEST_FUNCTION(testVarNull);
      TST_ADD_TEST_FUNCTION(testEmptyTable);
      TST_ADD_TEST_FUNCTION(testVarTableInvalid);
      TST_ADD_TEST_FUNCTION(testVarArrayInvalid);
      TST_ADD_TEST_FUNCTION(testVarUserdataInvalid);
      TST_ADD_TEST_FUNCTION(testVarErrorTraversal);
   TST_END_FIXTURE
}

