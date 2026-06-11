//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testCOLvar
//
// Description: test COLvar
//---------------------------------------------------------------------------
#include "COLexamplePrecomp.h"
#pragma hdrstop

#include <stdio.h>

#include <TST/TSThelpers.h>
#include <TST/TSTtestCollection.h>
#include <TST/TSTtestFunction.h>
#include <COL/COLvar.h>
#include <COL/COLbinaryParser.h>
#include <COL/COLlog.h>
#include <FIL/FILutils.h>
COL_LOG_MODULE;

static void testSimple()
{
   COLvar v1;
   TST_ASSERT_EQUALS(COLvar::None, v1.type());
   TST_ASSERT_EQUALS(true,  v1.isNone());
   TST_ASSERT_EQUALS(true,  v1.isScalar());
   TST_ASSERT_EQUALS(false, v1.isCollection());
   TST_ASSERT_EQUALS(true,  v1.isLeaf());
   TST_ASSERT_EQUALS(false, v1.isInteger());
   TST_ASSERT_EQUALS(false, v1.isDouble());
   TST_ASSERT_EQUALS(false, v1.isString());
   TST_ASSERT_EQUALS(false, v1.isNumeric());
   TST_ASSERT_EQUALS(false, v1.isArray());
   TST_ASSERT_EQUALS(false, v1.isMap());
   TST_ASSERT_EQUALS(0,     v1.size());
   TST_ASSERT_EQUALS(0,     v1.asInt());
   TST_ASSERT_EQUALS(0.0,   v1.asDouble());
   TST_ASSERT_EQUALS("",    v1.str());
   TST_ASSERT_EQUALS(COLstring(""), v1.asString());
   {
      COLvar v2(v1);
      TST_ASSERT_EQUALS(COLvar::None, v2.type());
      TST_ASSERT_EQUALS(true, v2.isNone());
      TST_ASSERT_EQUALS(0, v2.asInt());
      TST_ASSERT_EQUALS(0.0, v2.asDouble());
      TST_ASSERT_EQUALS(COLstring(""), v2.asString());
      TST_ASSERT_EQUALS(true,  v2.isScalar());
      TST_ASSERT_EQUALS(false, v2.isCollection());
      TST_ASSERT_EQUALS(true,  v2.isLeaf());
   }
   {
      COLvar v3;
      v3 = v1;
      TST_ASSERT_EQUALS(COLvar::None, v3.type());
      TST_ASSERT_EQUALS(true, v3.isNone());
      TST_ASSERT_EQUALS(0, v3.asInt());
      TST_ASSERT_EQUALS(0.0, v3.asDouble());
      TST_ASSERT_EQUALS(COLstring(""), v3.asString());
      TST_ASSERT_EQUALS(true,  v3.isScalar());
      TST_ASSERT_EQUALS(false, v3.isCollection());
      TST_ASSERT_EQUALS(true,  v3.isLeaf());
   }
   COLvar v4;
   TST_ASSERT_EQUALS("", v4.asString());
}

static void testBoolean()
{
   COLvar v(true);
   TST_ASSERT_EQUALS(COLvar::Boolean, v.type());
   TST_ASSERT_EQUALS(true,   v.isBoolean());
   TST_ASSERT_EQUALS(false,  v.isString());
   TST_ASSERT_EQUALS(false,  v.isArray());
   TST_ASSERT_EQUALS(false,  v.isMap());
   TST_ASSERT_EQUALS(false,  v.isInteger());
   TST_ASSERT_EQUALS(false,  v.isDouble());
   TST_ASSERT_EQUALS(true,   v.asBool());
   TST_ASSERT_EQUALS(1,      v.asInt());
   TST_ASSERT_EQUALS(1.0,    v.asDouble());
   TST_ASSERT_EQUALS("true", v.asString());
   TST_ASSERT_EQUALS("true", v.json());
   TST_ASSERT_EQUALS("",     v.str());

   v[5] = 27;
   TST_ASSERT_EQUALS(COLvar::Array, v.type());
   TST_ASSERT_EQUALS(false,  v.isBoolean());
   TST_ASSERT_EQUALS(false,  v.isString());
   TST_ASSERT_EQUALS(true,   v.isArray());
   TST_ASSERT_EQUALS(false,  v.isMap());
   TST_ASSERT_EQUALS(false,  v.isInteger());
   TST_ASSERT_EQUALS(false,  v.isDouble());
   TST_ASSERT_EQUALS(false,  v.asBool());
   TST_ASSERT_EQUALS(0,      v.asInt());
   TST_ASSERT_EQUALS(0,      v.asDouble());
   TST_ASSERT_EQUALS("",     v.asString());
   TST_ASSERT_EQUALS("[null,null,null,null,null,27]", v.json());

   v = false;
   TST_ASSERT_EQUALS(COLvar::Boolean, v.type());
   TST_ASSERT_EQUALS(true,   v.isBoolean());
   TST_ASSERT_EQUALS(false,  v.isString());
   TST_ASSERT_EQUALS(false,  v.isArray());
   TST_ASSERT_EQUALS(false,  v.isMap());
   TST_ASSERT_EQUALS(false,  v.isInteger());
   TST_ASSERT_EQUALS(false,  v.isDouble());
   TST_ASSERT_EQUALS(false,  v.asBool());
   TST_ASSERT_EQUALS(0,      v.asInt());
   TST_ASSERT_EQUALS(0.0,    v.asDouble());
   TST_ASSERT_EQUALS("false",v.asString());
   TST_ASSERT_EQUALS("false",v.json());
   TST_ASSERT_EQUALS("",     v.str());

   v = "false";
   TST_ASSERT_EQUALS(COLvar::String, v.type());
   TST_ASSERT_EQUALS(false,  v.isBoolean());
   TST_ASSERT_EQUALS(true,   v.isString());
   TST_ASSERT_EQUALS(false,  v.isArray());
   TST_ASSERT_EQUALS(false,  v.isMap());
   TST_ASSERT_EQUALS(false,  v.isInteger());
   TST_ASSERT_EQUALS(false,  v.isDouble());
   TST_ASSERT_EQUALS(false,  v.asBool());
   TST_ASSERT_EQUALS(0,      v.asInt());
   TST_ASSERT_EQUALS(0.0,    v.asDouble());
   TST_ASSERT_EQUALS("false",v.asString());
   TST_ASSERT_EQUALS("\"false\"",v.json());

   v = "true";
   TST_ASSERT_EQUALS(COLvar::String, v.type());
   TST_ASSERT_EQUALS(false,  v.isBoolean());
   TST_ASSERT_EQUALS(true,   v.isString());
   TST_ASSERT_EQUALS(false,  v.isArray());
   TST_ASSERT_EQUALS(false,  v.isMap());
   TST_ASSERT_EQUALS(false,  v.isInteger());
   TST_ASSERT_EQUALS(false,  v.isDouble());
   TST_ASSERT_EQUALS(true,   v.asBool());
   TST_ASSERT_EQUALS(0,      v.asInt());
   TST_ASSERT_EQUALS(0,      v.asDouble());
   TST_ASSERT_EQUALS("true", v.asString());
   TST_ASSERT_EQUALS("\"true\"",v.json());

   v = "junk";
   TST_ASSERT_EQUALS(COLvar::String, v.type());
   TST_ASSERT_EQUALS(false,  v.isBoolean());
   TST_ASSERT_EQUALS(true,   v.isString());
   TST_ASSERT_EQUALS(false,  v.isArray());
   TST_ASSERT_EQUALS(false,  v.isMap());
   TST_ASSERT_EQUALS(false,  v.isInteger());
   TST_ASSERT_EQUALS(false,  v.isDouble());
   TST_ASSERT_EQUALS(false,  v.asBool());
   TST_ASSERT_EQUALS(0,      v.asInt());
   TST_ASSERT_EQUALS(0.0,    v.asDouble());
   TST_ASSERT_EQUALS("junk", v.asString());
   TST_ASSERT_EQUALS("junk", v.str());
   TST_ASSERT_EQUALS("\"junk\"",v.json());

   v = "123";
   TST_ASSERT_EQUALS(COLvar::String, v.type());
   TST_ASSERT_EQUALS(false,  v.isBoolean());
   TST_ASSERT_EQUALS(true,   v.isString());
   TST_ASSERT_EQUALS(false,  v.isArray());
   TST_ASSERT_EQUALS(false,  v.isMap());
   TST_ASSERT_EQUALS(false,  v.isInteger());
   TST_ASSERT_EQUALS(false,  v.isDouble());
   TST_ASSERT_EQUALS(true,   v.asBool());
   TST_ASSERT_EQUALS(123,    v.asInt());
   TST_ASSERT_EQUALS(123.0,  v.asDouble());
   TST_ASSERT_EQUALS("123",  v.asString());
   TST_ASSERT_EQUALS("\"123\"", v.json());

   v = "0";
   TST_ASSERT_EQUALS(COLvar::String, v.type());
   TST_ASSERT_EQUALS(false,  v.isBoolean());
   TST_ASSERT_EQUALS(true,   v.isString());
   TST_ASSERT_EQUALS(false,  v.isArray());
   TST_ASSERT_EQUALS(false,  v.isMap());
   TST_ASSERT_EQUALS(false,  v.isInteger());
   TST_ASSERT_EQUALS(false,  v.isDouble());
   TST_ASSERT_EQUALS(false,  v.asBool());
   TST_ASSERT_EQUALS(0,      v.asInt());
   TST_ASSERT_EQUALS(0.0,    v.asDouble());
   TST_ASSERT_EQUALS("0",    v.asString());
   TST_ASSERT_EQUALS("\"0\"",    v.json());

   v = "";
   TST_ASSERT_EQUALS(COLvar::String, v.type());
   TST_ASSERT_EQUALS(false,  v.isBoolean());
   TST_ASSERT_EQUALS(true,   v.isString());
   TST_ASSERT_EQUALS(false,  v.isArray());
   TST_ASSERT_EQUALS(false,  v.isMap());
   TST_ASSERT_EQUALS(false,  v.isInteger());
   TST_ASSERT_EQUALS(false,  v.isDouble());
   TST_ASSERT_EQUALS(false,  v.asBool());
   TST_ASSERT_EQUALS(0,      v.asInt());
   TST_ASSERT_EQUALS(0.0,    v.asDouble());
   TST_ASSERT_EQUALS("",     v.asString());
   TST_ASSERT_EQUALS("\"\"", v.json());

   v = "1";
   TST_ASSERT_EQUALS(COLvar::String, v.type());
   TST_ASSERT_EQUALS(false,  v.isBoolean());
   TST_ASSERT_EQUALS(true,   v.isString());
   TST_ASSERT_EQUALS(false,  v.isArray());
   TST_ASSERT_EQUALS(false,  v.isMap());
   TST_ASSERT_EQUALS(false,  v.isInteger());
   TST_ASSERT_EQUALS(false,  v.isDouble());
   TST_ASSERT_EQUALS(true,   v.asBool());
   TST_ASSERT_EQUALS(1,      v.asInt());
   TST_ASSERT_EQUALS(1.0,    v.asDouble());
   TST_ASSERT_EQUALS("1",    v.asString());
   TST_ASSERT_EQUALS("\"1\"",v.json());

   v = COLvar(56.78);
   TST_ASSERT_EQUALS(COLvar::Double, v.type());
   TST_ASSERT_EQUALS(false,  v.isNull());
   TST_ASSERT_EQUALS(false,  v.isBoolean());
   TST_ASSERT_EQUALS(false,  v.isString());
   TST_ASSERT_EQUALS(false,  v.isArray());
   TST_ASSERT_EQUALS(true,   v.isDouble());
   TST_ASSERT_EQUALS(false,  v.isMap());
   TST_ASSERT_EQUALS(false,  v.isInteger());
   TST_ASSERT_EQUALS(true,   v.isDouble());
   TST_ASSERT_EQUALS(true,   v.asBool());
   TST_ASSERT_EQUALS(56,     v.asInt());
   TST_ASSERT_EQUALS(56.78,  v.asDouble());
   TST_ASSERT_EQUALS("56.78", v.asString());
   TST_ASSERT_EQUALS("56.78", v.json());

   v = 0.0;
   TST_ASSERT_EQUALS(COLvar::Double, v.type());
   TST_ASSERT_EQUALS(false,  v.isNull());
   TST_ASSERT_EQUALS(false,  v.isBoolean());
   TST_ASSERT_EQUALS(false,  v.isString());
   TST_ASSERT_EQUALS(false,  v.isArray());
   TST_ASSERT_EQUALS(true,   v.isDouble());
   TST_ASSERT_EQUALS(false,  v.isMap());
   TST_ASSERT_EQUALS(false,  v.isInteger());
   TST_ASSERT_EQUALS(true,   v.isDouble());
   TST_ASSERT_EQUALS(false,  v.asBool());
   TST_ASSERT_EQUALS(0,      v.asInt());
   TST_ASSERT_EQUALS(0.0,    v.asDouble());
   TST_ASSERT_EQUALS("0",    v.asString());
   TST_ASSERT_EQUALS("0",    v.json());

   v = COLvar();
   TST_ASSERT_EQUALS(COLvar::None, v.type());
   TST_ASSERT_EQUALS(true,   v.isNull());
   TST_ASSERT_EQUALS(false,  v.isBoolean());
   TST_ASSERT_EQUALS(false,  v.isString());
   TST_ASSERT_EQUALS(false,  v.isArray());
   TST_ASSERT_EQUALS(false,  v.isMap());
   TST_ASSERT_EQUALS(false,  v.isInteger());
   TST_ASSERT_EQUALS(false,  v.isDouble());
   TST_ASSERT_EQUALS(false,  v.asBool());
   TST_ASSERT_EQUALS(0,      v.asInt());
   TST_ASSERT_EQUALS(0.0,    v.asDouble());
   TST_ASSERT_EQUALS("",     v.asString());
   TST_ASSERT_EQUALS("null", v.json());
}

