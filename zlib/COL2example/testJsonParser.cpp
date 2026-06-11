//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: testJsonParser
//
//  Description: Implementation
//
//  Author: Vismay Shah
//  Date:   Tuesday October 17 2023 6:18pm
//  ---------------------------------------------------------------------------
#include <COL2example/testJsonParser.h>

#include <UNIT/UNITapp.h>

#include <FIL/FILutils.h>

#include <COL/COLvarJson.h>
#include <COL/COLstring.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

// Helper function to test JSON string parsing consistency.
static void testJson(const COLstring& jsonString, const COLstring& expectedJson) {
    COL_FUNCTION(testJson);
    COLvar V;
    bool success = V.parse(jsonString);
    COL_VAR(V.json());
    UNIT_ASSERT_EQUALS(true, success);  // Assert both parse actions were successful.
    UNIT_ASSERT_EQUALS(expectedJson, V.json());  // Compare against expected JSON output.
}

static void testJsonEmpty() {
   COL_FUNCTION(testJsonEmpty);
   testJson("\"\"", "\"\"");
   testJson("\'\'", "\"\"");
}

// See IX-3769 for more information on these unicode tests
static void testPrintableUnicodeChar(){
   COL_FUNCTION(testPrintableUnicodeChar);
   testJson("'Womens\\u0027s Hospital'", "\"Womens's Hospital\"");
   testJson("\"Foo\\u0009 Bar\"", "\"Foo\\t Bar\"");
   testJson("\"Foo\\u0008 Bar\"", "\"Foo\\b Bar\"");
   testJson("\"Foo\\u0004 Bar\"", "\"Foo\\u0004 Bar\"");
   //TODO Failing these tests
   //testJson("\"Foo\\u115f Bar\"", "\"Foo\\u115f Bar\""); // Korean char
   //testJson("\"Foo\\u00a0 Bar\"", "\"Foo\\u00a0 Bar\""); // nbsp
}

static void testSimpleString()       {COL_FUNCTION(testSimpleString);       testJson("  \"Hello World\"", "\"Hello World\"");   }
static void testEscapedString()      {COL_FUNCTION(testEscapedString);      testJson("\"Hello\\nWorld\"", "\"Hello\\nWorld\""); }
static void testSingleQuotedString() {COL_FUNCTION(testSingleQuotedString); testJson("'Hello World'",     "\"Hello World\"");   }

static void testUnicodeString()      {COL_FUNCTION(testUnicodeString);      testJson("{\"asd\" : \"Foo\\u115f Bar\"}", "{\"asd\" : \"Foo\\u115f Bar\"}"); }
static void testNumber()             {COL_FUNCTION(testNumber);             testJson("232323", "232323"); }
static void testDouble()             {COL_FUNCTION(testDouble);             testJson("2323.23", "2323.23"); }
static void testNegative()           {COL_FUNCTION(testNegative);           testJson("-2.3", "-2.3"); }
static void testArray()              {COL_FUNCTION(testArray);              testJson("[ 1,  2, 3,  \"Fred\" ]", "[1,2,3,\"Fred\"]"); }
static void testBooleanTrue()        {COL_FUNCTION(testBooleanTrue);        testJson("true", "true"); }
static void testBooleanFalse()       {COL_FUNCTION(testBooleanFalse);       testJson("false", "false"); }
static void testNull()               {COL_FUNCTION(testNull);               testJson("null", "null"); }
static void testObject()             {COL_FUNCTION(testObject);             testJson("{ 'Fred' : 1212, \"Mary\" : \"life\"  }", "{\"Fred\":1212,\"Mary\":\"life\"}"); }
static void testEmptyObject()        {COL_FUNCTION(testEmptyObject);        testJson("{   }", "{}"); }
static void testEmptyArray()         {COL_FUNCTION(testEmptyArray);         testJson("[  ] ", "[]"); }

static void testUnicode(){
   COL_FUNCTION(testUnicode);
   testJson("\"Women\\u0027s Hospital\"", "\"Women's Hospital\"");
}

static void checkError(const COLstring& Input, const COLstring& Expected){
   COL_FUNCTION(checkError);
   COL_VAR2(Input, Expected);
   COLvar J;
   bool Success = COLparseJson(Input, &J);
   COL_VAR(J.json(1));
   UNIT_ASSERT_EQUALS(false, Success);
   COLstring Error = J.asString();
   COL_VAR(Error);
   int Pos = Error.find("Position");
   Error = Error.substr(0, Pos-1);
   COL_VAR(Error);
   UNIT_ASSERT_EQUALS(Expected, Error);  // Compare against expected JSON output.
}

