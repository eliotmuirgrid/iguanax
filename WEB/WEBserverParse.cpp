// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: WEBserverParse
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Monday 22 May 2023 - 04:06PM
// ---------------------------------------------------------------------------

#include <WEB/WEBserverParse.h>
#include <WEB/WEBhandle.h>
#include <WEB/WEBrequestData.h>

#include <math.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;


void WEBtruncateBuffer(COLstring* pBuffer, int ChunkSize){
   COL_FUNCTION(WEBtruncateBuffer);
   COL_VAR(pBuffer->size());
   if (ChunkSize == pBuffer->size()){
      pBuffer->setSize(0);
   }else{
      *pBuffer = pBuffer->substr(ChunkSize, pBuffer->size()-ChunkSize);  // TODO memcpy could be faster here?
   }
   COL_TRC("Buffer truncated by " << ChunkSize << " to " << pBuffer->size());
}

bool WEBserverParse(COLstring* pBuffer, WEBrequestData* pRequestData, int* pTotalSize){
   COL_FUNCTION(WEBserverParse);
   COLstring& Buffer = *pBuffer;
   COL_DUMP("Headers:", Buffer.c_str(), Buffer.size());
   int HeaderSize;
   Buffer.find(&HeaderSize, "\r\n\r\n");
   WEBrequestParse(Buffer, pRequestData);
   COL_VAR(*pRequestData);
   if (pRequestData->Headers.count("content-length")){
      int Size = atoi(pRequestData->Headers["content-length"].c_str());
      COL_VAR(Size);
      if (Buffer.size() >= HeaderSize + 4 + Size){
         COL_TRC("We have a complete request!");
         int CompleteSize = HeaderSize + 4 + Size;
         COL_DUMP("Request:", Buffer.c_str(), CompleteSize);
         pRequestData->Body = Buffer.substr(HeaderSize+4, Size);
         COL_DUMP("Body:", pRequestData->Body.c_str(), pRequestData->Body.size());
         if (pTotalSize){
            COL_TRC("We want to know the number of bytes consumed and will not truncate buffer.");
            *pTotalSize = CompleteSize;
            return true;
         }
         WEBtruncateBuffer(pBuffer, CompleteSize);
         return true;
      }
   } else {
      COL_TRC("No content-length - assume there is no body.");
      pRequestData->Body.clear();
      if (pTotalSize){
         COL_TRC("We want to know the number of bytes consumed and will not truncate buffer.");
         *pTotalSize = HeaderSize+4;
         return true;
      }
      WEBtruncateBuffer(pBuffer, HeaderSize+4);
      return true;
   }
   // TODO - no support for chunk encoded.
   return false;
}