static void testIntCtor()
{
   COLvar v1(123);
   TST_ASSERT_EQUALS(COLvar::Integer, v1.type());
   TST_ASSERT_EQUALS(false, v1.isNone());
   TST_ASSERT_EQUALS(true, v1.isInteger());
   TST_ASSERT_EQUALS(false, v1.isDouble());
   TST_ASSERT_EQUALS(false, v1.isString());
   TST_ASSERT_EQUALS(true, v1.isNumeric());
   TST_ASSERT_EQUALS(false, v1.isArray());
   TST_ASSERT_EQUALS(false, v1.isMap());
   TST_ASSERT_EQUALS(123, v1.asInt());
   TST_ASSERT_EQUALS(123.0, v1.asDouble());
   TST_ASSERT_EQUALS(COLstring("123"), v1.asString());
   TST_ASSERT_EQUALS(true,  v1.isScalar());
   TST_ASSERT_EQUALS(false, v1.isCollection());
   TST_ASSERT_EQUALS(true,  v1.isLeaf());
   TST_ASSERT_EQUALS(0,     v1.size());
   TST_ASSERT_EQUALS("",    v1.str());
   {
      COLvar v2(v1);
      TST_ASSERT_EQUALS(COLvar::Integer, v2.type());
      TST_ASSERT_EQUALS(false, v2.isNone());
      TST_ASSERT_EQUALS(true, v2.isInteger());
      TST_ASSERT_EQUALS(false, v2.isDouble());
      TST_ASSERT_EQUALS(false, v2.isString());
      TST_ASSERT_EQUALS(true, v2.isNumeric());
      TST_ASSERT_EQUALS(false, v2.isArray());
      TST_ASSERT_EQUALS(false, v2.isMap());
      TST_ASSERT_EQUALS(123, v2.asInt());
      TST_ASSERT_EQUALS(123.0, v2.asDouble());
      TST_ASSERT_EQUALS(COLstring("123"), v2.asString());
      TST_ASSERT_EQUALS(true,  v2.isScalar());
      TST_ASSERT_EQUALS(false, v2.isCollection());
      TST_ASSERT_EQUALS(true,  v2.isLeaf());
      TST_ASSERT_EQUALS(0,     v2.size());
      TST_ASSERT_EQUALS("",    v2.str());
   }
   {
      COLvar v3;
      v3 = v1;
      TST_ASSERT_EQUALS(COLvar::Integer, v3.type());
      TST_ASSERT_EQUALS(false, v3.isNone());
      TST_ASSERT_EQUALS(true, v3.isInteger());
      TST_ASSERT_EQUALS(false, v3.isDouble());
      TST_ASSERT_EQUALS(false, v3.isString());
      TST_ASSERT_EQUALS(true, v3.isNumeric());
      TST_ASSERT_EQUALS(false, v3.isArray());
      TST_ASSERT_EQUALS(false, v3.isMap());
      TST_ASSERT_EQUALS(123, v3.asInt());
      TST_ASSERT_EQUALS(123.0, v3.asDouble());
      TST_ASSERT_EQUALS(COLstring("123"), v3.asString());
      TST_ASSERT_EQUALS(true,  v3.isScalar());
      TST_ASSERT_EQUALS(false, v3.isCollection());
      TST_ASSERT_EQUALS(true,  v3.isLeaf());

      // test converting an int into a string
      v3 = v3.asString();
      TST_ASSERT_EQUALS(COLvar::String, v3.type());
      TST_ASSERT_EQUALS(false, v3.isNone());
      TST_ASSERT_EQUALS(false, v3.isInteger());
      TST_ASSERT_EQUALS(false, v3.isDouble());
      TST_ASSERT_EQUALS(true, v3.isString());
      TST_ASSERT_EQUALS(false, v3.isNumeric());
      TST_ASSERT_EQUALS(false, v3.isArray());
      TST_ASSERT_EQUALS(false, v3.isMap());
      TST_ASSERT_EQUALS(123, v3.asInt());
      TST_ASSERT_EQUALS(123.0, v3.asDouble());
      TST_ASSERT_EQUALS(COLstring("123"), v3.asString());
      TST_ASSERT_EQUALS(true,  v3.isScalar());
      TST_ASSERT_EQUALS(false, v3.isCollection());
      TST_ASSERT_EQUALS(true,  v3.isLeaf());
   }
}

static void testLongLongCtor(){
   long long Arg = 12345678910;
   COLvar v1(Arg);
   TST_ASSERT_EQUALS(COLvar::Integer, v1.type());
   TST_ASSERT_EQUALS(false, v1.isNone());
   TST_ASSERT_EQUALS(true, v1.isInteger());
   TST_ASSERT_EQUALS(false, v1.isDouble());
   TST_ASSERT_EQUALS(false, v1.isString());
   TST_ASSERT_EQUALS(true, v1.isNumeric());
   TST_ASSERT_EQUALS(false, v1.isArray());
   TST_ASSERT_EQUALS(false, v1.isMap());
   TST_ASSERT_EQUALS(Arg, v1.asInt());
   TST_ASSERT_EQUALS(12345678910.0, v1.asDouble());
   TST_ASSERT_EQUALS(COLstring("12345678910"), v1.asString());
   TST_ASSERT_EQUALS(true,  v1.isScalar());
   TST_ASSERT_EQUALS(false, v1.isCollection());
   TST_ASSERT_EQUALS(true,  v1.isLeaf());
   TST_ASSERT_EQUALS(0,     v1.size());
   TST_ASSERT_EQUALS("",    v1.str());
   {
      COLvar v2(v1);
      TST_ASSERT_EQUALS(COLvar::Integer, v2.type());
      TST_ASSERT_EQUALS(false, v2.isNone());
      TST_ASSERT_EQUALS(true, v2.isInteger());
      TST_ASSERT_EQUALS(false, v2.isDouble());
      TST_ASSERT_EQUALS(false, v2.isString());
      TST_ASSERT_EQUALS(true, v2.isNumeric());
      TST_ASSERT_EQUALS(false, v2.isArray());
      TST_ASSERT_EQUALS(false, v2.isMap());
      TST_ASSERT_EQUALS(Arg, v2.asInt());
      TST_ASSERT_EQUALS(12345678910.0, v2.asDouble());
      TST_ASSERT_EQUALS(COLstring("12345678910"), v2.asString());
      TST_ASSERT_EQUALS(true,  v2.isScalar());
      TST_ASSERT_EQUALS(false, v2.isCollection());
      TST_ASSERT_EQUALS(true,  v2.isLeaf());
      TST_ASSERT_EQUALS(0,     v2.size());
      TST_ASSERT_EQUALS("",    v2.str());
   }
   {
      COLvar v3;
      v3 = v1;
      TST_ASSERT_EQUALS(COLvar::Integer, v3.type());
      TST_ASSERT_EQUALS(false, v3.isNone());
      TST_ASSERT_EQUALS(true, v3.isInteger());
      TST_ASSERT_EQUALS(false, v3.isDouble());
      TST_ASSERT_EQUALS(false, v3.isString());
      TST_ASSERT_EQUALS(true, v3.isNumeric());
      TST_ASSERT_EQUALS(false, v3.isArray());
      TST_ASSERT_EQUALS(false, v3.isMap());
      TST_ASSERT_EQUALS(Arg, v3.asInt());
      TST_ASSERT_EQUALS(12345678910.0, v3.asDouble());
      TST_ASSERT_EQUALS(COLstring("12345678910"), v3.asString());
      TST_ASSERT_EQUALS(true,  v3.isScalar());
      TST_ASSERT_EQUALS(false, v3.isCollection());
      TST_ASSERT_EQUALS(true,  v3.isLeaf());

      // test converting an int into a string
      v3 = v3.asString();
      TST_ASSERT_EQUALS(COLvar::String, v3.type());
      TST_ASSERT_EQUALS(false, v3.isNone());
      TST_ASSERT_EQUALS(false, v3.isInteger());
      TST_ASSERT_EQUALS(false, v3.isDouble());
      TST_ASSERT_EQUALS(true, v3.isString());
      TST_ASSERT_EQUALS(false, v3.isNumeric());
      TST_ASSERT_EQUALS(false, v3.isArray());
      TST_ASSERT_EQUALS(false, v3.isMap());
      TST_ASSERT_EQUALS(Arg, v3.asInt());
      TST_ASSERT_EQUALS(12345678910.0, v3.asDouble());
      TST_ASSERT_EQUALS(COLstring("12345678910"), v3.asString());
      TST_ASSERT_EQUALS(true,  v3.isScalar());
      TST_ASSERT_EQUALS(false, v3.isCollection());
      TST_ASSERT_EQUALS(true,  v3.isLeaf());
   }
}

static void testDoubleCtor()
{
   COLvar v1(-456.789);
   TST_ASSERT_EQUALS(COLvar::Double, v1.type());
   TST_ASSERT_EQUALS(false, v1.isNone());
   TST_ASSERT_EQUALS(false, v1.isInteger());
   TST_ASSERT_EQUALS(true, v1.isDouble());
   TST_ASSERT_EQUALS(false, v1.isString());
   TST_ASSERT_EQUALS(true, v1.isNumeric());
   TST_ASSERT_EQUALS(false, v1.isArray());
   TST_ASSERT_EQUALS(false, v1.isMap());
   TST_ASSERT_EQUALS(-456, v1.asInt());
   TST_ASSERT_EQUALS(-456.789, v1.asDouble());
   TST_ASSERT_EQUALS(COLstring("-456.789"), v1.asString());
   TST_ASSERT_EQUALS(true,  v1.isScalar());
   TST_ASSERT_EQUALS(false, v1.isCollection());
   TST_ASSERT_EQUALS(true,  v1.isLeaf());
   TST_ASSERT_EQUALS(0,     v1.size());
   TST_ASSERT_EQUALS("",    v1.str());
   {
      COLvar v2(v1);
      TST_ASSERT_EQUALS(COLvar::Double, v2.type());
      TST_ASSERT_EQUALS(false, v2.isNone());
      TST_ASSERT_EQUALS(false, v2.isInteger());
      TST_ASSERT_EQUALS(true,  v2.isDouble());
      TST_ASSERT_EQUALS(false, v2.isString());
      TST_ASSERT_EQUALS(true,  v2.isNumeric());
      TST_ASSERT_EQUALS(false, v2.isArray());
      TST_ASSERT_EQUALS(false, v2.isMap());
      TST_ASSERT_EQUALS(-456,  v2.asInt());
      TST_ASSERT_EQUALS(true,  v2.isScalar());
      TST_ASSERT_EQUALS(false, v2.isCollection());
      TST_ASSERT_EQUALS(true,  v2.isLeaf());
      TST_ASSERT_EQUALS(-456.789, v2.asDouble());
      TST_ASSERT_EQUALS(COLstring("-456.789"), v2.asString());
      TST_ASSERT_EQUALS("",    v2.str());
   }
   {
      COLvar v3;
      v3 = v1;
      TST_ASSERT_EQUALS(COLvar::Double, v3.type());
      TST_ASSERT_EQUALS(false, v3.isNone());
      TST_ASSERT_EQUALS(false, v3.isInteger());
      TST_ASSERT_EQUALS(true, v3.isDouble());
      TST_ASSERT_EQUALS(false, v3.isString());
      TST_ASSERT_EQUALS(true, v3.isNumeric());
      TST_ASSERT_EQUALS(false, v3.isArray());
      TST_ASSERT_EQUALS(false, v3.isMap());
      TST_ASSERT_EQUALS(-456, v3.asInt());
      TST_ASSERT_EQUALS(-456.789, v3.asDouble());
      TST_ASSERT_EQUALS(true,  v3.isScalar());
      TST_ASSERT_EQUALS(false, v3.isCollection());
      TST_ASSERT_EQUALS(true,  v3.isLeaf());
      TST_ASSERT_EQUALS(COLstring("-456.789"), v3.asString());

      // test converting a double into a string
      v3 = v3.asString();
      TST_ASSERT_EQUALS(COLvar::String, v3.type());
      TST_ASSERT_EQUALS(false, v3.isNone());
      TST_ASSERT_EQUALS(false, v3.isInteger());
      TST_ASSERT_EQUALS(false, v3.isDouble());
      TST_ASSERT_EQUALS(true, v3.isString());
      TST_ASSERT_EQUALS(false, v3.isNumeric());
      TST_ASSERT_EQUALS(false, v3.isArray());
      TST_ASSERT_EQUALS(false, v3.isMap());
      TST_ASSERT_EQUALS(-456, v3.asInt());
      TST_ASSERT_EQUALS(-456.789, v3.asDouble());
      TST_ASSERT_EQUALS(true,  v3.isScalar());
      TST_ASSERT_EQUALS(false, v3.isCollection());
      TST_ASSERT_EQUALS(true,  v3.isLeaf());
      TST_ASSERT_EQUALS(COLstring("-456.789"), v3.asString());
   }

   // test an int-like double
   v1 = -321.0;
   TST_ASSERT_EQUALS(COLvar::Double, v1.type());
   TST_ASSERT_EQUALS(false, v1.isNone());
   TST_ASSERT_EQUALS(false, v1.isInteger());
   TST_ASSERT_EQUALS(true,  v1.isDouble());
   TST_ASSERT_EQUALS(false, v1.isString());
   TST_ASSERT_EQUALS(true,  v1.isNumeric());
   TST_ASSERT_EQUALS(false, v1.isArray());
   TST_ASSERT_EQUALS(false, v1.isMap());
   TST_ASSERT_EQUALS(-321,  v1.asInt());
   TST_ASSERT_EQUALS(-321.0, v1.asDouble());
   TST_ASSERT_EQUALS(true,  v1.isScalar());
   TST_ASSERT_EQUALS(false, v1.isCollection());
   TST_ASSERT_EQUALS(true,  v1.isLeaf());
   TST_ASSERT_EQUALS(COLstring("-321"), v1.asString());
   {
      COLvar v2(v1);
      TST_ASSERT_EQUALS(COLvar::Double, v2.type());
      TST_ASSERT_EQUALS(false, v2.isNone());
      TST_ASSERT_EQUALS(false, v2.isInteger());
      TST_ASSERT_EQUALS(true,  v2.isDouble());
      TST_ASSERT_EQUALS(false, v2.isString());
      TST_ASSERT_EQUALS(true,  v2.isNumeric());
      TST_ASSERT_EQUALS(false, v2.isArray());
      TST_ASSERT_EQUALS(false, v2.isMap());
      TST_ASSERT_EQUALS(-321,  v2.asInt());
      TST_ASSERT_EQUALS(-321.0,v2.asDouble());
      TST_ASSERT_EQUALS(true,  v2.isScalar());
      TST_ASSERT_EQUALS(false, v2.isCollection());
      TST_ASSERT_EQUALS(true,  v2.isLeaf());
      TST_ASSERT_EQUALS(COLstring("-321"), v2.asString());
      TST_ASSERT_EQUALS(0,     v2.size());
   }
   {
      COLvar v3;
      const COLvar& cv3 = v3;
      v3 = v1;
      TST_ASSERT_EQUALS(COLvar::Double, v3.type());
      TST_ASSERT_EQUALS(false, v3.isNone());
      TST_ASSERT_EQUALS(false, v3.isInteger());
      TST_ASSERT_EQUALS(true, v3.isDouble());
      TST_ASSERT_EQUALS(false, v3.isString());
      TST_ASSERT_EQUALS(true, v3.isNumeric());
      TST_ASSERT_EQUALS(false, v3.isArray());
      TST_ASSERT_EQUALS(false, v3.isMap());
      TST_ASSERT_EQUALS(-321, v3.asInt());
      TST_ASSERT_EQUALS(-321.0, v3.asDouble());
      TST_ASSERT_EQUALS(true,  v3.isScalar());
      TST_ASSERT_EQUALS(false, v3.isCollection());
      TST_ASSERT_EQUALS(true,  v3.isLeaf());
      TST_ASSERT_EQUALS(COLstring("-321"), v3.asString());

      v3[27] = 54;
      TST_ASSERT(v3.isArray());
      TST_ASSERT(!v3.isMap());
      TST_ASSERT(cv3.array()[27].asInt() == 54);

      v3["qwerty"] = 101;
      TST_ASSERT(v3.isMap());
      TST_ASSERT(!v3.isArray());
      TST_ASSERT(cv3.map()["qwerty"].asInt() == 101);
   }
}