void testJsonError(){
   COL_FUNCTION(testJsonError);
   checkError("", "Error parsing JSON: Empty string");
   //checkError("  ", "\"Error parsing JSON: Empty string\"");
   checkError("[",                   "Error parsing JSON: Unable to find closing ]");
   checkError("'Mismatched quote\"", "Error parsing JSON: Missing closing quote");
   checkError("{'Key\":\"value1\", \'foo\":[1,2,3]}","Error parsing JSON: Missing colon after key");
   checkError("[{ 'A' : 'ff'}, { 'B', : 'gg' },],",  "Error parsing JSON: Missing colon after key");
}

static void testLargePipedriveJson(){
   COL_FUNCTION(testLargePipedriveJson);
   COLstring Json;
   FILreadFile("testJsonParser/large_pipedrive.json", &Json);
   COLvar D;
   D.parse(Json);
   COL_DBG(D.json(1));   
}

// Andrew Vajocski tests 

// Takes json and actual answer arguement to test specific whitespace cases
// Ex. if [ ] was passed into above function, error would be raised because
// the json parser deletes whitespaces that aren't apart of strings
static void testJsonHelper2(COLstring json, COLstring actual) {
   COLvar v;
   v.parse(json);
   UNIT_ASSERT_EQUALS(actual, v.json());
}

// Takes json and actual answer arguement to test specific whitespace cases
// Ex. if [ ] was passed into above function, error would be raised because
// the json parser deletes whitespaces that aren't apart of strings
static void testInvalidJsonHelper(COLstring json) {
   COL_FUNCTION(testInvalidJsonHelper);
   COLvar v;
   bool valid = v.parse(json);
   COL_VAR3(json, valid, v);
   UNIT_ASSERT_EQUALS(false, valid);
}

// Takes json and actual answer arguement to test specific whitespace cases
// Ex. if [ ] was passed into above function, error would be raised because
// the json parser deletes whitespaces that aren't apart of strings
static void testValidJsonHelper(COLstring json) {
   COL_FUNCTION(testValidJsonHelper);
   COLvar v;
   bool valid = v.parse(json);
   COL_VAR(json);
   COL_VAR(valid);
   UNIT_ASSERT_EQUALS(true, valid);
}

// Takes single argument for both parsing and the answer
static void testJsonHelper(COLstring json) {
   COLvar v;
   v.parse(json);
   UNIT_ASSERT_EQUALS(json, v.json());
}

