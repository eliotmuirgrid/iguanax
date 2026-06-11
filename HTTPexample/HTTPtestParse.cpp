//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Application: HTTPexample 
//
// Description:
//
// Simple test routines for HTTP library
// 
// Author: Eliot Muir
// Date:   Thursday, December 7th, 2006 @ 03:22:24 PM
//
//---------------------------------------------------------------------------
#include "HTTPexamplePrecomp.h"
#pragma hdrstop

#include <COL/COLlog.h>

COL_LOG_MODULE;

#include <CMD/CMDlineParser.h>

#include <HTTP/HTTPrequest.h>
#include <HTTP/HTTPutils.h>
#include <HTTP/HTTPheader.h>
#include <HTTP/HTTPheaderParser.h>
#include <HTTP/HTTPrequestParser.h>
#include <HTTP/HTTPbodyParser.h>

#include <FIL/FILfile.h>
#include <FIL/FILdirEnumerator.h>
#include <FIL/FILutils.h>
#include <FIL/FILpathUtils.h>

#include <DIF/DIFdiffer.h>

void HTTPtestHeaderParser()
{
   HTTPheader Header;
   HTTPheaderParser Parser(Header);

   COLstring MimeData;
   COLostream Stream(MimeData);

   Stream << "Dodo: blah\r\n";
   Stream << "Bilbo: blah\r\n";
   Stream << "Smith: blah do da\r\n";
   Stream << "\r\n";
  
   HTTPparseResult Result = Parser.parse(MimeData.c_str(), MimeData.size());

   if (Result == HTTP_PARSE_ERROR)
   {
      COL_ERROR_STREAM(Parser.lastError() << " at " << Parser.currentParseIndex(), COLerror::PreCondition);
      return;
   }
   TST_ASSERT(Result == HTTP_PARSE_COMPLETE);
   
   Parser.reset(Header);

   for (COLuint32 SizeOfData=0; SizeOfData < MimeData.size(); SizeOfData++)
   {
      Result = Parser.parse(MimeData.c_str(), SizeOfData);
      if (Result == HTTP_PARSE_ERROR)
      {
         COL_ERROR_STREAM(Parser.lastError() << " at " << Parser.currentParseIndex(), COLerror::PreCondition);
         return;
      }
      TST_ASSERT(Result == HTTP_PARSE_NEED_MORE_DATA);
   }

   Result = Parser.parse(MimeData.c_str(), MimeData.size());

   TST_ASSERT(Result == HTTP_PARSE_COMPLETE);
}

void HTTPtestHeaderParserErrorHandling()
{
   HTTPheader Header;
   HTTPheaderParser Parser(Header);

   COLstring MimeData;
   COLostream Stream(MimeData);

   Stream << "Dodo:blah\r\n";
   Stream << "\r\n";
  
   HTTPparseResult Result = Parser.parse(MimeData.c_str(), MimeData.size());

   TST_ASSERT(Result == HTTP_PARSE_ERROR);
   COLstring ErrorString;
   COLostream ErrorStream(ErrorString);
   ErrorStream.setNewLine("\r\n");

   HTTPshowParseErrorLocation(MimeData.c_str(), MimeData.size(), Parser.currentParseIndex(), ErrorStream);
   ErrorStream << Parser.lastError();
   COL_VAR(ErrorString);
   COLstring Expected = "Dodo:blah\r\n     ^\r\nExpected space after header : character";
   TST_ASSERT(ErrorString == Expected);

   MimeData.clear();
   ErrorString.clear();
   Stream << "Dodo: blah\r\n";
   Stream << "Dies: blah\r";
   Stream << "\r\n\r";
  
   Parser.reset(Header);
   Result = Parser.parse(MimeData.c_str(), MimeData.size());

   TST_ASSERT(Result == HTTP_PARSE_ERROR);
   TST_ASSERT(Parser.lastError() == "Expected \\n after \\r");
   MimeData.clear();
   ErrorString.clear();   
   Stream << "Dodo: blah\r\n";
   Stream << "Dies: blah\r\n";
   Stream << "\r\r";
  
   Parser.reset(Header);
   Result = Parser.parse(MimeData.c_str(), MimeData.size());
   TST_ASSERT(Result == HTTP_PARSE_ERROR);
   TST_ASSERT(Parser.lastError() == "Expected \\n after \\r at end of header block");   
}   

void HTTPoutputResultFile(const HTTPrequest& Request, const void* pBodyData, COLuint32 SizeOfBody, const COLstring& FileName)
{
   FILfile OutFile;
   OutFile.open(FileName, FILfile::Rewrite);
   COLostream OutStream(OutFile);
   OutStream.setNewLine("\r\n"); 

   HTTPdumpRequest(OutStream, Request, pBodyData, SizeOfBody);
}