static void testStringCtor()
{
   COLvar v1("-123.4");
   TST_ASSERT_EQUALS(COLvar::String, v1.type());
   TST_ASSERT_EQUALS(false, v1.isNone());
   TST_ASSERT_EQUALS(false, v1.isInteger());
   TST_ASSERT_EQUALS(false, v1.isDouble());
   TST_ASSERT_EQUALS(true, v1.isString());
   TST_ASSERT_EQUALS(false, v1.isNumeric());
   TST_ASSERT_EQUALS(false, v1.isArray());
   TST_ASSERT_EQUALS(false, v1.isMap());
   TST_ASSERT_EQUALS(-123, v1.asInt());
   TST_ASSERT_EQUALS(-123.4, v1.asDouble());
   TST_ASSERT_EQUALS(COLstring("-123.4"), v1.asString());
   TST_ASSERT_EQUALS(0,     v1.size());
   TST_ASSERT_EQUALS("-123.4", v1.str());
   {
      COLvar v2(v1);
      TST_ASSERT_EQUALS(COLvar::String, v2.type());
      TST_ASSERT_EQUALS(false, v2.isNone());
      TST_ASSERT_EQUALS(false, v2.isInteger());
      TST_ASSERT_EQUALS(false, v2.isDouble());
      TST_ASSERT_EQUALS(true, v2.isString());
      TST_ASSERT_EQUALS(false, v2.isNumeric());
      TST_ASSERT_EQUALS(false, v2.isArray());
      TST_ASSERT_EQUALS(false, v2.isMap());
      TST_ASSERT_EQUALS(-123, v2.asInt());
      TST_ASSERT_EQUALS(-123.4, v2.asDouble());
      TST_ASSERT_EQUALS(COLstring("-123.4"), v2.asString());
   }
   {
      COLvar v3;
      v3 = v1;
      TST_ASSERT_EQUALS(COLvar::String, v3.type());
      TST_ASSERT_EQUALS(false, v3.isNone());
      TST_ASSERT_EQUALS(false, v3.isInteger());
      TST_ASSERT_EQUALS(false, v3.isDouble());
      TST_ASSERT_EQUALS(true, v3.isString());
      TST_ASSERT_EQUALS(false, v3.isNumeric());
      TST_ASSERT_EQUALS(false, v3.isArray());
      TST_ASSERT_EQUALS(false, v3.isMap());
      TST_ASSERT_EQUALS(-123, v3.asInt());
      TST_ASSERT_EQUALS(-123.4, v3.asDouble());
      TST_ASSERT_EQUALS(COLstring("-123.4"), v3.asString());
   }

   // test an int-like double string
   v1 = "-321.000";
   TST_ASSERT_EQUALS(COLvar::String, v1.type());
   TST_ASSERT_EQUALS(false, v1.isNone());
   TST_ASSERT_EQUALS(false, v1.isInteger());
   TST_ASSERT_EQUALS(false, v1.isDouble());
   TST_ASSERT_EQUALS(true,  v1.isString());
   TST_ASSERT_EQUALS(false, v1.isNumeric());
   TST_ASSERT_EQUALS(false, v1.isArray());
   TST_ASSERT_EQUALS(false, v1.isMap());
   TST_ASSERT_EQUALS(-321,  v1.asInt());
   TST_ASSERT_EQUALS(-321,  v1.asDouble());
   TST_ASSERT_EQUALS(true,  v1.isScalar());
   TST_ASSERT_EQUALS(false, v1.isCollection());
   TST_ASSERT_EQUALS(true,  v1.isLeaf());
   TST_ASSERT_EQUALS(COLstring("-321.000"), v1.asString());
   {
      COLvar v2(v1);
      TST_ASSERT_EQUALS(COLvar::String, v2.type());
      TST_ASSERT_EQUALS(false, v2.isNone());
      TST_ASSERT_EQUALS(false, v2.isInteger());
      TST_ASSERT_EQUALS(false, v2.isDouble());
      TST_ASSERT_EQUALS(true, v2.isString());
      TST_ASSERT_EQUALS(false, v2.isNumeric());
      TST_ASSERT_EQUALS(false, v2.isArray());
      TST_ASSERT_EQUALS(false, v2.isMap());
      TST_ASSERT_EQUALS(-321, v2.asInt());
      TST_ASSERT_EQUALS(-321.0, v2.asDouble());
      TST_ASSERT_EQUALS(COLstring("-321.000"), v2.asString());

      // test converting string into a double
      COLvar v4 = v2.asDouble();
      TST_ASSERT_EQUALS(COLvar::Double, v4.type());
      TST_ASSERT_EQUALS(false, v4.isNone());
      TST_ASSERT_EQUALS(false, v4.isInteger());
      TST_ASSERT_EQUALS(true, v4.isDouble());
      TST_ASSERT_EQUALS(false, v4.isString());
      TST_ASSERT_EQUALS(true, v4.isNumeric());
      TST_ASSERT_EQUALS(false, v4.isArray());
      TST_ASSERT_EQUALS(false, v4.isMap());
      TST_ASSERT_EQUALS(-321, v4.asInt());
      TST_ASSERT_EQUALS(-321.0, v4.asDouble());
      TST_ASSERT_EQUALS(COLstring("-321"), v4.asString());
   }
   {
      COLvar v3;
      v3 = v1;
      TST_ASSERT_EQUALS(COLvar::String, v3.type());
      TST_ASSERT_EQUALS(false, v3.isNone());
      TST_ASSERT_EQUALS(false, v3.isInteger());
      TST_ASSERT_EQUALS(false, v3.isDouble());
      TST_ASSERT_EQUALS(true,  v3.isString());
      TST_ASSERT_EQUALS(false, v3.isNumeric());
      TST_ASSERT_EQUALS(false, v3.isArray());
      TST_ASSERT_EQUALS(false, v3.isMap());
      TST_ASSERT_EQUALS(-321,  v3.asInt());
      TST_ASSERT_EQUALS(-321.0,v3.asDouble());
      TST_ASSERT_EQUALS(true,  v3.isScalar());
      TST_ASSERT_EQUALS(false, v3.isCollection());
      TST_ASSERT_EQUALS(true,  v3.isLeaf());
      TST_ASSERT_EQUALS(COLstring("-321.000"), v3.asString());
   }
}

static void testStringAppend()
{
   COL_FUNCTION(testStringAppend);
   COLvar var;

   TST_ASSERT_EQUALS(COLvar::None, var.type());
   var.append("append to a COLvar::None");
   TST_ASSERT_EQUALS("\"append to a COLvar::None\"", var.json());

   var = 123;
   TST_ASSERT_EQUALS(COLvar::Integer, var.type());
   var.append(" append to a COLvar::Integer");
   TST_ASSERT_EQUALS("\"123 append to a COLvar::Integer\"", var.json());
   TST_ASSERT_EQUALS("123 append to a COLvar::Integer", var.str());

   var = -765.432;
   TST_ASSERT_EQUALS(COLvar::Double, var.type());
   var.append(" append to a COLvar::Double");
   TST_ASSERT_EQUALS("\"-765.432 append to a COLvar::Double\"", var.json());

   var = "My String.";
   TST_ASSERT_EQUALS(COLvar::String, var.type());
   var.append(" Append to a COLvar::String");
   TST_ASSERT_EQUALS("\"My String. Append to a COLvar::String\"", var.json());
   var.append(". Again.");
   TST_ASSERT_EQUALS("\"My String. Append to a COLvar::String. Again.\"", var.json());

   var.setArrayType();
   var.push_back(22000);
   TST_ASSERT_EQUALS(COLvar::Array, var.type());
   TST_ASSERT_EQUALS("", var.str());
   var.append("Append to a COLvar::Array");
   TST_ASSERT_EQUALS("\"Append to a COLvar::Array\"", var.json());

   var["foo"] = "bar";
   TST_ASSERT_EQUALS(COLvar::Map, var.type());
   TST_ASSERT_EQUALS("",          var.str());
   var.append("Append to a COLvar::Map");
   TST_ASSERT_EQUALS("\"Append to a COLvar::Map\"", var.json());

   var = false;
   TST_ASSERT_EQUALS(COLvar::Boolean, var.type());
   var.append(" Append to a COLvar::Boolean");
   TST_ASSERT_EQUALS("\"false Append to a COLvar::Boolean\"", var.json());

   var = true;
   TST_ASSERT_EQUALS(COLvar::Boolean, var.type());
   var.append(" Append to a COLvar::Boolean");
   TST_ASSERT_EQUALS("\"true Append to a COLvar::Boolean\"", var.json());
}

static void testAssignment()
{
   // test replacing a long string
   {
      // this had leaked in valgrind previously
      COLvar vv("Some string longer than COLstring's internal buffer.");
      TST_ASSERT_EQUALS("Some string longer than COLstring's internal buffer.", vv.asString());
      COLvar zz("Another string.");
      vv = zz;
      TST_ASSERT_EQUALS(COLstring("Another string."), vv.asString());
   }
   {
      // this had leaked in valgrind previously
      COLvar vv("Some string longer than COLstring's internal buffer.");
      TST_ASSERT_EQUALS("Some string longer than COLstring's internal buffer.", vv.asString());
      vv = COLvar();
      TST_ASSERT_EQUALS(COLstring("null"), vv.json());
   }
   {
      // this had leaked in valgrind previously
      COLvar vv("Some string longer than COLstring's internal buffer.");
      TST_ASSERT_EQUALS("Some string longer than COLstring's internal buffer.", vv.asString());
      vv = COLvar(123);
      TST_ASSERT_EQUALS(COLstring("123"), vv.json());
   }
   {
      // this had leaked in valgrind previously
      COLvar vv("Some string longer than COLstring's internal buffer.");
      TST_ASSERT_EQUALS("Some string longer than COLstring's internal buffer.", vv.asString());
      COLvar zz;
      zz.push_back(33);
      zz.push_back(22);
      vv = zz;
      TST_ASSERT_EQUALS(COLstring("[33,22]"), vv.json());
   }
   {
      // this had leaked in valgrind previously
      COLvar vv("Some string longer than COLstring's internal buffer.");
      TST_ASSERT_EQUALS("Some string longer than COLstring's internal buffer.", vv.asString());
      COLvar zz;
      zz["foo"] = "bar";
      zz[23] = 45;
      vv = zz;
      TST_ASSERT_EQUALS(COLstring("{\"23\":45,\"foo\":\"bar\"}"), vv.json());
   }

   // test replacing an Array
   {
      COLvar vv;
      vv.push_back(123);
      vv.push_back(456);
      TST_ASSERT_EQUALS("[123,456]", vv.json());
      COLvar zz("Another string.");
      vv = zz;
      TST_ASSERT_EQUALS(COLstring("\"Another string.\""), vv.json());
   }
   {
      COLvar vv;
      vv.push_back(123);
      vv.push_back(456);
      TST_ASSERT_EQUALS("[123,456]", vv.json());
      vv = COLvar();
      TST_ASSERT_EQUALS(COLstring("null"), vv.json());
   }
   {
      COLvar vv;
      vv.push_back(123);
      vv.push_back(456);
      TST_ASSERT_EQUALS("[123,456]", vv.json());
      vv = COLvar(123);
      TST_ASSERT_EQUALS(COLstring("123"), vv.json());
   }
   {
      COLvar vv;
      vv.push_back(123);
      vv.push_back(456);
      TST_ASSERT_EQUALS("[123,456]", vv.json());
      COLvar zz;
      zz.push_back(33);
      zz.push_back(111);
      zz.push_back(22);
      vv = zz;
      TST_ASSERT_EQUALS(COLstring("[33,111,22]"), vv.json());
   }
   {
      COLvar vv;
      vv.push_back(123);
      vv.push_back(456);
      TST_ASSERT_EQUALS("[123,456]", vv.json());
      COLvar zz;
      zz["foo"] = "bar";
      zz[23] = 45;
      vv = zz;
      TST_ASSERT_EQUALS(COLstring("{\"23\":45,\"foo\":\"bar\"}"), vv.json());
   }

   // test replacing a Map
   {
      COLvar vv;
      vv["one"] = 1;
      vv["too"] = 2;
      TST_ASSERT_EQUALS("{\"one\":1,\"too\":2}", vv.json());
      COLvar zz("Another string.");
      vv = zz;
      TST_ASSERT_EQUALS(COLstring("\"Another string.\""), vv.json());
   }
   {
      COLvar vv;
      vv["one"] = 1;
      vv["too"] = 2;
      TST_ASSERT_EQUALS("{\"one\":1,\"too\":2}", vv.json());
      vv = COLvar();
      TST_ASSERT_EQUALS(COLstring("null"), vv.json());
   }
   {
      COLvar vv;
      vv["one"] = 1;
      vv["too"] = 2;
      TST_ASSERT_EQUALS("{\"one\":1,\"too\":2}", vv.json());
      vv = COLvar(123);
      TST_ASSERT_EQUALS(COLstring("123"), vv.json());
   }
   {
      COLvar vv;
      vv["one"] = 1;
      vv["too"] = 2;
      TST_ASSERT_EQUALS("{\"one\":1,\"too\":2}", vv.json());
      COLvar zz;
      zz.push_back(33);
      zz.push_back(111);
      zz.push_back(22);
      vv = zz;
      TST_ASSERT_EQUALS(COLstring("[33,111,22]"), vv.json());
   }
   {
      COLvar vv;
      vv["one"] = 1;
      vv["too"] = 2;
      TST_ASSERT_EQUALS("{\"one\":1,\"too\":2}", vv.json());
      COLvar zz;
      zz["foo"] = "bar";
      zz[23] = 45;
      vv = zz;
      TST_ASSERT_EQUALS(COLstring("{\"23\":45,\"foo\":\"bar\"}"), vv.json());
   }
}