static void testJson()
{
   COLvar v;
   UNIT_ASSERT_EQUALS("null", v.json());

   v.setArrayType();
   UNIT_ASSERT_EQUALS(0, v.size());
   UNIT_ASSERT_EQUALS(COLvar::Array, v.type());
   UNIT_ASSERT_EQUALS(true, v.isArray());
   UNIT_ASSERT_EQUALS("[]", v.json());

   {
      COLvar v_copy_1(v);
      UNIT_ASSERT_EQUALS(0,             v_copy_1.size());
      UNIT_ASSERT_EQUALS(COLvar::Array, v_copy_1.type());
      UNIT_ASSERT_EQUALS(true,          v_copy_1.isArray());
      UNIT_ASSERT_EQUALS("[]",          v_copy_1.json());

      COLvar v_copy_2;
      v_copy_2 = v;
      UNIT_ASSERT_EQUALS(0,             v_copy_2.size());
      UNIT_ASSERT_EQUALS(COLvar::Array, v_copy_2.type());
      UNIT_ASSERT_EQUALS(true,          v_copy_2.isArray());
      UNIT_ASSERT_EQUALS("[]",          v_copy_2.json());
   }

   v.setMapType();
   UNIT_ASSERT_EQUALS(0, v.size());
   UNIT_ASSERT_EQUALS(COLvar::Map, v.type());
   UNIT_ASSERT_EQUALS("{}", v.json());

   {
      COLvar v_copy_1(v);
      UNIT_ASSERT_EQUALS(0,             v_copy_1.size());
      UNIT_ASSERT_EQUALS(COLvar::Map,   v_copy_1.type());
      UNIT_ASSERT_EQUALS(true,          v_copy_1.isMap());
      UNIT_ASSERT_EQUALS("{}",          v_copy_1.json());

      COLvar v_copy_2;
      v_copy_2 = v;
      UNIT_ASSERT_EQUALS(0,             v_copy_2.size());
      UNIT_ASSERT_EQUALS(COLvar::Map,   v_copy_2.type());
      UNIT_ASSERT_EQUALS(true,          v_copy_2.isMap());
      UNIT_ASSERT_EQUALS("{}",          v_copy_2.json());
   }

   COLvar                         empty_map_copy = v;
   UNIT_ASSERT_EQUALS(0,           empty_map_copy.size());
   UNIT_ASSERT_EQUALS(COLvar::Map, empty_map_copy.type());
   UNIT_ASSERT_EQUALS("{}",        empty_map_copy.json());

   v.setArrayType();
   UNIT_ASSERT_EQUALS(0, v.size());
   UNIT_ASSERT_EQUALS(COLvar::Array, v.type());
   UNIT_ASSERT_EQUALS("[]", v.json());

   COLvar                           empty_array_copy = v;
   UNIT_ASSERT_EQUALS(0,             empty_array_copy.size());
   UNIT_ASSERT_EQUALS(COLvar::Array, empty_array_copy.type());
   UNIT_ASSERT_EQUALS("[]",          empty_array_copy.json());

   v[3] = "Fred";
   UNIT_ASSERT_EQUALS(COLstring("[null,null,null,\"Fred\"]"), v.json());

   // test that const array lookups of not present attributes return const COLvar of type None
   UNIT_ASSERT_EQUALS(COLvar::Array, v.type());
   {
      const COLvar& cv0 = v;
      const COLvar& cvAttr = cv0["Attr Not Present"];
      UNIT_ASSERT_EQUALS(COLvar::None, cvAttr.type());
      UNIT_ASSERT_EQUALS(true,         cvAttr.isNone());
      UNIT_ASSERT_EQUALS("",           cvAttr);
      UNIT_ASSERT_EQUALS(false,   bool(cvAttr));
      UNIT_ASSERT_EQUALS(0,        int(cvAttr));
      UNIT_ASSERT_EQUALS(0.0,   double(cvAttr));
      UNIT_ASSERT_EQUALS(COLvar::None, cvAttr["attribute that is not present"]["blah"]["blah"].type());
   }
   {
      const COLvar& cv0 = v;
      const COLvar& cvAttr = cv0[-9876];
      UNIT_ASSERT_EQUALS(COLvar::None, cvAttr.type());
      UNIT_ASSERT_EQUALS(true,         cvAttr.isNone());
      UNIT_ASSERT_EQUALS("",           cvAttr);
      UNIT_ASSERT_EQUALS(false,   bool(cvAttr));
      UNIT_ASSERT_EQUALS(0,        int(cvAttr));
      UNIT_ASSERT_EQUALS(0.0,   double(cvAttr));
      UNIT_ASSERT_EQUALS(COLvar::None, cvAttr["attribute that is not present"]["blah"]["blah"].type());
   }

   v[1] = -234;
   UNIT_ASSERT_EQUALS("[null,-234,null,\"Fred\"]", v.json());

   v[0] = -567.89;
   UNIT_ASSERT_EQUALS("[-567.89,-234,null,\"Fred\"]", v.json());

   v[2]["foo"] = "bar";
   UNIT_ASSERT_EQUALS("[-567.89,-234,{\"foo\":\"bar\"},\"Fred\"]", v.json());

   // show that there are no cycles
   COLvar v2;
   v2["Attr"] = v;
   UNIT_ASSERT_EQUALS("{\"Attr\":[-567.89,-234,{\"foo\":\"bar\"},\"Fred\"]}", v2.json());
   UNIT_ASSERT_EQUALS("[-567.89,-234,{\"foo\":\"bar\"},\"Fred\"]", v.json());

   v = v2["Attr"];
   UNIT_ASSERT_EQUALS("[-567.89,-234,{\"foo\":\"bar\"},\"Fred\"]", v.json());
   UNIT_ASSERT_EQUALS("[-567.89,-234,{\"foo\":\"bar\"},\"Fred\"]", v2["Attr"].json());
   UNIT_ASSERT_EQUALS("{\"Attr\":[-567.89,-234,{\"foo\":\"bar\"},\"Fred\"]}", v2.json());

   // change v from an array to a map.
   v["1"] = "now\ta\nstring";
   UNIT_ASSERT_EQUALS("{\"0\":-567.89,\"1\":\"now\\ta\\nstring\",\"2\":{\"foo\":\"bar\"},\"3\":\"Fred\"}", v.json());
   UNIT_ASSERT_EQUALS("\"bar\"", v[2]["foo"].json());
   UNIT_ASSERT_EQUALS(true,  v.exists(0));
   UNIT_ASSERT_EQUALS(true,  v.exists("0"));
   UNIT_ASSERT_EQUALS(true,  v.exists(1));
   UNIT_ASSERT_EQUALS(true,  v.exists("1"));
   UNIT_ASSERT_EQUALS(true,  v.exists(2));
   UNIT_ASSERT_EQUALS(true,  v.exists("2"));
   UNIT_ASSERT_EQUALS(true,  v.exists(3));
   UNIT_ASSERT_EQUALS(true,  v.exists("3"));
   UNIT_ASSERT_EQUALS(false, v.exists(4));
   UNIT_ASSERT_EQUALS(false, v.exists("4"));
   UNIT_ASSERT_EQUALS(false, v.exists(-1));
   UNIT_ASSERT_EQUALS(false, v.exists("-1"));
   UNIT_ASSERT_EQUALS(false, v.exists("no such key"));

   const COLvar& cv = v;
   UNIT_ASSERT_EQUALS("{\"foo\":\"bar\"}", v[2].json());
   UNIT_ASSERT_EQUALS(true,  cv.exists(0));
   UNIT_ASSERT_EQUALS(true,  cv.exists("0"));
   UNIT_ASSERT_EQUALS(true,  cv.exists(1));
   UNIT_ASSERT_EQUALS(true,  cv.exists("1"));
   UNIT_ASSERT_EQUALS(true,  cv.exists(2));
   UNIT_ASSERT_EQUALS(true,  cv.exists("2"));
   UNIT_ASSERT_EQUALS(true,  cv.exists(3));
   UNIT_ASSERT_EQUALS(true,  cv.exists("3"));
   UNIT_ASSERT_EQUALS(false, cv.exists(4));
   UNIT_ASSERT_EQUALS(false, cv.exists("4"));

   const COLvar& v4 = cv["1"];
   UNIT_ASSERT_EQUALS("\"now\\ta\\nstring\"", v4.json());

   // test that const Map lookups of not present attributes return const COLvar of type None
   UNIT_ASSERT_EQUALS(COLvar::Map, v.type());
   {
      const COLvar& cv0 = v;
      const COLvar& cvAttr = cv0["Attr Not Present"];
      UNIT_ASSERT_EQUALS(COLvar::None, cvAttr.type());
      UNIT_ASSERT_EQUALS(true,         cvAttr.isNone());
      UNIT_ASSERT_EQUALS("",           cvAttr);
      UNIT_ASSERT_EQUALS(false,   bool(cvAttr));
      UNIT_ASSERT_EQUALS(0,        int(cvAttr));
      UNIT_ASSERT_EQUALS(0.0,   double(cvAttr));
      UNIT_ASSERT_EQUALS(COLvar::None, cvAttr["attribute that is not present"]["blah"]["blah"].type());
   }
   {
      const COLvar& cv0 = v;
      const COLvar& cvAttr = cv0[-9876];
      UNIT_ASSERT_EQUALS(COLvar::None, cvAttr.type());
      UNIT_ASSERT_EQUALS(true,         cvAttr.isNone());
      UNIT_ASSERT_EQUALS("",           cvAttr);
      UNIT_ASSERT_EQUALS(false,   bool(cvAttr));
      UNIT_ASSERT_EQUALS(0,        int(cvAttr));
      UNIT_ASSERT_EQUALS(0.0,   double(cvAttr));
      UNIT_ASSERT_EQUALS(COLvar::None, cvAttr["attribute that is not present"]["blah"]["blah"].type());
   }

   // test long chains of not present attributes
   UNIT_ASSERT_EQUALS("",           cv["no_such_key"][954274]["No such key."]);
   UNIT_ASSERT_EQUALS(COLvar::None, cv["no_such_key"][954274]["No such key."].type());
   UNIT_ASSERT_EQUALS(true,         cv["no_such_key"][954274]["No such key."].isNone());
   UNIT_ASSERT_EQUALS("",           cv["no_such_key"]);
   UNIT_ASSERT_EQUALS(true,         cv["no_such_key"].isNone());

   COLvar ev;
   ev.push_back(false);
   ev.push_back(true);
   ev.push_back("\b\f\\'\"");
   ev.push_back(false);
   UNIT_ASSERT_EQUALS("[false,true,\"\\b\\f\\\\'\\\"\",false]", ev.json());

   // Test the complete character range from 0x00 to 0xff.
   {
      char Buffer[257];
      int i = 0;
      for (; i <= 255; ++i) {
         Buffer[i] = (char)i;
      }
      Buffer[i] = 0;
      COLstring AllChars(Buffer, 256);
      COLvar AllCharsVar(AllChars);
      UNIT_ASSERT_EQUALS("\"\\x00\\x01\\x02\\x03\\x04\\x05\\x06\\x07\\b\\t\\n\\v\\f\\r\\x0E\\x0F\\x10\\x11\\x12\\x13\\x14\\x15\\x16\\x17\\x18\\x19\\x1A\\x1B\\x1C\\x1D\\x1E\\x1F !\\\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\\\]^_`abcdefghijklmnopqrstuvwxyz{|}~\\x7F\\x80\\x81\\x82\\x83\\x84\\x85\\x86\\x87\\x88\\x89\\x8A\\x8B\\x8C\\x8D\\x8E\\x8F\\x90\\x91\\x92\\x93\\x94\\x95\\x96\\x97\\x98\\x99\\x9A\\x9B\\x9C\\x9D\\x9E\\x9F\\xA0\\xA1\\xA2\\xA3\\xA4\\xA5\\xA6\\xA7\\xA8\\xA9\\xAA\\xAB\\xAC\\xAD\\xAE\\xAF\\xB0\\xB1\\xB2\\xB3\\xB4\\xB5\\xB6\\xB7\\xB8\\xB9\\xBA\\xBB\\xBC\\xBD\\xBE\\xBF\\xC0\\xC1\\xC2\\xC3\\xC4\\xC5\\xC6\\xC7\\xC8\\xC9\\xCA\\xCB\\xCC\\xCD\\xCE\\xCF\\xD0\\xD1\\xD2\\xD3\\xD4\\xD5\\xD6\\xD7\\xD8\\xD9\\xDA\\xDB\\xDC\\xDD\\xDE\\xDF\\xE0\\xE1\\xE2\\xE3\\xE4\\xE5\\xE6\\xE7\\xE8\\xE9\\xEA\\xEB\\xEC\\xED\\xEE\\xEF\\xF0\\xF1\\xF2\\xF3\\xF4\\xF5\\xF6\\xF7\\xF8\\xF9\\xFA\\xFB\\xFC\\xFD\\xFE\\xFF\"", AllCharsVar.json(0, COLvar::StrEncCpp));
   }

   // Takes 2 args {actualJson, expectedAnswer}
   testJsonHelper2("{\"A\":1,\"C\":2,\"B\":[]}", "{\"A\":1,\"B\":[],\"C\":2}");
   testJsonHelper2("{\"A\":1,\"C\":2,\"B\":[]}", "{\"A\":1,\"B\":[],\"C\":2}");
   testJsonHelper2("{\"A\":1,\"C\":2,\"B\":[  ]}", "{\"A\":1,\"B\":[],\"C\":2}");
   testJsonHelper2("{\"A\":1,\"C\":2,\"B\":{  }}", "{\"A\":1,\"B\":{},\"C\":2}");

   testJsonHelper2("{ }", "{}");
   testJsonHelper2("{ \r }", "{}");
   testJsonHelper2("{ \t }", "{}");
   testJsonHelper2("{ \n }", "{}");

   testJsonHelper2("[ ]", "[]");
   testJsonHelper2("[ \r ]", "[]");
   testJsonHelper2("[ \t ]", "[]");
   testJsonHelper2("[ \n ]", "[]");
   testJsonHelper2("[ \"\u002F\" ]", "[\"/\"]");

   testJsonHelper("[{}]");
   testJsonHelper("[]");
   testJsonHelper("{}");
   // Takes 2 args {actualJson, expectedAnswer}
   testJsonHelper2("{\"A\":1,\"C\":2,\"B\":[]}", "{\"A\":1,\"B\":[],\"C\":2}");
   testJsonHelper2("{\"A\":1,\"C\":2,\"B\":[]}", "{\"A\":1,\"B\":[],\"C\":2}");
   testJsonHelper2("{\"A\":1,\"C\":2,\"B\":[  ]}", "{\"A\":1,\"B\":[],\"C\":2}");
   testJsonHelper2("{\"A\":1,\"C\":2,\"B\":{  }}", "{\"A\":1,\"B\":{},\"C\":2}");

   testJsonHelper2("{ }", "{}");
   testJsonHelper2("{ \r }", "{}");
   testJsonHelper2("{ \t }", "{}");
   testJsonHelper2("{ \n }", "{}");

   testJsonHelper2("[ ]", "[]");
   testJsonHelper2("[ \r ]", "[]");
   testJsonHelper2("[ \t ]", "[]");
   testJsonHelper2("[ \n ]", "[]");
}