COLuint32 HTTPpopulateRequest(HTTPrequestParser& Parser, HTTPrequest& Request, COLstring& Data, const COLstring& FileName) {
   COLstring Content;
   FILreadFile(FileName, &Content);
   COL_TRC("Parsing " << FileName << "...");
   Data.write(Content.c_str(), Content.size());

   // we do it this way to test the reset logic
   Parser.reset(Request);
   COLuint32 Result = Parser.parse((const char*)Data.c_str(), Data.size());
   COL_VAR(Result);
   COLPOSTCONDITION(Result == HTTP_PARSE_COMPLETE);
   return Parser.endOfHeader();
}

void HTTPtestRequest(const HTTPrequest& Request, COLuint32 EndOfHeader, const COLstring& Data, const COLstring& OutFile, const COLstring& Expected)
{
   HTTPoutputResultFile(Request, Data.c_str() + EndOfHeader, Data.size()-EndOfHeader, OutFile);

   if (FILfileExists(Expected))
   {
      DIFdiffer Differ;
      if (!Differ.filesMatch(OutFile, Expected, COLcerr))
      {
         COL_ERROR_STREAM("Test failed.  Expected output " << newline << Expected << newline 
            << "does not match output " << newline << OutFile, COLerror::PreCondition);  
      }
   }
   else
   {
      COL_TRC("Expected output not present so generating it.");
      HTTPoutputResultFile(Request, Data.data() + EndOfHeader, Data.size()-EndOfHeader, Expected);
   }
}

void HTTPtestParseRequest() {
   COLstring WorkingDir = FILworkingDir();

   FILsetWorkingDir("SampleRequests");
   FILdirEnumerator SampleEnumerator("*.txt");

   HTTPrequestParser RequestParser;
   try
   {
      COLstring InputFile;
      while (SampleEnumerator.getFile(InputFile))
      {
         COL_VAR(InputFile);
         COLstring OutFile = FILpathNameWithoutLastExt(InputFile) + ".out";
         COLstring ExpectedOut = FILpathNameWithoutLastExt(InputFile) + ".expected";

         COL_TRC("Testing request " << InputFile << "...");
         HTTPrequest Request;
         COLstring Data;
         COLuint32 EndOfHeader = HTTPpopulateRequest(RequestParser, Request, Data, InputFile);
         HTTPtestRequest(Request, EndOfHeader, Data, OutFile, ExpectedOut);
         COL_TRC("Done!");
      }
   }
   catch(...)
   {
      FILsetWorkingDir(WorkingDir);
      throw;
   }
   FILsetWorkingDir(WorkingDir);
}
   