static void testArrayAccess()
{
   COLvar v;
   TST_ASSERT_EQUALS(true,  v.isNone());
   TST_ASSERT_EQUALS(false, v.isArray());
   TST_ASSERT_EQUALS(false, v.isMap());
   TST_ASSERT_EQUALS(true,  v.isScalar());
   TST_ASSERT_EQUALS(false, v.isCollection());
   TST_ASSERT_EQUALS(true,  v.isLeaf());
   TST_ASSERT_EQUALS(false, v.asBool());
   TST_ASSERT_EQUALS(0,     v.size());

   v[0] = "testing array access. index 0.";

   // test the array node
   TST_ASSERT_EQUALS(COLvar::Array, v.type());
   TST_ASSERT_EQUALS(false, v.isNone());
   TST_ASSERT_EQUALS(false, v.isInteger());
   TST_ASSERT_EQUALS(false, v.isDouble());
   TST_ASSERT_EQUALS(false, v.isString());
   TST_ASSERT_EQUALS(false, v.isNumeric());
   TST_ASSERT_EQUALS(true,  v.isArray());
   TST_ASSERT_EQUALS(false, v.isMap());
   TST_ASSERT_EQUALS(false, v.asBool());
   TST_ASSERT_EQUALS(0,     v.asInt());
   TST_ASSERT_EQUALS(0,     v.asDouble());
   TST_ASSERT_EQUALS(false, v.isScalar());
   TST_ASSERT_EQUALS(true,  v.isCollection());
   TST_ASSERT_EQUALS(false, v.isLeaf());
   TST_ASSERT_EQUALS(COLstring(""), v.asString());
   TST_ASSERT_EQUALS(1,     v.size());

   // test the array element
   TST_ASSERT_EQUALS(COLvar::String, v[0].type());
   TST_ASSERT_EQUALS(false, v[0].isNone());
   TST_ASSERT_EQUALS(false, v[0].isInteger());
   TST_ASSERT_EQUALS(false, v[0].isDouble());
   TST_ASSERT_EQUALS(true,  v[0].isString());
   TST_ASSERT_EQUALS(false, v[0].isNumeric());
   TST_ASSERT_EQUALS(false, v[0].isArray());
   TST_ASSERT_EQUALS(false, v[0].isMap());
   TST_ASSERT_EQUALS(0,     v[0].asInt());
   TST_ASSERT_EQUALS(0,     v[0].asDouble());
   TST_ASSERT_EQUALS(true,  v[0].isScalar());
   TST_ASSERT_EQUALS(false, v[0].isCollection());
   TST_ASSERT_EQUALS(true,  v[0].isLeaf());
   TST_ASSERT_EQUALS(0,     v[0].size());
   TST_ASSERT_EQUALS(COLstring("testing array access. index 0."), v[0].asString());

   v[1] = "testing array access. index 1.";

   // test the array node
   TST_ASSERT_EQUALS(COLvar::Array, v.type());
   TST_ASSERT_EQUALS(false, v.isNone());
   TST_ASSERT_EQUALS(false, v.isInteger());
   TST_ASSERT_EQUALS(false, v.isDouble());
   TST_ASSERT_EQUALS(false, v.isString());
   TST_ASSERT_EQUALS(false, v.isNumeric());
   TST_ASSERT_EQUALS(true,  v.isArray());
   TST_ASSERT_EQUALS(false, v.isMap());
   TST_ASSERT_EQUALS(false, v.asBool());
   TST_ASSERT_EQUALS(0,     v.asInt());
   TST_ASSERT_EQUALS(0,     v.asDouble());
   TST_ASSERT_EQUALS(2,     v.size());
   TST_ASSERT_EQUALS("",    v.asString());

   // test the array element
   TST_ASSERT_EQUALS(COLvar::String,v[1].type());
   TST_ASSERT_EQUALS(false,         v[1].isNone());
   TST_ASSERT_EQUALS(false,         v[1].isInteger());
   TST_ASSERT_EQUALS(false,         v[1].isDouble());
   TST_ASSERT_EQUALS(true,          v[1].isString());
   TST_ASSERT_EQUALS(false,         v[1].isNumeric());
   TST_ASSERT_EQUALS(false,         v[1].isArray());
   TST_ASSERT_EQUALS(false,         v[1].isMap());
   TST_ASSERT_EQUALS(0,             v[1].asInt());
   TST_ASSERT_EQUALS(0,             v[1].asDouble());
   TST_ASSERT_EQUALS(COLstring("testing array access. index 0."), v[0].asString());
   TST_ASSERT_EQUALS(COLstring("testing array access. index 1."), v[1].asString());

   v[123] = "testing array access. index 123.";

   // test the array node
   TST_ASSERT_EQUALS(COLvar::Array, v.type());
   TST_ASSERT_EQUALS(false, v.isNone());
   TST_ASSERT_EQUALS(false, v.isInteger());
   TST_ASSERT_EQUALS(false, v.isDouble());
   TST_ASSERT_EQUALS(false, v.isString());
   TST_ASSERT_EQUALS(false, v.isNumeric());
   TST_ASSERT_EQUALS(true,  v.isArray());
   TST_ASSERT_EQUALS(false, v.isMap());
   TST_ASSERT_EQUALS(false, v.asBool());
   TST_ASSERT_EQUALS(0,     v.asInt());
   TST_ASSERT_EQUALS(0,     v.asDouble());
   TST_ASSERT_EQUALS("",    v.asString());
   TST_ASSERT_EQUALS(124,   v.size());

   // test the array element
   TST_ASSERT_EQUALS(COLvar::String,v[123].type());
   TST_ASSERT_EQUALS(false,         v[123].isNone());
   TST_ASSERT_EQUALS(false,         v[123].isInteger());
   TST_ASSERT_EQUALS(false,         v[123].isDouble());
   TST_ASSERT_EQUALS(true,          v[123].isString());
   TST_ASSERT_EQUALS(false,         v[123].isNumeric());
   TST_ASSERT_EQUALS(false,         v[123].isArray());
   TST_ASSERT_EQUALS(false,         v[123].isMap());
   TST_ASSERT_EQUALS(0,             v[123].asInt());
   TST_ASSERT_EQUALS(0,             v[123].asDouble());
   TST_ASSERT_EQUALS(COLstring("testing array access. index 0."), v[0].asString());
   TST_ASSERT_EQUALS(COLstring("testing array access. index 1."), v[1].asString());
   TST_ASSERT_EQUALS(COLstring("testing array access. index 123."), v[123].asString());

   v[-1] = "test promotion of COLvar to a map";

   // test the map node
   TST_ASSERT_EQUALS(COLvar::Map, v.type());
   TST_ASSERT_EQUALS(false, v.isNone());
   TST_ASSERT_EQUALS(false, v.isInteger());
   TST_ASSERT_EQUALS(false, v.isDouble());
   TST_ASSERT_EQUALS(false, v.isString());
   TST_ASSERT_EQUALS(false, v.isNumeric());
   TST_ASSERT_EQUALS(false, v.isArray());
   TST_ASSERT_EQUALS(true,  v.isMap());
   TST_ASSERT_EQUALS(false, v.asBool());
   TST_ASSERT_EQUALS(0,     v.asInt());
   TST_ASSERT_EQUALS(0,     v.asDouble());
   TST_ASSERT_EQUALS(false, v.isScalar());
   TST_ASSERT_EQUALS(true,  v.isCollection());
   TST_ASSERT_EQUALS(false, v.isLeaf());
   TST_ASSERT_EQUALS(4,     v.size());
   TST_ASSERT_EQUALS("",    v.asString());

   // test the map element
   TST_ASSERT_EQUALS(COLvar::String,v[-1].type());
   TST_ASSERT_EQUALS(false,         v[-1].isNone());
   TST_ASSERT_EQUALS(false,         v[-1].isInteger());
   TST_ASSERT_EQUALS(false,         v[-1].isDouble());
   TST_ASSERT_EQUALS(true,          v[-1].isString());
   TST_ASSERT_EQUALS(false,         v[-1].isNumeric());
   TST_ASSERT_EQUALS(false,         v[-1].isArray());
   TST_ASSERT_EQUALS(false,         v[-1].isMap());
   TST_ASSERT_EQUALS(false,         v[-1].asBool());
   TST_ASSERT_EQUALS(0,             v[-1].asInt());
   TST_ASSERT_EQUALS(0,             v[-1].asDouble());
   TST_ASSERT_EQUALS(true,          v[-1].isScalar());
   TST_ASSERT_EQUALS(false,         v[-1].isCollection());
   TST_ASSERT_EQUALS(true,          v[-1].isLeaf());
   TST_ASSERT_EQUALS(0,             v[-1].size());
   TST_ASSERT_EQUALS(COLstring("testing array access. index 0."), v[0].asString());
   TST_ASSERT_EQUALS(COLstring("testing array access. index 1."), v[1].asString());
   TST_ASSERT_EQUALS(COLstring("testing array access. index 123."), v[123].asString());
   TST_ASSERT_EQUALS(COLstring("test promotion of COLvar to a map"), v[-1].asString());
}

static void testMapAccess()
{
   COLvar v = 123;
   v["abc"] = -987.654;

   // test the map node
   TST_ASSERT_EQUALS(COLvar::Map,   v.type());
   TST_ASSERT_EQUALS(false,         v.isNone());
   TST_ASSERT_EQUALS(false,         v.isInteger());
   TST_ASSERT_EQUALS(false,         v.isDouble());
   TST_ASSERT_EQUALS(false,         v.isString());
   TST_ASSERT_EQUALS(false,         v.isNumeric());
   TST_ASSERT_EQUALS(false,         v.isArray());
   TST_ASSERT_EQUALS(true,          v.isMap());
   TST_ASSERT_EQUALS(false,         v.asBool());
   TST_ASSERT_EQUALS(0,             v.asInt());
   TST_ASSERT_EQUALS(0,             v.asDouble());
   TST_ASSERT_EQUALS(false,         v.isScalar());
   TST_ASSERT_EQUALS(true,          v.isCollection());
   TST_ASSERT_EQUALS(false,         v.isLeaf());
   TST_ASSERT_EQUALS(1,             v.size());
   TST_ASSERT_EQUALS(COLstring(""), v.asString());

   // test the map element
   TST_ASSERT_EQUALS(COLvar::Double, v["abc"].type());
   TST_ASSERT_EQUALS(false,          v["abc"].isNone());
   TST_ASSERT_EQUALS(false,          v["abc"].isInteger());
   TST_ASSERT_EQUALS(true,           v["abc"].isDouble());
   TST_ASSERT_EQUALS(false,          v["abc"].isString());
   TST_ASSERT_EQUALS(true,           v["abc"].isNumeric());
   TST_ASSERT_EQUALS(false,          v["abc"].isArray());
   TST_ASSERT_EQUALS(false,          v["abc"].isMap());
   TST_ASSERT_EQUALS(-987,           v["abc"].asInt());
   TST_ASSERT_EQUALS(-987.654,       v["abc"].asDouble());
   TST_ASSERT_EQUALS("-987.654",     v["abc"].asString());
   TST_ASSERT_EQUALS(true,           v["abc"].isScalar());
   TST_ASSERT_EQUALS(false,          v["abc"].isCollection());
   TST_ASSERT_EQUALS(true,           v["abc"].isLeaf());
   TST_ASSERT_EQUALS("-987.654",     v.map()["abc"].asString());
   TST_ASSERT_EQUALS(-987.654,       v.map()["abc"].asDouble());

   // convert map to an int
   v = 5;
   TST_ASSERT_EQUALS(COLvar::Integer, v.type());
   TST_ASSERT_EQUALS(false,         v.isNone());
   TST_ASSERT_EQUALS(true,          v.isInteger());
   TST_ASSERT_EQUALS(false,         v.isDouble());
   TST_ASSERT_EQUALS(false,         v.isString());
   TST_ASSERT_EQUALS(true,          v.isNumeric());
   TST_ASSERT_EQUALS(false,         v.isArray());
   TST_ASSERT_EQUALS(false,         v.isMap());
   TST_ASSERT_EQUALS(true,          v.asBool());
   TST_ASSERT_EQUALS(5,             v.asInt());
   TST_ASSERT_EQUALS(5,             v.asDouble());
   TST_ASSERT_EQUALS(true,          v.isScalar());
   TST_ASSERT_EQUALS(false,         v.isCollection());
   TST_ASSERT_EQUALS(true,          v.isLeaf());
   TST_ASSERT_EQUALS(0,             v.size());
   TST_ASSERT_EQUALS(COLstring("5"),v.asString());

   // convert to a map
   v[123] = "fred";

   // the node
   TST_ASSERT_EQUALS(COLvar::Array, v.type());
   TST_ASSERT_EQUALS(false,         v.isNone());
   TST_ASSERT_EQUALS(false,         v.isInteger());
   TST_ASSERT_EQUALS(false,         v.isDouble());
   TST_ASSERT_EQUALS(false,         v.isString());
   TST_ASSERT_EQUALS(false,         v.isNumeric());
   TST_ASSERT_EQUALS(true,          v.isArray());
   TST_ASSERT_EQUALS(false,         v.isMap());
   TST_ASSERT_EQUALS(false,         v.asBool());
   TST_ASSERT_EQUALS(0,             v.asInt());
   TST_ASSERT_EQUALS(0,             v.asDouble());
   TST_ASSERT_EQUALS(COLstring(""), v.asString());
   TST_ASSERT_EQUALS(123+1,         v.size());

   // the element
   TST_ASSERT_EQUALS(COLvar::String, v[123].type());
   TST_ASSERT_EQUALS(false,          v[123].isNone());
   TST_ASSERT_EQUALS(false,          v[123].isInteger());
   TST_ASSERT_EQUALS(false,          v[123].isDouble());
   TST_ASSERT_EQUALS(true,           v[123].isString());
   TST_ASSERT_EQUALS(false,          v[123].isNumeric());
   TST_ASSERT_EQUALS(false,          v[123].isArray());
   TST_ASSERT_EQUALS(false,          v[123].isMap());
   TST_ASSERT_EQUALS(0,              v[123].asInt());
   TST_ASSERT_EQUALS(0,              v[123].asDouble());
   TST_ASSERT_EQUALS("fred",         v[123].asString());
   TST_ASSERT_EQUALS("fred",         v[123].asString());
}

static void testMapErase()
{
   COL_FUNCTION(testMapErase);
   COLvar var;
   var.erase("Nine");
   TST_ASSERT_EQUALS("null", var.json());

   var["seven"] = 7;
   var["Three"] = 3;
   var["One"] = 1;
   TST_ASSERT_EQUALS("{\"One\":1,\"seven\":7,\"Three\":3}", var.json());

   var.erase("Three");
   TST_ASSERT_EQUALS("{\"One\":1,\"seven\":7}", var.json());

   var.erase("One");
   TST_ASSERT_EQUALS("{\"seven\":7}", var.json());

   var.erase("seven");
   TST_ASSERT_EQUALS("{}", var.json());
}

