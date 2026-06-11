//---------------------------------------------------------------------------
// Copyright (C) 1997-2019 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: WEBchunkEncodingParser
//
// Author: Eliot Muir
//
// Description:
//
// Chunked encoding parser
//---------------------------------------------------------------------------

#include "WEBchunkEncodingParser.h"
#include "WEButils.h"

#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

// Example of the format we are parsing.
/*
4\r\n
Wiki\r\n
5\r\n
\r\n0d\r\n\r\n
E\r\n
 in\r\n
\r\n
chunks.\r\n
0\r\n
\r\n
*/

WEBchunkEncodingParser::WEBchunkEncodingParser(){
   COL_METHOD(WEBchunkEncodingParser::WEBchunkEncodingParser);
}

WEBchunkEncodingParser::~WEBchunkEncodingParser(){
   COL_METHOD(WEBchunkEncodingParser::~WEBchunkEncodingParser);
}

void WEBchunkEncodingParser::reset(){ 
   COL_METHOD(WEBchunkEncodingParser::reset);
   m_Data.clear(); 
}

void WEBtruncateString(COLstring* pData, int Count){
   COL_FUNCTION(WEBtruncateString);
   pData->shift(Count);
}

int WEBfindNextChunk(COLstring& Data, COLstring* pResult){
   COL_FUNCTION(WEBfindNextChunk);
   // XXX\r\n  is the format of the hex lengths we are looking for - cheating by only
   // looking for the \r terminating character and assuming it is followed by \n
   int LFPosition;  // deliberately not initializing this.
   if (!Data.find(&LFPosition, '\r')){ 
      COL_TRC("Chunk length is not terminated yet.");
      return -1;
   }
   int ChunkSize = WEBconvertHexString(Data.c_str(), LFPosition);
   COL_VAR(ChunkSize);
   int ExpectedDataPos = 4 + ChunkSize + LFPosition;
   if (Data.size() >= ExpectedDataPos){
      COL_TRC("Found chunk of size " << ChunkSize);
      // Could be a Schemiel the painter algorithm here with appending
      // if the string is not cleared.
      pResult->append(Data.c_str()+LFPosition+2, ChunkSize);
      COL_DUMP("Chunk:", Data.c_str()+LFPosition+2, ChunkSize);
      WEBtruncateString(&Data, ExpectedDataPos);
      COL_DUMP("Next Chunk:", Data.c_str(), COL_MIN(8, Data.size()));
      return ChunkSize;
   } else {
      COL_DBG("We do not have a complete chunk.");
      return -1;
   }  
}

bool WEBchunkEncodingParser::parseData(const char* pExtraData, int Size, COLstring* pChunk){
   COL_FUNCTION(WEBchunkEncodingParser::parseData);
   COL_TRC("Added " << Size << " now total is " << Size + m_Data.size()); 
   m_Data.append(pExtraData, Size);
   int DumpSize = COL_MIN(100, m_Data.size());
   COL_DUMP("Last 100 bytes:", m_Data.c_str()+m_Data.size()-DumpSize, DumpSize);
   int ChunkSize = -1;
   do{
      ChunkSize = WEBfindNextChunk(m_Data, pChunk);
      if (ChunkSize != -1 && m_pChunkCallback.get()){
         m_pChunkCallback->run(pChunk, false);
      }
   } while(ChunkSize > 0);
   if (ChunkSize == 0){
      COL_TRC("We have the last zero chunk");
      m_pChunkCallback->run(pChunk, true);
   }
   return ChunkSize == 0;
}

void WEBchunkEncodingParser::setChunkCallback(COLauto< COLclosure2<COLstring*, bool> > pChunkCallback){
   COL_FUNCTION(WEBchunkEncodingParser::setChunkCallback);
   m_pChunkCallback = pChunkCallback;
}