void testTestValidJson(){
   COL_FUNCTION(testTestValidJson);
   testValidJsonHelper("[{}]");
   testValidJsonHelper("{}");
   testValidJsonHelper("[]");
   testValidJsonHelper("{ \n }");
   testValidJsonHelper("{ \r }");
   testValidJsonHelper("{ \t }");
   testValidJsonHelper("[ \n ]");
   testValidJsonHelper("[ \r ]");
   testValidJsonHelper("[ \t ]");
   testValidJsonHelper("{ \"test\": [ ] }");
   testValidJsonHelper("{ \"test\": true }");
   testValidJsonHelper("{ \"test\": false }");
   testValidJsonHelper("{ \"test\": [ 1 ] }");
   testValidJsonHelper("{ \"test2\": {\"Foo\": \"Bar\"} }");
   testValidJsonHelper("[[{ \"two\" : 2332 }]]");
   testValidJsonHelper("{ \"bjKw6CecWlqekyziuDcXHT\": { \"Description\": \"#admin\", \"Email\": \"\", \"Guid\": \"bjKw6CecWlqekyziuDcXHT\", \"Password\": \"lHiRDkSSZwe3MVV6IuOVtSOdEMud67q0RNpiw3QHrWDyiSjeCN4ouqPs1zDHpsl5D7AE86745034E7C46F18E26306F3F5B26E88B8485D76A4B183E0F7D7D92F472C71A55BA62DE45FFE3670A927ED05E15339FC1358236F588AD1C3B422BEF78D02\", \"Username\": \"admin\"}}");
   testValidJsonHelper("{ \":\" : 1}");
   testValidJsonHelper("[1,2,33,4]");
   testValidJsonHelper("{ \"test\": 1}");
   testValidJsonHelper("[1,2]");
   testValidJsonHelper("[1]");
   testValidJsonHelper("[{}]");
   testValidJsonHelper("[true]");
   testValidJsonHelper("[false]");
   testValidJsonHelper("{ \"test\": \"foo\"}");
   testValidJsonHelper("{ \"test \": \"foo\"}");
   testValidJsonHelper("{ \"fields\": [\t{\"foo\": \"bar\"}]}");
   testValidJsonHelper("{ \"test\": \"\u002B\" }");
}