static void testMixedArrayAndMapAccess()
{
   COLvar v;
   for (int i = 0; i <= 3000; ++i) {
      v[i] = i*10;
      char buf[64];
      snprintf(buf, 64, "%d", i*10);

      // the node
      TST_ASSERT_EQUALS(i*10,          v[i].asInt());
      TST_ASSERT_EQUALS(false,         v.isNone());
      TST_ASSERT_EQUALS(false,         v.isInteger());
      TST_ASSERT_EQUALS(false,         v.isDouble());
      TST_ASSERT_EQUALS(false,         v.isString());
      TST_ASSERT_EQUALS(false,         v.isNumeric());
      TST_ASSERT_EQUALS(false,         v.asBool());
      TST_ASSERT_EQUALS(0,             v.asInt());
      TST_ASSERT_EQUALS(0,             v.asDouble());
      TST_ASSERT_EQUALS(COLstring(""), v.asString());

      // the element
      TST_ASSERT_EQUALS(i*10,           v[i].asInt());
      TST_ASSERT_EQUALS(COLvar::Integer,v[i].type());
      TST_ASSERT_EQUALS(false,          v[i].isNone());
      TST_ASSERT_EQUALS(true,           v[i].isInteger());
      TST_ASSERT_EQUALS(false,          v[i].isDouble());
      TST_ASSERT_EQUALS(false,          v[i].isString());
      TST_ASSERT_EQUALS(true,           v[i].isNumeric());
      TST_ASSERT_EQUALS(false,          v[i].isArray());
      TST_ASSERT_EQUALS(false,          v[i].isMap());
      TST_ASSERT_EQUALS(i*10,           v[i].asDouble());
      TST_ASSERT_EQUALS(buf,            v[i].asString());

      // the type of the container changes from Array to Map after 2000
      //TST_ASSERT_EQUALS((i <= 2000 ? COLvar::Array : COLvar::Map), v.type());
      TST_ASSERT_EQUALS(COLvar::Array, v.type());
   }

   v[4000] = 0;
   TST_ASSERT_EQUALS(COLvar::Array, v.type());

   v[8000] = 0;
   TST_ASSERT_EQUALS(COLvar::Array, v.type());

   v[170000] = 0;
   TST_ASSERT_EQUALS(COLvar::Map, v.type());
   TST_ASSERT_EQUALS(false,       v.isScalar());
   TST_ASSERT_EQUALS(true,        v.isCollection());
   TST_ASSERT_EQUALS(false,       v.isLeaf());
   TST_ASSERT_EQUALS(false,       v.asBool());
   TST_ASSERT_EQUALS(0,           v.asInt());
   TST_ASSERT_EQUALS(0,           v.asDouble());
   TST_ASSERT_EQUALS("",          v.asString());
}

// Takes single argument for both parsing and the answer
static void testJsonHelper(COLstring json) {
   COLvar v;
   v.parse(json);
   TST_ASSERT_EQUALS(json, v.json());
}

static void testCollectionExceptions()
{
   COLvar v;
   const COLvar& cv = v;
   TST_ASSERT_EXCEPTION(v.map()["abc123"].isScalar(), COLerror("Failed precondition: type_ == Map", COLerror::PreCondition));
   TST_ASSERT_EXCEPTION(cv.array()[999].isScalar(), COLerror("Failed precondition: type_ == Array && u_.vec_", COLerror::PreCondition));

   v[27] = 123;
   TST_ASSERT_EXCEPTION(v.map()["abc123"].isScalar(), COLerror("Failed precondition: type_ == Map", COLerror::PreCondition));
   TST_ASSERT_EXCEPTION(cv.array()[999].isScalar(), COLerror("Failed precondition: n >= 0 && n < size_", COLerror::PreCondition));
   TST_ASSERT_EQUALS(123, cv.array()[27].asInt());

   v["bob"] = 123;
   TST_ASSERT_EXCEPTION(cv.array()[999].isScalar(), COLerror("Failed precondition: type_ == Array && u_.vec_", COLerror::PreCondition));
   TST_ASSERT_EXCEPTION(cv.array()[123].isScalar(), COLerror("Failed precondition: type_ == Array && u_.vec_", COLerror::PreCondition));
   TST_ASSERT_EQUALS(123, cv.map()["bob"].asInt());
   TST_ASSERT_EQUALS(true, cv.map()["bob"].isScalar());
   TST_ASSERT_EXCEPTION(cv.map()["abc123"].isScalar(), COLerror("Failed precondition: place != NULL", COLerror::PreCondition));
}

static void testArrayPushBack()
{
   COLvar v;
   TST_ASSERT_EQUALS(0, v.size());
   TST_ASSERT_EQUALS(COLvar::None, v.type());

   v.setArrayType();
   TST_ASSERT_EQUALS(0, v.size());
   TST_ASSERT_EQUALS(COLvar::Array, v.type());

   v.setMapType();
   TST_ASSERT_EQUALS(0, v.size());
   TST_ASSERT_EQUALS(COLvar::Map, v.type());

   v.setArrayType();
   TST_ASSERT_EQUALS(0, v.size());
   TST_ASSERT_EQUALS(COLvar::Array, v.type());

   v.setArrayType();
   TST_ASSERT_EQUALS(0, v.size());
   TST_ASSERT_EQUALS(COLvar::Array, v.type());

   v.push_back(-123);
   TST_ASSERT_EQUALS(COLvar::Array, v.type());
   TST_ASSERT_EQUALS(1, v.size());

   v.push_back("bob");
   TST_ASSERT_EQUALS(COLvar::Array, v.type());
   TST_ASSERT_EQUALS(2, v.size());

   v.push_back(COLvar());
   TST_ASSERT_EQUALS(COLvar::Array, v.type());
   TST_ASSERT_EQUALS(3, v.size());

   v.push_back(77.8899);
   TST_ASSERT_EQUALS(COLvar::Array, v.type());
   TST_ASSERT_EQUALS(4, v.size());

   COLvar m;
   m["foo"] = "bar";
   v.push_back(m);
   TST_ASSERT_EQUALS(COLvar::Array, v.type());
   TST_ASSERT_EQUALS(5, v.size());

   v.push_back();
   TST_ASSERT_EQUALS(COLvar::Array, v.type());
   TST_ASSERT_EQUALS(6, v.size());

   v.push_back(0);
   TST_ASSERT_EQUALS(COLvar::Array, v.type());
   TST_ASSERT_EQUALS(7, v.size());

   v.push_back();
   TST_ASSERT_EQUALS(COLvar::Array, v.type());
   TST_ASSERT_EQUALS(8, v.size());

   TST_ASSERT_EQUALS("[-123,\"bob\",null,77.8899,{\"foo\":\"bar\"},null,0,null]", v.json());
}

static void testEscape()
{
   TST_ASSERT_EQUALS(COLstring("foo"), COLvar::escape("foo"));
   TST_ASSERT_EQUALS(COLstring("'foo'"), COLvar::escape("'foo'"));
   TST_ASSERT_EQUALS(COLstring("\\\"'foo\\\"'"), COLvar::escape("\"'foo\"'"));
}