void HTTPtestBodyParser()
{
   HTTPparseResult Result;
   HTTPbodyParser Parser;
   COLstring Body;
   COLsinkString BodySink(Body);
   HTTPheader Header[6];
   const char* Ref = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
   const char* Raw[] =
   {
      "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890", // Fixed Body

      "19\x0D\x0A"      "abcdefghijklmnopqrstuvwxy"    "\x0D\x0A" // Chunked Body
      "1C\x0D\x0A"      "zABCDEFGHIJKLMNOPQRSTUVWXYZ1" "\x0D\x0A"
      "9\x0D\x0A"       "234567890"                    "\x0D\x0A"
      "0\x0D\x0A",

      "19\x0D\x0A"      "abcdefghijklmnopqrstuvwxy"    "\x0D\x0A" // Same chunk Body as above except extra space
      "1c     \x0D\x0A" "zABCDEFGHIJKLMNOPQRSTUVWXYZ1" "\x0D\x0A" // after chunk length and lower case hex for
      "9\x0D\x0A"       "234567890"                    "\x0D\x0A" // chunk length
      "0    \x0D\x0A",

      "19\x0D\x0A"      "abcdefghijklmnopqrstuvwxy"    "\x0D\x0A" // Same chunk Body as above except chunk is
      "1C\x0D\x0A"      "zABCDEFGHIJKLMNOPQRSTUVWXYZ1" "XY"       // terminated with chars other than CRLF
      "9\x0D\x0A"       "234567890"                    "\x0D\x0A"
      "0\x0D\x0A",

      "19\x0D\x0A"      "abcdefghijklmnopqrstuvwxy"    "\x0D\x0A" // Same chunk Body as above except chunk
      "QA\x0D\x0A"      "zABCDEFGHIJKLMNOPQRSTUVWXYZ1" "\x0D\x0A" // length is not hex number
      "9\x0D\x0A"       "234567890"                    "\x0D\x0A"
      "0\x0D\x0A",

      ""                                                          // Null body
   };
   Header[0].addHeader("Content-Length", "62");
   Header[1].addHeader("Transfer-Encoding", "chunked");
   Header[2].addHeader("Transfer-Encoding", "chunked");
   Header[3].addHeader("Transfer-Encoding", "chunked");
   Header[4].addHeader("Transfer-Encoding", "chunked");

   // Parse the input strings into individual blocks, making sure that
   // Parser.parse only returns "complete" when the size of the block is the
   // same as the size of the input.
   const int RefLen = ::strlen(Ref);
   // Iterate over the first three input sequences in the Raw array. The last
   // three entries are tested at the end of the function.
   for(int InputIndex=0 ; InputIndex<3 ; InputIndex++)
   {
      // Parse the input string into blocks of varying sizes.
      int Len = ::strlen(Raw[InputIndex]);
      for(int ReadBlock1Size=0 ; ReadBlock1Size<=Len ; ReadBlock1Size++)
      {
         for(int ReadBlock2Size=ReadBlock1Size; ReadBlock2Size<=Len ; ReadBlock2Size++)
         {
            int Blocks[] = { ReadBlock1Size, ReadBlock2Size, Len };
            const char* RawBytes = Raw[InputIndex];
            int Offset = 0;
            Parser.reset();
            Body.clear();
            for(int BlockIndex=0 ; BlockIndex<3 ; BlockIndex++)
            {
               Result = Parser.parse(RawBytes, Blocks[BlockIndex]-Offset, Header[InputIndex], BodySink);
               TST_ASSERT(Result != HTTP_PARSE_ERROR);
               TST_ASSERT((Result == HTTP_PARSE_COMPLETE) == (Blocks[BlockIndex] == Len));
            }
            TST_ASSERT(Body.size() == RefLen);
            TST_ASSERT(::strncmp(Ref, (const char*) (Body.data()), RefLen) == 0);
            ::memset((void*)Body.c_str(), 0, RefLen); // Clear contents to be sure we don't use stale data
         }
      }
   }

   // Parse the input strings into individual blocks, resetting the parse index
   // after each parse.
   for(int InputIndex=0 ; InputIndex<3 ; InputIndex++)
   {
      // Parse the input string into blocks of varying sizes.
      int Len = ::strlen(Raw[InputIndex]);
      for(int ReadBlock1Size=0 ; ReadBlock1Size<=Len ; ReadBlock1Size++)
      {
         for(int ReadBlock2Size=ReadBlock1Size; ReadBlock2Size<=Len ; ReadBlock2Size++)
         {
            int Blocks[] = { ReadBlock1Size, ReadBlock2Size, Len };
            const char* RawBytes = Raw[InputIndex];
            int Offset = 0;
            Parser.reset();
            Body.clear();
            for(int BlockIndex=0 ; BlockIndex<3 ; BlockIndex++)
            {
               Result = Parser.parse(RawBytes, Blocks[BlockIndex]-Offset, Header[InputIndex], BodySink);
               TST_ASSERT(Result != HTTP_PARSE_ERROR);
               TST_ASSERT((Result == HTTP_PARSE_COMPLETE) == (Blocks[BlockIndex] == Len));

               Offset   += Parser.parseIndex();
               RawBytes += Parser.parseIndex();
               Parser.resetParseIndex();
            }
            TST_ASSERT(Body.size() == RefLen);
            TST_ASSERT(::strncmp(Ref, (const char*) (Body.data()), RefLen) == 0);
            ::memset((void*)Body.c_str(), 0, RefLen); // Clear contents to be sure we don't use stale data
         }
      }
   }

   Parser.reset();
   Body.clear();
   Result = Parser.parse(Raw[3], ::strlen(Raw[3]), Header[3], BodySink);
   TST_ASSERT(Result == HTTP_PARSE_ERROR);
   TST_ASSERT(Parser.lastError() == "Chunk terminator (CRLF) is missing");

   Parser.reset();
   Body.clear();
   Result = Parser.parse(Raw[4], ::strlen(Raw[4]), Header[4], BodySink);
   TST_ASSERT(Result == HTTP_PARSE_ERROR);
   TST_ASSERT(Parser.lastError() == "Invalid chunk length");

   Parser.reset();
   Body.clear();
   Result = Parser.parse(Raw[5], ::strlen(Raw[5]), Header[5], BodySink);
   TST_ASSERT(Result == HTTP_PARSE_COMPLETE);
   TST_ASSERT(Body.size() == 0);
}
