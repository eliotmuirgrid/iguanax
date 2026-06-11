//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: HTTPbodyParser
//
// Description:
//
// Implementation.  For extracting either chunked or full HTTP body.
//
// Author: Ramanan Sivasothinathan
// Date:   Thursday, September 10th, 2009 @ 11:57:49 AM
//---------------------------------------------------------------------------
#include "HTTPprecomp.h"
#pragma hdrstop

#include "HTTPbodyParser.h"
#include "HTTPheader.h"
#include "HTTPheaderParser.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

enum HTTPbodyParseState
{
   HTTP_BODY_INIT,       // Initializing
   HTTP_BODY_NEXT_CHUNK, // Waiting for next chunk
   HTTP_BODY_TERM_CHUNK, // Reading-in CRLF terminator of chunk
   HTTP_BODY_READ_CHUNK, // Reading-in current chunk
   HTTP_BODY_READ_FIXED, // Reading-in non-chunked body
   HTTP_BODY_DONE,       // Parsing is completed
   HTTP_BODY_ERROR       // Parser is in error state
};

class HTTPbodyParserPrivate
{
public:
   HTTPbodyParserPrivate(bool CombineChunks);
   void reset();
   void write(const void* pData, COLuint32 Size, COLsink& Sink);
   HTTPparseResult parse(const void* pData, COLuint32 SizeOfData, const HTTPheader& Header, COLsink& Body);

   COLstring Buffer;
   COLsinkString BufferSink;
   HTTPbodyParseState State;
   COLuint32          BytesNeeded;
   COLuint32          ParseIndex;
   COLstring          LastError;
   bool         Combine;
};

//---------------------------------------------------------------------------

HTTPbodyParserPrivate::HTTPbodyParserPrivate(bool CombineChunks) : BufferSink(Buffer), Combine(CombineChunks) { reset(); }

void HTTPbodyParserPrivate::reset(){
   Buffer.clear();
   State       = HTTP_BODY_INIT;
   BytesNeeded = 0;
   ParseIndex  = 0;
   LastError.clear();
}

void HTTPbodyParserPrivate::write(const void* pData, COLuint32 Size, COLsink& Sink){
   if (Size == 0) return;
   Sink.write(pData, Size);
   ParseIndex  += Size;
   BytesNeeded -= Size;
}

HTTPparseResult HTTPbodyParserPrivate::parse(const void* pData, COLuint32 SizeOfData, const HTTPheader& Header, COLsink& Body){
   COL_METHOD(HTTPbodyParserPrivate::parse);
   COLPRECONDITION(SizeOfData >= ParseIndex);
   while (true) {
      const COLuint8* CrLf = NULL;
      const COLuint8* Data = ((const COLuint8*) pData) + ParseIndex; // Skip bytes already processed
      COLuint32       Size = SizeOfData                - ParseIndex; // Skip bytes already processed
      COLuint32       Pos  = 0;
      unsigned int    ChunkSize = 0;
      switch (State) {
         case HTTP_BODY_DONE:
            return HTTP_PARSE_COMPLETE; // break;
         case HTTP_BODY_ERROR:
            return HTTP_PARSE_ERROR;    // break;
         case HTTP_BODY_INIT:
            Pos = Header.findHeader("Transfer-Encoding");
            if (Pos != npos && Header.headerValue(Pos) == "chunked") {
               COL_TRC("We have chunked encoding of the HTTP body.");
               BytesNeeded = 0;
               Buffer.clear();
               State = HTTP_BODY_NEXT_CHUNK;
            } else {
               BytesNeeded = (COLuint32) (HTTPheaderGetContentLength(Header));
               COL_TRC("We have a fixed sized HTTP body - i.e. Content-Length");
               State = HTTP_BODY_READ_FIXED;
            }
            COL_VAR(BytesNeeded);
            continue; // break;
         case HTTP_BODY_READ_FIXED:
            write(Data, (Size>BytesNeeded) ? BytesNeeded : Size, Body);
            if (BytesNeeded > 0) { return HTTP_PARSE_NEED_MORE_DATA; }
            State = HTTP_BODY_DONE;
            return HTTP_PARSE_COMPLETE; // break;
         case HTTP_BODY_READ_CHUNK:
            write(Data, (Size>BytesNeeded) ? BytesNeeded : Size, Body);
            if (BytesNeeded > 0) { return HTTP_PARSE_NEED_MORE_DATA; }
            Buffer.clear();
            BytesNeeded = 2;
            State = HTTP_BODY_TERM_CHUNK;
            continue; // break;
         case HTTP_BODY_TERM_CHUNK:
            write(Data, (Size>BytesNeeded) ? BytesNeeded : Size, BufferSink);
            if (BytesNeeded > 0) { return HTTP_PARSE_NEED_MORE_DATA; }
            if (::memcmp(Buffer.data(), "\x0D\x0A", 2) != 0) {
               LastError = "Chunk terminator (CRLF) is missing";
               State = HTTP_BODY_ERROR;
               return HTTP_PARSE_ERROR;
            }
            Buffer.clear();
            State = HTTP_BODY_NEXT_CHUNK;
            if (!Combine) { return HTTP_PARSE_CHUNK_COMPLETE; }
            continue; // break;
         case HTTP_BODY_NEXT_CHUNK:
            if (Buffer.size() > 1000) {
               LastError = "Chunk header is too long";
               State = HTTP_BODY_ERROR;
               return HTTP_PARSE_ERROR;
            }
            if (Size==0 || (CrLf=(const COLuint8*)(HTTPfindNewline(Data,Size)))==NULL) { return HTTP_PARSE_NEED_MORE_DATA; }
            write(Data, CrLf-Data, BufferSink);
            Buffer.write("", 1); // Add null terminator
            if (::sscanf((const char*) (Buffer.data()), "%X", &ChunkSize) != 1) {
               LastError = "Invalid chunk length";
               State = HTTP_BODY_ERROR;
               return HTTP_PARSE_ERROR;
            }
            Buffer.clear();
            if (ChunkSize == 0){ // End of chunking
               BytesNeeded = 0;
               State = HTTP_BODY_DONE;
               return HTTP_PARSE_COMPLETE;
            } else { // Start of next chunk
               BytesNeeded = ChunkSize;
               State = HTTP_BODY_READ_CHUNK;
               continue;
            }
            break;
         default:
            LastError = "Unexpected parser internal state";
            State = HTTP_BODY_ERROR;
            return HTTP_PARSE_ERROR;
      }
   }
   LastError = "Unexpected parser internal state";
   State = HTTP_BODY_ERROR;
   return HTTP_PARSE_ERROR;
}

//---------------------------------------------------------------------------

HTTPbodyParser::HTTPbodyParser(bool CombineChunks)       { pMember = new HTTPbodyParserPrivate(CombineChunks); }
HTTPbodyParser::~HTTPbodyParser()                        { delete pMember                                    ; }
void HTTPbodyParser::reset()                             { pMember->reset()                                  ; }
COLuint32 HTTPbodyParser::parseIndex()                   { return pMember->ParseIndex                        ; }
void HTTPbodyParser::resetParseIndex()                   { pMember->ParseIndex = 0                           ; }
const COLstring& HTTPbodyParser::lastError() const       { return pMember->LastError                         ; }

HTTPparseResult HTTPbodyParser::parse(const void* pData, COLuint32 SizeOfData, const HTTPheader& Header, COLsink& Body){
   return pMember->parse(pData, SizeOfData, Header, Body);
}