static void testBinary()
{
   COL_FUNCTION(testBinary);

   COLvar v0;
   int BytesConsumed = 0;
   TST_ASSERT_EQUALS(COLvar::kFAILURE, v0.parseBinary(0, 50000, &BytesConsumed));
   TST_ASSERT_EQUALS(COLvar::kFAILURE, v0.parseBinary("not parsed", -1, &BytesConsumed));
   TST_ASSERT_EQUALS(COLvar::kNEED_MORE_DATA, v0.parseBinary("not parsed", 0, &BytesConsumed));
   TST_ASSERT_EQUALS(COLvar::kFAILURE, v0.parseBinary("bad binary COLvar", 17, &BytesConsumed));

   COLvar v;
   COLstring Buffer;

   v.toBinary(&Buffer);
   TST_ASSERT_EQUALS(1, Buffer.size());
   TST_ASSERT_EQUALS(COLvar::None, Buffer[0]);

   COLvar vp;
   COLvar::ParseState status = vp.parseBinary(Buffer.data(), Buffer.size(), &BytesConsumed);
   TST_ASSERT_EQUALS(1, BytesConsumed);
   TST_ASSERT_EQUALS(COLvar::kSUCCESS, status);
   TST_ASSERT_EQUALS(true, vp.isNull());

   Buffer += "extra bytes";
   BytesConsumed = 0;
   status = vp.parseBinary(Buffer.data(), Buffer.size(), &BytesConsumed);
   TST_ASSERT_EQUALS(COLvar::kSUCCESS, status);
   TST_ASSERT_EQUALS(1, BytesConsumed);
   TST_ASSERT_EQUALS(true, vp.isNull());

   v = true;
   Buffer.clear();
   v.toBinary(&Buffer);
   TST_ASSERT_EQUALS(1, Buffer.size());
   TST_ASSERT_EQUALS(COLvar::BooleanTrue, Buffer[0]);
   TST_ASSERT_EQUALS(COLstring("true"), v.json());
   BytesConsumed = 33;
   status = vp.parseBinary(Buffer.data(), Buffer.size(), &BytesConsumed);
   TST_ASSERT_EQUALS(COLvar::kSUCCESS, status);
   TST_ASSERT_EQUALS(1, BytesConsumed);
   TST_ASSERT_EQUALS(COLvar::Boolean, vp.type());
   TST_ASSERT_EQUALS(1, vp.asInt());
   TST_ASSERT_EQUALS(true, vp.asBool());
   TST_ASSERT_EQUALS(COLstring("true"), vp.json());
   TST_ASSERT_EQUALS(v.json(), vp.json());

   v = false;
   Buffer.clear();
   v.toBinary(&Buffer);
   TST_ASSERT_EQUALS(1, Buffer.size());
   TST_ASSERT_EQUALS(COLvar::BooleanFalse, Buffer[0]);
   TST_ASSERT_EQUALS(COLstring("false"), v.json());
   BytesConsumed = 33;
   status = vp.parseBinary(Buffer.data(), Buffer.size(), &BytesConsumed);
   TST_ASSERT_EQUALS(COLvar::kSUCCESS, status);
   TST_ASSERT_EQUALS(1, BytesConsumed);
   TST_ASSERT_EQUALS(COLvar::Boolean, vp.type());
   TST_ASSERT_EQUALS(0, vp.asInt());
   TST_ASSERT_EQUALS(false, vp.asBool());
   TST_ASSERT_EQUALS(COLstring("false"), vp.json());
   TST_ASSERT_EQUALS(v.json(), vp.json());

   v = 0;
   Buffer.clear();
   v.toBinary(&Buffer);
   COL_DUMP("0 as varint buffer", Buffer.data(), Buffer.size());
   TST_ASSERT_EQUALS((1 + 0), Buffer.size());
   TST_ASSERT_EQUALS(COLvar::VarIntPositive, (int)Buffer[0]);

   v = 14;
   Buffer.clear();
   v.toBinary(&Buffer);
   COL_DUMP("14 as varint buffer", Buffer.data(), Buffer.size());
   TST_ASSERT_EQUALS((1 + 0), Buffer.size());
   TST_ASSERT_EQUALS(COLvar::VarIntPositive, 0xf & (unsigned char)Buffer[0]);

   v = -14;
   Buffer.clear();
   v.toBinary(&Buffer);
   COL_DUMP("-14 as varint buffer", Buffer.data(), Buffer.size());
   TST_ASSERT_EQUALS((1 + 0), Buffer.size());
   TST_ASSERT_EQUALS(COLvar::VarIntNegative, 0xf & (unsigned char)Buffer[0]);

   v = 15;
   Buffer.clear();
   v.toBinary(&Buffer);
   COL_DUMP("15 as varint buffer", Buffer.data(), Buffer.size());
   TST_ASSERT_EQUALS((1 + 1), Buffer.size());
   TST_ASSERT_EQUALS(COLvar::VarIntPositive, 0xf & (unsigned char)Buffer[0]);
   TST_ASSERT_EQUALS(15, (unsigned)(unsigned char)Buffer[1]); // varint encoded abs

   v = -15;
   Buffer.clear();
   v.toBinary(&Buffer);
   COL_DUMP("-15 as varint buffer", Buffer.data(), Buffer.size());
   TST_ASSERT_EQUALS((1 + 1), Buffer.size());
   TST_ASSERT_EQUALS(COLvar::VarIntNegative, 0xf & (unsigned char)Buffer[0]);
   TST_ASSERT_EQUALS(15, (unsigned)(unsigned char)Buffer[1]); // varint encoded abs

   v = 127;
   Buffer.clear();
   v.toBinary(&Buffer);
   COL_DUMP("127 as varint buffer", Buffer.data(), Buffer.size());
   TST_ASSERT_EQUALS((1 + 1), Buffer.size());
   TST_ASSERT_EQUALS(COLvar::VarIntPositive, 0xf & (unsigned char)Buffer[0]);
   TST_ASSERT_EQUALS(127, (unsigned)(unsigned char)Buffer[1]); // varint encoded abs

   v = -127;
   Buffer.clear();
   v.toBinary(&Buffer);
   COL_DUMP("-127 as varint buffer", Buffer.data(), Buffer.size());
   TST_ASSERT_EQUALS((1 + 1), Buffer.size());
   TST_ASSERT_EQUALS(COLvar::VarIntNegative, 0xf & (unsigned char)Buffer[0]);
   TST_ASSERT_EQUALS(127, (unsigned)(unsigned char)Buffer[1]); // varint encoded abs

   v = 128;
   Buffer.clear();
   v.toBinary(&Buffer);
   COL_DUMP("128 as varint buffer", Buffer.data(), Buffer.size());
   TST_ASSERT_EQUALS((1 + 2), Buffer.size());
   TST_ASSERT_EQUALS(COLvar::VarIntPositive, 0xf & (unsigned char)Buffer[0]);
   TST_ASSERT_EQUALS(128, (unsigned)(unsigned char)Buffer[1]); // varint encoded abs

   v = -128;
   Buffer.clear();
   v.toBinary(&Buffer);
   COL_DUMP("-128 as varint buffer", Buffer.data(), Buffer.size());
   TST_ASSERT_EQUALS((1 + 2), Buffer.size());
   TST_ASSERT_EQUALS(COLvar::VarIntNegative, 0xf & (unsigned char)Buffer[0]);
   TST_ASSERT_EQUALS(128, (unsigned)(unsigned char)Buffer[1]); // varint encoded abs

   v = 1000;
   Buffer.clear();
   v.toBinary(&Buffer);
   COL_DUMP("1000 as varint buffer", Buffer.data(), Buffer.size());
   TST_ASSERT_EQUALS((1 + 2), Buffer.size());
   TST_ASSERT_EQUALS(COLvar::VarIntPositive, 0xf & (unsigned char)Buffer[0]);

   v = -1000;
   Buffer.clear();
   v.toBinary(&Buffer);
   COL_DUMP("-1000 as varint buffer", Buffer.data(), Buffer.size());
   TST_ASSERT_EQUALS((1 + 2), Buffer.size());
   TST_ASSERT_EQUALS(COLvar::VarIntNegative, 0xf & (unsigned char)Buffer[0]);

   // test varint integer encoding for big and small and positive and negative integers
   for (int ii = -3000; ii <= 3000; ++ii) {
      v = ii;
      const int abs = (ii >= 0) ? ii : -ii;
      Buffer.clear();
      v.toBinary(&Buffer);
      COL_DUMP(ii << " as varint buffer", Buffer.data(), Buffer.size());
      if (ii < -127 || ii > 127) {
         // type byte is followed by 2 varint bytes
         TST_ASSERT_EQUALS((1 + 2), Buffer.size());
         // the hi bits must be set to 15
         TST_ASSERT(((unsigned char)Buffer[0] & 0xf0) == 0xf0);
         TST_ASSERT_EQUALS((0x80), (0x80 & (unsigned)(unsigned char)Buffer[1]));
         TST_ASSERT_EQUALS(((unsigned)abs & 0x7f), (0x7f & (unsigned)(unsigned char)Buffer[1]));
         TST_ASSERT_EQUALS(0, (0x80 & (unsigned)(unsigned char)Buffer[2]));
         TST_ASSERT_EQUALS(((unsigned)(abs & 0x2f80) >> 7), (0x7f & (unsigned)(unsigned char)Buffer[2]));
      }
      else if (ii <= -15 || ii >= 15) {
         // type byte is followed by 1 varint byte
         TST_ASSERT_EQUALS((1 + 1), Buffer.size());
         // the hi bits must be set to 15
         TST_ASSERT(((unsigned char)Buffer[0] & 0xf0) == 0xf0);
         TST_ASSERT_EQUALS(abs, (unsigned)(unsigned char)Buffer[1]);
         TST_ASSERT_EQUALS(0, (0x80 & (unsigned)(unsigned char)Buffer[1]));
      }
      else {
         // the integer absolute value of 14 or less is encoded in the
         // hi bits of the type byte
         COLASSERT(ii >= -14 && ii <= 14);
         TST_ASSERT_EQUALS((1 + 0), Buffer.size());
         // the hi bits must NOT be set to 15
         TST_ASSERT(((unsigned char)Buffer[0] & 0xf0) != 0xf0);
         TST_ASSERT_EQUALS(abs, ((unsigned)(unsigned char)Buffer[0] >> 4));
      }
      if (ii < 0) {
         TST_ASSERT_EQUALS(COLvar::VarIntNegative, 0xf & (unsigned char)Buffer[0]);
      } else {
         TST_ASSERT_EQUALS(COLvar::VarIntPositive, 0xf & (unsigned char)Buffer[0]);
      }
   }

   v = 0x01020304;
   Buffer.clear();
   v.toBinary(&Buffer);
   COL_DUMP("0x01020304 as varint buffer", Buffer.data(), Buffer.size());
   TST_ASSERT_EQUALS((1 + 4), Buffer.size());
   TST_ASSERT_EQUALS(COLvar::VarIntPositive, 0xf & (unsigned char)Buffer[0]);

   BytesConsumed = 33;
   status = vp.parseBinary(Buffer.data(), Buffer.size(), &BytesConsumed);
   TST_ASSERT_EQUALS(COLvar::kSUCCESS, status);
   TST_ASSERT_EQUALS(5, BytesConsumed);
   TST_ASSERT_EQUALS(COLvar::Integer, vp.type());
   TST_ASSERT_EQUALS(0x01020304, vp.asInt());

   BytesConsumed = 777;
   status = vp.parseBinary(Buffer.data(), 0, &BytesConsumed);
   TST_ASSERT_EQUALS(COLvar::kNEED_MORE_DATA, status);
   TST_ASSERT_EQUALS(0, BytesConsumed);

   BytesConsumed = 484;
   status = vp.parseBinary(Buffer.data(), 1, &BytesConsumed);
   TST_ASSERT_EQUALS(COLvar::kNEED_MORE_DATA, status);
   TST_ASSERT_EQUALS(0, BytesConsumed);

   BytesConsumed = 84;
   status = vp.parseBinary(Buffer.data(), 2, &BytesConsumed);
   TST_ASSERT_EQUALS(COLvar::kNEED_MORE_DATA, status);
   TST_ASSERT_EQUALS(0, BytesConsumed);

   BytesConsumed = -3445;
   status = vp.parseBinary(Buffer.data(), 3, &BytesConsumed);
   TST_ASSERT_EQUALS(COLvar::kNEED_MORE_DATA, status);
   TST_ASSERT_EQUALS(0, BytesConsumed);

   BytesConsumed = 123;
   status = vp.parseBinary(Buffer.data(), 4, &BytesConsumed);
   TST_ASSERT_EQUALS(COLvar::kNEED_MORE_DATA, status);
   TST_ASSERT_EQUALS(0, BytesConsumed);

   BytesConsumed = 999;
   status = vp.parseBinary(Buffer.data(), 5, &BytesConsumed);
   TST_ASSERT_EQUALS(COLvar::kSUCCESS, status);
   TST_ASSERT_EQUALS(5, BytesConsumed);

   v = -1234.125e59;
   Buffer.clear();
   v.toBinary(&Buffer);
   TST_ASSERT_EQUALS((1 + 8), Buffer.size());
   TST_ASSERT_EQUALS(COLvar::Double, Buffer[0]);

   BytesConsumed = 999;
   status = vp.parseBinary(Buffer.data(), Buffer.size(), &BytesConsumed);
   TST_ASSERT_EQUALS(COLvar::kSUCCESS, status);
   TST_ASSERT_EQUALS(9, BytesConsumed);
   TST_ASSERT_EQUALS(-1234.125e59, vp.asDouble());

   for (int ii = 0; ii <= 8; ++ii) {
      BytesConsumed = -99;
      status = vp.parseBinary(Buffer.data(), ii, &BytesConsumed);
      TST_ASSERT_EQUALS(COLvar::kNEED_MORE_DATA, status);
      TST_ASSERT_EQUALS(0, BytesConsumed);
   }

   v = "Hello, world!";
   Buffer.clear();
   v.toBinary(&Buffer);
   TST_ASSERT_EQUALS((1+0+13), Buffer.size());
   TST_ASSERT_EQUALS(COLvar::String, 0xf & (unsigned char)Buffer[0]);

   BytesConsumed = 999;
   status = vp.parseBinary(Buffer.data(), Buffer.size(), &BytesConsumed);
   TST_ASSERT_EQUALS(COLvar::kSUCCESS, status);
   TST_ASSERT_EQUALS((1+0+13), BytesConsumed);
   TST_ASSERT_EQUALS(COLstring("Hello, world!"), vp.asString());

   for (int ii = 0; ii < (1+0+13); ++ii) {
      BytesConsumed = -99;
      status = vp.parseBinary(Buffer.data(), ii, &BytesConsumed);
      TST_ASSERT_EQUALS(COLvar::kNEED_MORE_DATA, status);
      TST_ASSERT_EQUALS(0, BytesConsumed);
   }

   v.reset();
   v.setArrayType();
   Buffer.clear();
   v.toBinary(&Buffer);
   TST_ASSERT_EQUALS((1+0), Buffer.size());
   TST_ASSERT_EQUALS(COLvar::Array, 0xf & (unsigned char)Buffer[0]);

   Buffer.clear();
   v.push_back(-12345678);
   v.push_back(87);
   v.push_back(3000000);
   v.toBinary(&Buffer);
   TST_ASSERT_EQUALS(COLvar::Array, 0xf & (unsigned char)Buffer[0]);

   v.reset();
   v.setMapType();
   Buffer.clear();
   v.toBinary(&Buffer);
   TST_ASSERT_EQUALS((1 + 0), Buffer.size());
   TST_ASSERT_EQUALS(COLvar::Map, 0xf & (unsigned char)Buffer[0]);

   v.reset();
   v.setMapType();
   Buffer.clear();
   v["abc"] = "HI";
   TST_ASSERT_EQUALS(COLstring("{\"abc\":\"HI\"}"), v.json());
   COL_TRC_DUMP("before toBinary", Buffer.data(), Buffer.size());
   v.toBinary(&Buffer);
   COL_TRC_DUMP("after toBinary", Buffer.data(), Buffer.size());
   TST_ASSERT_EQUALS((1 + 0 + (0+1+3) + (1+0+2)), Buffer.size());
   TST_ASSERT_EQUALS(COLvar::Map, 0xf & (unsigned char)Buffer[0]);

   BytesConsumed = 123;
   status = vp.parseBinary(Buffer.data(), 2, &BytesConsumed);
   TST_ASSERT_EQUALS(COLvar::kNEED_MORE_DATA, status);

   BytesConsumed = 123;
   status = vp.parseBinary(Buffer.data(), Buffer.size(), &BytesConsumed);
   TST_ASSERT_EQUALS(COLvar::kSUCCESS, status);
   TST_ASSERT_EQUALS(COLstring("{\"abc\":\"HI\"}"), v.json());
   TST_ASSERT_EQUALS(COLstring("{\"abc\":\"HI\"}"), vp.json());
   TST_ASSERT_EQUALS(v.json(), vp.json());
   TST_ASSERT_EQUALS((1 + 0 + (0+1+3) + (1+0+2)), BytesConsumed);

   Buffer.clear();
   v.reset();
   v["A"] = true;
   v.toBinary(&Buffer);
   status = vp.parseBinary(Buffer.data(), Buffer.size(), &BytesConsumed);
   TST_ASSERT_EQUALS(COLvar::kSUCCESS, status);
   TST_ASSERT_EQUALS(COLstring("{\"A\":true}"), v.json());
   TST_ASSERT_EQUALS(COLstring("{\"A\":true}"), vp.json());
   TST_ASSERT_EQUALS(v.json(), vp.json());
   TST_ASSERT_EQUALS((1 + 0 + (0+1+1) + 1), BytesConsumed);

   Buffer.clear();
   v.reset();
   v["B"].push_back() = false;
   v.toBinary(&Buffer);
   status = vp.parseBinary(Buffer.data(), Buffer.size(), &BytesConsumed);
   TST_ASSERT_EQUALS(COLvar::kSUCCESS, status);
   TST_ASSERT_EQUALS(COLstring("{\"B\":[false]}"), v.json());
   TST_ASSERT_EQUALS(COLstring("{\"B\":[false]}"), vp.json());
   TST_ASSERT_EQUALS(v.json(), vp.json());
   TST_ASSERT_EQUALS((1 + 0 + (0+1+1) + (1 + 0 + 1)), BytesConsumed);

   // check array encoding from 0 to 300 elements
   v.reset();
   v.setArrayType();
   for (int ii = 0; ii <= 260; ++ii) {
      if (ii > 0) {
         v.push_back().set(ii * 10 + 10000);
      }

      // convert array to binary
      Buffer.clear();
      v.toBinary(&Buffer);
      TST_ASSERT_EQUALS(COLvar::Array, 0xf & (unsigned)(unsigned char)Buffer[0]);
      if (ii <= 14) {
         TST_ASSERT_EQUALS(ii, ((0xf0 & (unsigned)(unsigned char)Buffer[0]) >> 4));
      } else {
         COLASSERT(ii >= 15);
         TST_ASSERT_EQUALS(0xf0, ((0xf0 & (unsigned)(unsigned char)Buffer[0])));
         if (ii <= 127) {
            TST_ASSERT_EQUALS(ii, (0x7f & (unsigned)(unsigned char)Buffer[1]));
         } else {
            COLASSERT(ii >= 128);
            TST_ASSERT_EQUALS(((unsigned)ii & 0x7f), (0x7f & (unsigned)(unsigned char)Buffer[1]));
            TST_ASSERT_EQUALS(((unsigned)(ii & 0x2f80) >> 7), (0x7f & (unsigned)(unsigned char)Buffer[2]));
         }
      }

      // parse the binary buffer and verify size and elements
      COLvar NewVar;
      COLvarBinaryStreamParser Parser;
      Parser.append(Buffer);
      COLvar::ParseState parseState = Parser.parse(&NewVar);
      TST_ASSERT_EQUALS(COLvar::kSUCCESS, parseState);
      TST_ASSERT_EQUALS(0, Parser.countOfUnparsedBytes());
      TST_ASSERT_EQUALS(COLvar::Array, NewVar.type());
      TST_ASSERT_EQUALS(ii, NewVar.size());
      for (int x = 1; x < ii; ++x) {
         TST_ASSERT_EQUALS(x * 10 + 10000, NewVar[x-1].asInt());
      }
   }

   // check map encoding for 0 to 300 pairs
   v.reset();
   v.setMapType();
   for (int ii = 0; ii <= 260; ++ii) {
      if (ii > 0) {
         v[COLstring("K") + COLvar(ii).asString()] = ii;
      }

      // convert map to binary
      Buffer.clear();
      v.toBinary(&Buffer);
      TST_ASSERT_EQUALS(COLvar::Map, 0xf & (unsigned)(unsigned char)Buffer[0]);
      if (ii <= 14) {
         TST_ASSERT_EQUALS(ii, ((0xf0 & (unsigned)(unsigned char)Buffer[0]) >> 4));
      } else {
         COLASSERT(ii >= 15);
         TST_ASSERT_EQUALS(0xf0, ((0xf0 & (unsigned)(unsigned char)Buffer[0])));
         if (ii <= 127) {
            TST_ASSERT_EQUALS(ii, (0x7f & (unsigned)(unsigned char)Buffer[1]));
         } else {
            COLASSERT(ii >= 128);
            TST_ASSERT_EQUALS(((unsigned)ii & 0x7f), (0x7f & (unsigned)(unsigned char)Buffer[1]));
            TST_ASSERT_EQUALS(((unsigned)(ii & 0x2f80) >> 7), (0x7f & (unsigned)(unsigned char)Buffer[2]));
         }
      }

      // parse the binary buffer and verify size and map pairs
      COLvar NewVar;
      COLvarBinaryStreamParser Parser;
      Parser.append(Buffer);
      COLvar::ParseState parseState = Parser.parse(&NewVar);
      TST_ASSERT_EQUALS(COLvar::kSUCCESS, parseState);
      TST_ASSERT_EQUALS(0, Parser.countOfUnparsedBytes());
      TST_ASSERT_EQUALS(COLvar::Map, NewVar.type());
      TST_ASSERT_EQUALS(ii, NewVar.size());
      for (int x = 1; x < ii; ++x) {
         TST_ASSERT_EQUALS(x, NewVar[COLstring("K") + COLvar(x).asString()].asInt());
      }
   }
}