void testTrailingCommas(){
   COL_FUNCTION(testTrailingCommas);
   testValidJsonHelper("{ \"fields\": [], }");
   testValidJsonHelper("[1,]");
   testValidJsonHelper("[1, 2, 3,]");
   testValidJsonHelper("[\"foo\", \"bar\",]");
   testValidJsonHelper("{ \"fields\": [], }");

   testValidJsonHelper("{ \"fields\": { \"foo\": \"bar\" },}");
   testValidJsonHelper("{ \"fields\": { \"foo\": \"bar\", },}");
   testValidJsonHelper("[1,2,[11,12,] ]");
   testValidJsonHelper("[1,2,[11,12,],]");
   testValidJsonHelper("{ \"fields\": [],}");
   testValidJsonHelper("[1, { \"fields\": [],},]");
   testValidJsonHelper("true");
   testValidJsonHelper("1");
   testValidJsonHelper("\"test\"");
   testValidJsonHelper("{ \"fields\": [],}");
   testValidJsonHelper("{ \"fields\": {\"foo\": \"bar\",},}");
}

void testInvalidJson(){
   COL_FUNCTION(testInvalidJson);
   // Testing broken JSON
   testInvalidJsonHelper("{ \"test\": [ ]");
   testInvalidJsonHelper("[{ \"test [ ]}]-----------------------------------------------------------------");
   testInvalidJsonHelper("[{ \"test-----------------------------------------------------------------\" : 1}, \"test2]");
   testInvalidJsonHelper("[{ \"-----------------------------------------------------------------\" : 1}, \"test2, \"-----------------------------------------------------------------\"]");
   testInvalidJsonHelper("{ [ ]");
   testInvalidJsonHelper("{ two : 2332 }");
   testInvalidJsonHelper("[ two, 2332 ]");
   testInvalidJsonHelper("{ \"test\" [ ] }");
   testInvalidJsonHelper("{ \"test2\": {\"Foo\" \"Bar\"} }");
   testInvalidJsonHelper("{ , \"test\"}");
   testInvalidJsonHelper("[ , \"test\"]");
   testInvalidJsonHelper("{ : 1}");
   testInvalidJsonHelper("[ \":\" : ]");
   testInvalidJsonHelper("[ \":\" : 1]");
   testInvalidJsonHelper("   ");
   testInvalidJsonHelper("\t\r\n  \t\r\n ");
   testInvalidJsonHelper("\t  \r \n ");
   testInvalidJsonHelper("{\"foo\" 1$");
   testInvalidJsonHelper("{\"foo\" \"1$\"");
   testInvalidJsonHelper("{\"foo\" false");
   testInvalidJsonHelper("{\"foo\": \"bar ");
   
   // Testing trailing comma
   testInvalidJsonHelper("[1, 2, 3,,],,");
   testInvalidJsonHelper(",,,[1, 2, 3,]");
   testInvalidJsonHelper("[{ \"A\" : \"ff\"}, { \"B\", : \"gg\" },]");

   testInvalidJsonHelper("{\"schema\":}");
   testInvalidJsonHelper("    tt [1,2,[11,12,],, ] ");
   testInvalidJsonHelper("{ \"test\": \u002B }");
}

