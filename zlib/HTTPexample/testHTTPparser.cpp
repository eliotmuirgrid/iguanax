//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testHTTPparser.cpp
//
// Description:
//
// This is a test of HTTPparser
//
// Author: Eliot Muir
// Date:   Fri 24 Jan 2014 11:32:00 EST
//---------------------------------------------------------------------------

#include <TST/TSTtestCollection.h>
#include <TST/TSTtestFunction.h>
#include <TST/TSThelpers.h>

#include <HTTP/HTTPparser.h>
#include <HTTP/HTTPvariables.h>

#include <FIL/FILutils.h>
#include <FIL/FILdirEnumerator.h>
#include <FIL/FILpathUtils.h>

#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void testLoadSample(const COLstring& Name, COLstring* pContent){
   COL_FUNCTION(testLoadData);
   COLstring File, Content;
   File = FILexecutableDir();
   FILaddPathSeparator(File);
   File += "SampleRequests";
   FILaddPathSeparator(File);
   File += Name;
   COL_VAR(File);
   FILreadFile(File, pContent);
}

void testRequestInfoInJSON(const HTTPrequestInfo& Info, COLvar* pTree){
   COLvar& Tree = *pTree;
   COLvar& HeaderSection = Tree["header"];
   HeaderSection["request"]["method"] = Info.Request.method();
   HeaderSection["request"]["location"] = Info.Request.location();
   HeaderSection["request"]["query"] = Info.Request.queryString();
   const HTTPheader& Headers = Info.Request.header();
   for (int i=0; i < Headers.countOfHeader(); i++){
      HeaderSection["headers"][Headers.headerKey(i)] = Headers.headerValue(i);
   }
   Tree["body"] = Info.Body;
   // The header section is no longer supported by the parser, since the buffer gets shifted around
   Tree["header_data"] = Info.Header;
   Tree["size_headers"] = Info.Header.size();
   Tree["size_body"] = Info.Body.size();
}

void testLoad(const COLstring& Name, COLstring* pContent){
   COL_FUNCTION(testLoad);
   COLstring File, Content;
   File = FILexecutableDir();
   FILaddPathSeparator(File);
   File += "TestData";
   FILaddPathSeparator(File);
   File += Name + ".bin";
   COL_VAR(File);
   FILreadFile(File, pContent);
}

static void testCompareToExpected(const COLstring& Name, const COLvar& ActualTree){
   COL_FUNCTION(testCompareToExpected);
   COLstring File, Content;
   File = FILexecutableDir();
   FILaddPathSeparator(File);
   File += "TestData";
   FILaddPathSeparator(File);
   File += Name + ".out.json";
   COL_VAR(File);
   if (!FILfileExists(File)){
      COLcout << "WARNING: Writing out expected file." << newline;
      FILwriteFile(File, ActualTree.json(true));
   }
   COLstring ExpectedJson;
   FILreadFile(File, &ExpectedJson);
   COLvar Expected;
   Expected.parse(ExpectedJson.c_str(), ExpectedJson.size());
   TST_ASSERT_EQUALS(Expected.json(), ActualTree.json());
}

static void testIncrementalParse(const COLstring& TestName, const COLstring& Request) {
   COL_FUNCTION(testIncrementalParse);
   HTTPparser Parser;
   COLstring ParseBuffer;
   Parser.setBuffer(&ParseBuffer);
   HTTPrequestInfo ParsedRequest;
   HTTPparser::ParseResult Result;
   COLvar ResultTree;
   int CountOfRequest = 0;
   // Do an incremental parse.
   COLstring ErrorMsg;
   for (int i = 0; i < Request.size(); i++){
      ParseBuffer.append(1, Request[i]);
      Result = Parser.parse(&ParsedRequest, &ErrorMsg);
      if (Result == HTTPparser::Success){
         testRequestInfoInJSON(ParsedRequest, &ResultTree[CountOfRequest]);
         CountOfRequest++;
      }
   }
   COL_VAR(CountOfRequest);
   testCompareToExpected(TestName, ResultTree);
}