static void testBinaryStreamParse()
{
   COL_FUNCTION(testBinaryStreamParse);

   COLvarBinaryStreamParser Parser;
   TST_ASSERT_EQUALS(false, Parser.shouldParse());

   COLstring BinaryBuf;
   COLvar Var;

   //
   // test streaming 1 good COLvar binary message but in 2 fragments.
   //

   COLvar OriginalVar1;
   OriginalVar1["cat"] = "dog";
   BinaryBuf.clear();
   OriginalVar1.toBinary(&BinaryBuf);
   COL_DUMP("testBinaryStreamParse first message", BinaryBuf.data(), BinaryBuf.size());

   // output an incomplete fragment of the binary message

   const int firstFragmentSize = 2;
   Parser.append(BinaryBuf.data(), firstFragmentSize);
   TST_ASSERT_EQUALS(true, Parser.shouldParse());

   // try to parse the message fragment - expect to need more data
   COLvar::ParseState rc = Parser.parse(&Var);
   TST_ASSERT_EQUALS(COLvar::kNEED_MORE_DATA, rc);
   TST_ASSERT(Parser.countOfUnparsedBytes() > 0);

   // output the rest of the binary message

   Parser.append(BinaryBuf.data() + firstFragmentSize,
                 BinaryBuf.size() - firstFragmentSize);
   TST_ASSERT_EQUALS(true, Parser.shouldParse());

   // parse the complete message
   TST_ASSERT_EQUALS(COLvar::kSUCCESS, Parser.parse(&Var));
   TST_ASSERT_EQUALS("{\"cat\":\"dog\"}", Var.json());
   TST_ASSERT_EQUALS(0, Parser.countOfUnparsedBytes());

   //
   // Test streaming 2 good COLvar binary messages followed by some garbage bytes.
   // The subsequent parse will then try to parse that entire stream of messages.
   //

   COLvar OriginalVar2;
   int i = 0;
   OriginalVar2["horse"][i++] = -1234;
   OriginalVar2["horse"][i++] = 12345678;
   OriginalVar2["horse"][i++] = 23.5;
   OriginalVar2["horse"][i++] = COLvar();
   OriginalVar2["horse"][i++] = true;
   OriginalVar2["horse"][i++] = false;
   OriginalVar2["horse"][i++] = "A value.";
   OriginalVar2["horse"][i++].setMapType();
   OriginalVar2["horse"][i++].setArrayType();
   int a = 0;
   for (a = -2; a <= 20; ++a) {
      if (a%3 == 0)
         OriginalVar2["horse"][i++] = a;
      else
         OriginalVar2["horse"][i++] = (double)a;
   }
   for (a = 126; a <= 130; ++a) {
      if (a%3 == 0)
         OriginalVar2["horse"][i++] = a;
      else
         OriginalVar2["horse"][i++] = (double)a;
   }
   BinaryBuf.clear();
   OriginalVar2.toBinary(&BinaryBuf);
   Parser.append(BinaryBuf);
   TST_ASSERT_EQUALS(true, Parser.shouldParse());
   TST_ASSERT(Parser.countOfUnparsedBytes() > 0);

   COLvar OriginalVar3;
   OriginalVar3["squirrel"] = 654.125;
   BinaryBuf.clear();
   OriginalVar3.toBinary(&BinaryBuf);
   Parser.append(BinaryBuf);
   TST_ASSERT_EQUALS(true, Parser.shouldParse());
   TST_ASSERT(Parser.countOfUnparsedBytes() > 0);

   // append some garbage bytes after the messages
   const int TrailingGarbageByteCount = 3;
   BinaryBuf = COLstring("\xff\xee\xdd",TrailingGarbageByteCount);
   Parser.append(BinaryBuf);
   TST_ASSERT(Parser.countOfUnparsedBytes() > 0);
   TST_ASSERT_EQUALS(true, Parser.shouldParse());
   COL_DUMP("testBinaryStreamParse last 2 messages and trailing garbage", Parser.peekUnparsed(), Parser.countOfUnparsedBytes());

   i = 1;
   while (Parser.shouldParse()) {
      const COLvar::ParseState state = Parser.parse(&Var);
      switch (i) {
      case 1:
         TST_ASSERT_EQUALS(COLvar::kSUCCESS, state);
         TST_ASSERT_EQUALS("{\"horse\":[-1234,12345678,23.5,null,true,false,\"A value.\",{},[],-2,-1,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,126,127,128,129,130]}", Var.json());
         TST_ASSERT(Parser.countOfUnparsedBytes() > 0);
         TST_ASSERT_EQUALS(true, Parser.shouldParse());
         break;
      case 2:
         TST_ASSERT_EQUALS(COLvar::kSUCCESS, state);
         TST_ASSERT_EQUALS("{\"squirrel\":654.125}", Var.json());
         TST_ASSERT_EQUALS(TrailingGarbageByteCount, Parser.countOfUnparsedBytes());
         TST_ASSERT_EQUALS(true, Parser.shouldParse());
         break;
      case 3:
         TST_ASSERT_EQUALS(COLvar::kFAILURE, state);
         TST_ASSERT_EQUALS("null", Var.json());
         TST_ASSERT_EQUALS(TrailingGarbageByteCount, Parser.countOfUnparsedBytes());
         TST_ASSERT_EQUALS(false, Parser.shouldParse());
         break;
      }
      if (state == COLvar::kSUCCESS) {
         COL_VAR2(i, Var.json());
      }
      ++i;
   }
   TST_ASSERT_EQUALS(4, i);
}

static void testKeyOrdering()
{
   COL_FUNCTION(testKeyOrdering);

   TST_ASSERT_EQUALS(0, COLstring("a").icompare(COLstring("a")));
   TST_ASSERT_EQUALS(0, COLstring("A").icompare(COLstring("a")));
   TST_ASSERT_EQUALS(0, COLstring("a").icompare(COLstring("A")));
   TST_ASSERT_EQUALS(0, COLstring("aa").icompare(COLstring("AA")));
   TST_ASSERT_EQUALS(-1, COLstring("a").icompare(COLstring("b")));
   TST_ASSERT_EQUALS(1, COLstring("b").icompare(COLstring("a")));
   TST_ASSERT_EQUALS(-1, COLstring("A").icompare(COLstring("b")));
   TST_ASSERT_EQUALS(1, COLstring("B").icompare(COLstring("a")));
   TST_ASSERT_EQUALS(-1, COLstring("a").icompare(COLstring("B")));
   TST_ASSERT_EQUALS(1, COLstring("b").icompare(COLstring("A")));

   COLvar v1;
   v1["AA"] = 60;
   v1["B"] = 4;
   v1["aA"] = 3;
   v1["b"] = 40;
   v1["Aa"] = 5;
   v1["A"] = 1;
   v1["aa"] = 6;
   v1["a"] = 2;

   const COLstring expected_json = "{\"A\":1,\"a\":2,\"AA\":60,\"Aa\":5,\"aA\":3,\"aa\":6,\"B\":4,\"b\":40}";

   TST_ASSERT_EQUALS(expected_json, v1.json());
   COL_VAR2(expected_json, v1.json());

   // test copy ctor of COLvar map
   COLvar v2 = v1;
   TST_ASSERT_EQUALS(expected_json, v2.json());

   // test assignment operator of COLvar map
   COLvar v3;
   v3 = v2;
   TST_ASSERT_EQUALS(expected_json, v3.json());
}

static void testForwardFillingArrayStride1000()
{
   COL_FUNCTION(testForwardFillingArrayStride1000);
   COLvar var;
   for (int i = 0; i <= 250000; i += 1000) {
      var[i] = 4*i;
   }
   TST_ASSERT_EQUALS(true,     var.isArray());
   TST_ASSERT_EQUALS(false,    var.isMap());
   TST_ASSERT_EQUALS(250000+1, var.size());
}

static void testForwardFillingArrayStride10000()
{
   COL_FUNCTION(testForwardFillingArrayStride10000);
   COLvar var;
   for (int i = 0; i <= 250000; i += 10000) {
      var[i] = 4*i;
   }
   TST_ASSERT_EQUALS(false, var.isArray());
   TST_ASSERT_EQUALS(true,  var.isMap());
   TST_ASSERT_EQUALS(25+1,  var.size());
}

static void testForwardFillingArrayStride100000()
{
   COL_FUNCTION(testForwardFillingArrayStride100000);
   COLvar var;
   for (int i = 0; i <= 250000; i += 100000) {
      var[i] = 4*i;
   }
   TST_ASSERT_EQUALS(false, var.isArray());
   TST_ASSERT_EQUALS(true,  var.isMap());
   TST_ASSERT_EQUALS("{\"0\":0,\"100000\":400000,\"200000\":800000}", var.json());
}

static void testArrayIndexesNearZero()
{
   COL_FUNCTION(testArrayIndexesNearZero);
   {
      COLvar var;
      for (int i = 9; i >= 3; --i) {
         var[i] = 10*i;
      }
      TST_ASSERT_EQUALS("[null,null,null,30,40,50,60,70,80,90]", var.json());
   }
   {
      COLvar var;
      for (int i = 3; i >= 0; --i) {
         var[i] = 10*i;
      }
      TST_ASSERT_EQUALS("[0,10,20,30]", var.json());
   }
   {
      COLvar var;
      for (int i = 3; i >= -1; --i) {
         var[i] = 10*i;
      }
      TST_ASSERT_EQUALS("{\"-1\":-10,\"0\":0,\"1\":10,\"2\":20,\"3\":30}", var.json());
   }
   {
      COLvar var;
      for (int i = 3; i >= -3; --i) {
         var[i] = 10*i;
      }
      TST_ASSERT_EQUALS("{\"-1\":-10,\"-2\":-20,\"-3\":-30,\"0\":0,\"1\":10,\"2\":20,\"3\":30}", var.json());
   }
   {
      COLvar var;
      for (int i = -3; i <= 2; ++i) {
         var[i] = 10*i;
      }
      TST_ASSERT_EQUALS("{\"-1\":-10,\"-2\":-20,\"-3\":-30,\"0\":0,\"1\":10,\"2\":20}", var.json());
   }
}

static COLstring some_function(double d, int i, const COLstring& s) {
   COLostreamString Stream;
   Stream << "some_function: d = " << d << ", i = " << i << ", s = " << s;
   return Stream.str();
}

static void testConversionOperators()
{
   COLvar v;
   TST_ASSERT_EQUALS("", v);
   TST_ASSERT_EQUALS(int(0), int(v));
   TST_ASSERT_EQUALS(0.0, double(v));
   TST_ASSERT_EQUALS(false, bool(v));

   v = "123.456";
   TST_ASSERT_EQUALS("123.456", v);
   TST_ASSERT_EQUALS(123.456, double(v));
   TST_ASSERT_EQUALS(123, int(v));
   TST_ASSERT_EQUALS(true, bool(v));

   const COLstring s = v;
   TST_ASSERT_EQUALS("123.456", s);

   const COLstring& sr = v;
   TST_ASSERT_EQUALS("123.456", sr);

   int i = v;
   TST_ASSERT_EQUALS(123, i);

   double d = v;
   TST_ASSERT_EQUALS(123.456, d);

   // Compile error: ambiguous. This is correct. Requires an explicit cast.
   // long Long = v;
   // TST_ASSERT_EQUALS(123, Long);

   // Compile error: ambiguous. This is correct. Requires an explicit cast.
   // unsigned u = v;
   // TST_ASSERT_EQUALS(123, u);

   // test conversion operators as arguments to a function
   TST_ASSERT_EQUALS("some_function: d = 123.456000, i = 123, s = 123.456", some_function(v,v,v));
}

//
// tests
//
//   COLvar*       COLvar::get(int idx);
//   const COLvar* COLvar::get(int idx) const;
//   bool          COLvar::exists(int idx) const;
//
static void testGetIndex()
{
   COL_FUNCTION(testGetIndex);
   COLvar var;
   const COLvar& cvar = var;

   TST_ASSERT(0 == var.get(4444));
   TST_ASSERT(0 == cvar.get(4444));

   TST_ASSERT(0 == var.get(5));
   TST_ASSERT(0 == cvar.get(5));

   var[5] = "foo";
   var[12] = 27000;

   TST_ASSERT(var.get(5) != 0);
   TST_ASSERT(cvar.get(5) != 0);

   TST_ASSERT_EQUALS(&var[5], var.get(5));
   TST_ASSERT_EQUALS(&cvar[5], cvar.get(5));

   TST_ASSERT_EQUALS("foo", var.get(5)->asString());
   TST_ASSERT_EQUALS("foo", cvar.get(5)->asString());

   TST_ASSERT_EQUALS(27000, var.get(12)->asInt());
   TST_ASSERT_EQUALS(27000, cvar.get(12)->asInt());

   // verify that these array elements were not created
   TST_ASSERT(0 == var.get(4444));
   TST_ASSERT(0 == cvar.get(4444));

   TST_ASSERT_EQUALS(true,  var.exists(5));
   TST_ASSERT_EQUALS(true, cvar.exists(5));

   TST_ASSERT_EQUALS(false,  var.exists(4444));
   TST_ASSERT_EQUALS(false, cvar.exists(4444));
}

//
// tests
//
//   COLvar*       COLvar::get(const COLstring& key);
//   const COLvar* COLvar::get(const COLstring& key) const;
//   bool          COLvar::exists(const COLstring& key) const;
//
static void testGetKey()
{
   COL_FUNCTION(testGetKey);
   COLvar var;
   const COLvar& cvar = var;

   TST_ASSERT(0 == var.get("fred"));
   TST_ASSERT(0 == cvar.get("fred"));

   TST_ASSERT(0 == var.get(5));
   TST_ASSERT(0 == cvar.get(5));

   var["bob"] = "dog";
   var["doug"] = 1947;

   TST_ASSERT(var.get("bob") != 0);
   TST_ASSERT(cvar.get("bob") != 0);

   TST_ASSERT_EQUALS(&var["bob"], var.get("bob"));
   TST_ASSERT_EQUALS(&cvar["bob"], cvar.get("bob"));

   TST_ASSERT_EQUALS("dog", var.get("bob")->asString());
   TST_ASSERT_EQUALS("dog", cvar.get("bob")->asString());

   TST_ASSERT_EQUALS(1947, var.get("doug")->asInt());
   TST_ASSERT_EQUALS(1947, cvar.get("doug")->asInt());

   // verify that these map elements were not created
   TST_ASSERT(0 == var.get("fred"));
   TST_ASSERT(0 == cvar.get("fred"));

   TST_ASSERT_EQUALS(true,  var.exists("bob"));
   TST_ASSERT_EQUALS(true, cvar.exists("bob"));

   TST_ASSERT_EQUALS(false,  var.exists("fred"));
   TST_ASSERT_EQUALS(false, cvar.exists("fred"));
}

static void testSwap()
{
   COL_FUNCTION(testSwap);
   const char* cs = "some long string to test swap";
   COLstring s;
   COLvar var;

   TST_ASSERT_EQUALS(COLvar::None, var.type());
   s = cs;
   var.swap(&s);
   TST_ASSERT_EQUALS(cs, var.str());
   TST_ASSERT_EQUALS("", s);

   var = 123;
   TST_ASSERT_EQUALS(COLvar::Integer, var.type());
   s = cs;
   var.swap(&s);
   TST_ASSERT_EQUALS(cs, var.str());
   TST_ASSERT_EQUALS("", s);

   var = -765.432;
   TST_ASSERT_EQUALS(COLvar::Double, var.type());
   s = cs;
   var.swap(&s);
   TST_ASSERT_EQUALS(cs, var.str());
   TST_ASSERT_EQUALS("", s);

   var = "Some other long string to prime the COLvar";
   TST_ASSERT_EQUALS(COLvar::String, var.type());
   s = "A long string to test swap.";
   var.swap(&s);
   TST_ASSERT_EQUALS("A long string to test swap.", var.str());
   TST_ASSERT_EQUALS("Some other long string to prime the COLvar", s);

   var.setArrayType();
   var.push_back(22000);
   TST_ASSERT_EQUALS(COLvar::Array, var.type());
   s = cs;
   var.swap(&s);
   TST_ASSERT_EQUALS(cs, var.str());
   TST_ASSERT_EQUALS("", s);

   var["foo"] = "bar";
   TST_ASSERT_EQUALS(COLvar::Map, var.type());
   s = cs;
   var.swap(&s);
   TST_ASSERT_EQUALS(cs, var.str());
   TST_ASSERT_EQUALS("", s);

   var = false;
   TST_ASSERT_EQUALS(COLvar::Boolean, var.type());
   s = cs;
   var.swap(&s);
   TST_ASSERT_EQUALS(cs, var.str());
   TST_ASSERT_EQUALS("", s);

   var = true;
   TST_ASSERT_EQUALS(COLvar::Boolean, var.type());
   s = cs;
   var.swap(&s);
   TST_ASSERT_EQUALS(cs, var.str());
   TST_ASSERT_EQUALS("", s);
}