void testInvalidDifficult(){
   COL_FUNCTION(testInvalidDifficult);
   testInvalidJsonHelper("true,,");
   testInvalidJsonHelper("1,,");
   testInvalidJsonHelper("\"test\",,");
   testInvalidJsonHelper("\"test\" gsgd");
   testInvalidJsonHelper("{ \"fields\": [1,2,3,],},,");
   testInvalidJsonHelper("\"test\" {}");
   testInvalidJsonHelper("\"test\" []");
}

void testTrailingBodyCommas(){
   testValidJsonHelper("{ \"fields\": [],},");
   testValidJsonHelper("[1, { \"fields\": [],},],");
   testValidJsonHelper("true,");
   testValidJsonHelper("1,");
   testValidJsonHelper("\"test\",");
   testValidJsonHelper("{ \"fields\": [],},");
}

void testJsonParser(UNITapp* pApp) {
   pApp->add("json/empty", &testJsonEmpty);
   pApp->add("json/unicode/printable", &testPrintableUnicodeChar);
   pApp->add("test/json/pipedrive", &testLargePipedriveJson);

   pApp->add("json/error",          &testJsonError);
   pApp->add("json/simple_string",  &testSimpleString);
   pApp->add("json/escaped_string", &testEscapedString);
   pApp->add("json/single_quote",   &testSingleQuotedString);
   pApp->add("json/boolean_true",   &testBooleanTrue);
   pApp->add("json/boolean_false",  &testBooleanFalse);
   pApp->add("json/null",           &testNull);
   
   //pApp->add("json/unicode",        &testUnicodeString);
   pApp->add("json/number",         &testNumber);
   pApp->add("json/negative",       &testNegative);
   pApp->add("json/double",         &testDouble);
   pApp->add("json/array",          &testArray);
   pApp->add("json/object",         &testObject);
   pApp->add("json/empty/object",   &testEmptyObject);
   pApp->add("json/empty/array",    &testEmptyArray);
   pApp->add("json/unicode",        &testUnicode);
   pApp->add("json/andrew",         &testJson);
   pApp->add("json/valid",          &testTestValidJson);
   pApp->add("json/trailing_commas",&testTrailingCommas);
   pApp->add("json/invalid",        &testInvalidJson);
   
   //IX-3796 It's questionable that the examples in this test should be parsed.
   //pApp->add("json/test_trailing_body_commas", &testTrailingBodyCommas);
   pApp->add("json/invalid/difficult", &testInvalidDifficult);  
}  