static void testWholeBufferParse(const COLstring& TestName, const COLstring& Request) {
   COL_FUNCTION(testWholeBufferParse);
   HTTPparser Parser;
   COLstring ParseBuffer;
   Parser.setBuffer(&ParseBuffer);
   HTTPrequestInfo ParsedRequest;
   HTTPparser::ParseResult Result;
   int CountOfRequest = 0;
   COLstring ErrorMsg;
   COLvar ResultTree;
   ParseBuffer.append(Request);
   do {
      Result = Parser.parse(&ParsedRequest, &ErrorMsg);
      if (Result == HTTPparser::Success) {
         testRequestInfoInJSON(ParsedRequest, &ResultTree[CountOfRequest]);
         CountOfRequest++;
      }
   } while (Result == HTTPparser::Success);
   COL_VAR(CountOfRequest);
   TST_ASSERT_EQUALS(HTTPparser::EmptyBuffer, Result);
   testCompareToExpected(TestName, ResultTree);
}

static void testHighWaterParse(const COLstring& TestName, const COLstring& Request) {
   COL_FUNCTION(testHighWaterParse);
   HTTPparser Parser;
   COLstring ParseBuffer;
   Parser.setBuffer(&ParseBuffer);
   HTTPrequestInfo ParsedRequest;
   HTTPparser::ParseResult Result;
   int CountOfRequest = 0;
   COLstring ErrorMsg;
   COLvar ResultTree;
   COL_TRC("Prime the buffer.");
   for (int i=0;i < 1000; i++){
      ParseBuffer.append(Request);
      do{
         Result = Parser.parse(&ParsedRequest, &ErrorMsg);
      } while (Result == HTTPparser::Success);
   }
   COL_TRC("One last write.");
   ParseBuffer.append(Request);
   do {
      Result = Parser.parse(&ParsedRequest, &ErrorMsg);
      if (Result == HTTPparser::Success){
         testRequestInfoInJSON(ParsedRequest, &ResultTree[CountOfRequest]);
         CountOfRequest++;
      }
   } while (Result == HTTPparser::Success);

   COL_VAR(CountOfRequest);
   TST_ASSERT_EQUALS(HTTPparser::EmptyBuffer, Result);
   testCompareToExpected(TestName, ResultTree);
}


static void testParser(const COLstring& TestName){
   COL_FUNCTION(testParser);
   COLstring Request;
   testLoad(TestName, &Request);
   COL_VAR(Request.size());
   COL_DUMP("Request data:", Request.c_str(),Request.size());
   
   testIncrementalParse(TestName, Request);
   testWholeBufferParse(TestName, Request);
   testHighWaterParse(TestName, Request);
}

static void testSimple(){
   testParser("post_example");
}


static void testArray(){
   COL_FUNCTION(testArray);
   COLstring FileName;
   COLstring DataDir = FILexecutableDir();
   FILdirEnumerator Enumerator(DataDir + "TestData/*.bin");
   while(Enumerator.getFile(FileName)){
      COLcout << FileName << newline;
      testParser(FILpathNameWithoutLastExt(FileName));
   }
}

static void testParseVariables(){
   COL_FUNCTION(testParseVariables);
   COLvar Vars;
   HTTPparseVariables("life=fred&smith=one+space&jerk=chicken%20tastes%20good.&hello=hi&hello=goodbye", &Vars);
   TST_ASSERT_EQUALS("fred", Vars["life"].asString());
   TST_ASSERT_EQUALS("one space", Vars["smith"].asString());
   TST_ASSERT_EQUALS("chicken tastes good.", Vars["jerk"].asString());
   TST_ASSERT(Vars["hello"].isArray());
   TST_ASSERT_EQUALS("hi", Vars["hello"][0].asString());
   TST_ASSERT_EQUALS("goodbye", Vars["hello"][1].asString());
   COL_VAR(Vars);
}

TSTtestable* testHTTPparser(){
   TST_START_COLLECTION("HTTPparser")
      TST_ADD_TEST_FUNCTION(testSimple);
      TST_ADD_TEST_FUNCTION(testArray);
      TST_ADD_TEST_FUNCTION(testParseVariables);
   TST_END_COLLECTION
}