static void testFastCompare()
{
   COL_FUNCTION(testFastCompare);
   COLvar arrA, arrB, arrC, arrD;
   COLvar mapA, mapB, mapC, mapD, mapE;
   arrA.push_back()=1  ; arrA.push_back()="A";
   arrB.push_back()=1  ; arrB.push_back()="A";
   arrC.push_back()=1  ; arrC.push_back()="X";
   arrD.push_back()="X";
   mapA["A"] = 1; mapA["B"] = "B";
   mapB["A"] = 1; mapB["B"] = "B";
   mapC["A"] = 2; mapC["B"] = "B";
   mapD["A"] = "B";
   mapE["B"] = "B";

   TST_ASSERT(COLvar::fastCompare(mapA         ,mapA         )==0); // same map
   TST_ASSERT(COLvar::fastCompare(arrA         ,arrA         )==0); // same array
   TST_ASSERT(COLvar::fastCompare(COLvar(     ),COLvar(false))< 0); // diff type
   TST_ASSERT(COLvar::fastCompare(COLvar(false),COLvar(     ))> 0); // diff type
   TST_ASSERT(COLvar::fastCompare(arrA         ,arrD         )> 0); // diff size array
   TST_ASSERT(COLvar::fastCompare(arrD         ,arrA         )< 0); // diff size array
   TST_ASSERT(COLvar::fastCompare(mapA         ,mapD         )> 0); // diff size map
   TST_ASSERT(COLvar::fastCompare(mapD         ,mapA         )< 0); // diff size map
   TST_ASSERT(COLvar::fastCompare(mapD         ,mapE         )< 0); // diff map key
   TST_ASSERT(COLvar::fastCompare(mapE         ,mapD         )> 0); // diff map key
   TST_ASSERT(COLvar::fastCompare(COLvar(     ),COLvar(     ))==0); // equals none
   TST_ASSERT(COLvar::fastCompare(COLvar(false),COLvar(false))==0); // equals bool
   TST_ASSERT(COLvar::fastCompare(COLvar(12345),COLvar(12345))==0); // equals int
   TST_ASSERT(COLvar::fastCompare(COLvar(-0.35),COLvar(-0.35))==0); // equals double
   TST_ASSERT(COLvar::fastCompare(COLvar("abc"),COLvar("abc"))==0); // equals string
   TST_ASSERT(COLvar::fastCompare(arrA         ,arrB         )==0); // equals array
   TST_ASSERT(COLvar::fastCompare(mapA         ,mapB         )==0); // equals map
   TST_ASSERT(COLvar::fastCompare(COLvar(false),COLvar(true ))< 0); // less bool
   TST_ASSERT(COLvar::fastCompare(COLvar(10000),COLvar(12345))< 0); // less int
   TST_ASSERT(COLvar::fastCompare(COLvar(-0.45),COLvar(-0.35))< 0); // less double
   TST_ASSERT(COLvar::fastCompare(COLvar("aaa"),COLvar("abc"))< 0); // less string
   TST_ASSERT(COLvar::fastCompare(arrA         ,arrC         )< 0); // less array
   TST_ASSERT(COLvar::fastCompare(mapA         ,mapC         )< 0); // less map
   TST_ASSERT(COLvar::fastCompare(COLvar(true ),COLvar(false))> 0); // greater bool
   TST_ASSERT(COLvar::fastCompare(COLvar(12345),COLvar(10000))> 0); // greater int
   TST_ASSERT(COLvar::fastCompare(COLvar(-0.35),COLvar(-0.45))> 0); // greater double
   TST_ASSERT(COLvar::fastCompare(COLvar("abc"),COLvar("aaa"))> 0); // greater string
   TST_ASSERT(COLvar::fastCompare(arrC         ,arrA         )> 0); // greater array
   TST_ASSERT(COLvar::fastCompare(mapC         ,mapA         )> 0); // greater map

   // binary String fastCompare tests
   TST_ASSERT(COLvar::fastCompare(COLvar("",0), COLvar("",0)) == 0);     // zero len EQ
   TST_ASSERT(COLvar::fastCompare(COLvar("AB",2), COLvar("AB",2)) == 0); // same len EQ
   TST_ASSERT(COLvar::fastCompare(COLvar("Z"), COLvar("AB")) < 0);       // diff len LT
   TST_ASSERT(COLvar::fastCompare(COLvar("AB"),COLvar("Z"))  > 0);       // diff len GT
   TST_ASSERT(COLvar::fastCompare(COLvar("A"), COLvar("ZZ")) < 0);       // diff len LT
   TST_ASSERT(COLvar::fastCompare(COLvar("ZZ"),COLvar("A"))  > 0);       // diff len GT
   TST_ASSERT(COLvar::fastCompare(COLvar("cat",3),COLvar("dog",3)) < 0); // same len LT
   TST_ASSERT(COLvar::fastCompare(COLvar("\000a",2), COLvar("\000b",2)) < 0);  // same len LT
   TST_ASSERT(COLvar::fastCompare(COLvar("\000Z",2), COLvar("\000A",2)) > 0);  // same len GT
   TST_ASSERT(COLvar::fastCompare(COLvar("\000Z",2), COLvar("\000Z",2)) == 0); // same len EQ

   COLvar null1,            null2;
   COLvar true1(true),      true2(true);
   COLvar false1(false),    false2(false);
   COLvar int1(87),         int2(87);
   COLvar double1(-321.45), double2(-321.45);
   COLvar s1("ok"),         s2("ok");

   // operator== positive tests
   TST_ASSERT(null1          == null1);          // same None object
   TST_ASSERT(true1          == true1);          // same Boolean true object
   TST_ASSERT(false1         == false1);         // same Boolean false object
   TST_ASSERT(int1           == int1);           // same Integer object
   TST_ASSERT(double1        == double1);        // same Double object
   TST_ASSERT(s1             == s1);             // same String object
   TST_ASSERT(mapA           == mapA);           // same Map object
   TST_ASSERT(arrA           == arrA);           // same Array object
   TST_ASSERT(null1          == null2);          // same None contents
   TST_ASSERT(true1          == true2);          // same Boolean contents
   TST_ASSERT(false1         == false2);         // same Boolean contents
   TST_ASSERT(int1           == int2);           // same Integer contents
   TST_ASSERT(double1        == double2);        // same Double contents
   TST_ASSERT(s1             == s2);             // same String contents
   TST_ASSERT(arrA           == arrB);           // same Array contents
   TST_ASSERT(mapA           == mapB);           // same Map contents
   TST_ASSERT(""             == COLvar());       // test null COLvar and implicit operator COLstring()
   TST_ASSERT(COLvar()       == COLvar());       // same None contents
   TST_ASSERT(COLvar(true)   == COLvar(true));   // same Boolean contents
   TST_ASSERT(COLvar(false)  == COLvar(false));  // same Boolean contents
   TST_ASSERT(COLvar(123)    == COLvar(123));    // same Integer contents
   TST_ASSERT(COLvar(-123.4) == COLvar(-123.4)); // same Double contents
   TST_ASSERT(COLvar("fred") == COLvar("fred")); // same String contents
   TST_ASSERT(COLvar(-1.23)  == COLvar(-1.23));  // same Double contents

   // operator== negative tests
   TST_ASSERT_EQUALS(false, (COLvar("ok")  == COLvar("ab")));
   TST_ASSERT_EQUALS(false, (COLvar(123)   == COLvar(23)));
   TST_ASSERT_EQUALS(false, (COLvar()      == COLvar(1)));
   TST_ASSERT_EQUALS(false, (COLvar(true)  == COLvar(false)));
   TST_ASSERT_EQUALS(false, (COLvar(false) == COLvar(true)));
   TST_ASSERT_EQUALS(false, ("7"           == COLvar(8)));
   TST_ASSERT_EQUALS(false, (mapA          == mapD));
   TST_ASSERT_EQUALS(false, (arrA          == arrD));

   // operator!= positive tests
   TST_ASSERT(arrA           != arrD);           // different Array contents
   TST_ASSERT(mapA           != mapD);           // different Map contents
   TST_ASSERT(arrD           != arrA);           // different Array contents
   TST_ASSERT(mapD           != mapA);           // different Map contents
   TST_ASSERT(COLvar(123)    != COLvar(23));     // different Integer
   TST_ASSERT(COLvar(-123.4) != COLvar(23));     // different Integer
   TST_ASSERT(COLvar()       != COLvar(7));      // different None
   TST_ASSERT(COLvar("fred") != COLvar("red"));  // different String
   TST_ASSERT(COLvar(-1.23)  != COLvar(-0.23));  // different Double
   TST_ASSERT(COLvar(true)   != COLvar(false));  // different Boolean
   TST_ASSERT(COLvar(false)  != COLvar(true));   // different Boolean

   // operator!= negative tests
   TST_ASSERT_EQUALS(false, (null1   != null1));   // same None object
   TST_ASSERT_EQUALS(false, (true1   != true1));   // same Boolean true object
   TST_ASSERT_EQUALS(false, (false1  != false1));  // same Boolean false object
   TST_ASSERT_EQUALS(false, (int1    != int1));    // same Integer object
   TST_ASSERT_EQUALS(false, (double1 != double1)); // same Double object
   TST_ASSERT_EQUALS(false, (s1      != s1));      // same String object
   TST_ASSERT_EQUALS(false, (null1   != null2));   // same None contents
   TST_ASSERT_EQUALS(false, (true1   != true2));   // same Boolean contents
   TST_ASSERT_EQUALS(false, (false1  != false2));  // same Boolean contents
   TST_ASSERT_EQUALS(false, (int1    != int2));    // same Integer contents
   TST_ASSERT_EQUALS(false, (double1 != double2)); // same Double contents
   TST_ASSERT_EQUALS(false, (s1      != s2));      // same String contents
   TST_ASSERT_EQUALS(false, (mapA    != mapB));    // same Map contents
   TST_ASSERT_EQUALS(false, (arrA    != arrA));    // same Array contents
   TST_ASSERT_EQUALS(false, (COLvar("ok")  != COLvar("ok")));
   TST_ASSERT_EQUALS(false, (COLvar(123)   != COLvar(123)));
   TST_ASSERT_EQUALS(false, (COLvar()      != COLvar()));
   TST_ASSERT_EQUALS(false, (COLvar(true)  != COLvar(true)));
   TST_ASSERT_EQUALS(false, (COLvar(false) != COLvar(false)));
   TST_ASSERT_EQUALS(false, (""            != COLvar()));
   TST_ASSERT_EQUALS(false, ("7"           != COLvar(7)));

   // test binary string equality and inequality
   TST_ASSERT(COLvar("\000y",2)              != COLvar("\000n",2));
   TST_ASSERT(COLvar("\000Hi\000\001\xFF",6) == COLvar("\000Hi\000\001\xFF",6));
   TST_ASSERT(COLvar("\000Hi\000\001\xFF",6) != COLvar("\000Hi\000\001\xFE",6));
   TST_ASSERT(COLvar("\000Hi\000\001\xFE",6) != COLvar("\000Hi\000\001\xFF",6));
   TST_ASSERT(COLvar("ok\000y",4)            != COLvar("ok\000n",4));
   TST_ASSERT( false == (COLvar("ok\000y",4) == COLvar("ok\000n",4)) );
   TST_ASSERT( false == (COLvar("ok\000y",4) != COLvar("ok\000y",4)) );
}

static void testEscapeChar() {
   COLvar Var;
   Var["\""] = "\\\"";
   Var["\\"] = "\\\\";
   Var["/"]  = "\\/";
   Var["\b"] = "\\b";
   Var["\f"] = "\\f";
   Var["\n"] = "\\n";
   Var["\r"] = "\\r";
   Var["\t"] = "\\t";
   Var["\r\n"] = "testesg";
   // write to file
   const COLstring VarFile = "tempFile";
   FILwriteFile(VarFile, Var.json());
   // read from file
   COLstring Content;
   FILreadFile(VarFile, &Content);
   COLvar Json;
   Json.parse(Content);
   TST_ASSERT_EQUALS(Var.json(), Json.json());

   // COLstring s = "foo=bar&isUnitTest=true&usingPort=61487";
   // COLvar test;

   // TST_ASSERT_EQUALS(false, test.parse(s));
   // COL_VAR(test);
}

TSTtestable* COLvarTests()
{
   TST_START_COLLECTION("COLvarTests")
      TST_ADD_TEST_FUNCTION(testSimple);
      TST_ADD_TEST_FUNCTION(testBoolean);
      TST_ADD_TEST_FUNCTION(testIntCtor);
      TST_ADD_TEST_FUNCTION(testLongLongCtor);
      TST_ADD_TEST_FUNCTION(testDoubleCtor);
      TST_ADD_TEST_FUNCTION(testStringCtor);
      TST_ADD_TEST_FUNCTION(testStringAppend);
      TST_ADD_TEST_FUNCTION(testArrayAccess);
      TST_ADD_TEST_FUNCTION(testForwardFillingArrayStride1000);
      TST_ADD_TEST_FUNCTION(testForwardFillingArrayStride10000);
      TST_ADD_TEST_FUNCTION(testForwardFillingArrayStride100000);
      TST_ADD_TEST_FUNCTION(testArrayIndexesNearZero);
      TST_ADD_TEST_FUNCTION(testMapAccess);
      TST_ADD_TEST_FUNCTION(testMapErase);
      TST_ADD_TEST_FUNCTION(testMixedArrayAndMapAccess);
     // TST_ADD_TEST_FUNCTION(testCollectionExceptions);
      TST_ADD_TEST_FUNCTION(testArrayPushBack);
      TST_ADD_TEST_FUNCTION(testEscape);
      TST_ADD_TEST_FUNCTION(testBinary);
      TST_ADD_TEST_FUNCTION(testBinaryStreamParse);
      TST_ADD_TEST_FUNCTION(testKeyOrdering);
      TST_ADD_TEST_FUNCTION(testAssignment);
      TST_ADD_TEST_FUNCTION(testConversionOperators);
      TST_ADD_TEST_FUNCTION(testGetIndex);
      TST_ADD_TEST_FUNCTION(testGetKey);
      TST_ADD_TEST_FUNCTION(testSwap);
      TST_ADD_TEST_FUNCTION(testFastCompare);
      TST_ADD_TEST_FUNCTION(testEscapeChar);

   TST_END_FIXTURE
}
