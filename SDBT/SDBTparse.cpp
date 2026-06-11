// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBTparse
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Monday 27 February 2023 - 10:35AM
// ---------------------------------------------------------------------------

#include <SDBT/SDBTparse.h>

#include <SDB/SDBcontent.h>
#include <SDB/SDBnumber.h>
#include <SDB/SDBenvelope.h>


#include <COL/COLlog.h>
COL_LOG_MODULE;

void SDBTtruncate(COLstring* pBuffer, const char* pIn){
   COL_FUNCTION(SDBTtruncate);
   int Shift = pIn - pBuffer->get_buffer();
   if (Shift > 0){
      COL_TRC("Truncating buffer");
      pBuffer->shift(Shift);
   }
}

void SDBTparse(COLstring* pBuffer, int* pCount){
   COL_FUNCTION(SDBTparse);
   int AmountRead  = 0;
   const char* pIn = pBuffer->get_buffer();
   int BufferSize = pBuffer->size();
   do{
      COLuint64 ContentSize = 0;
      int AmountRead = SDBreadNumber(&ContentSize, pIn, BufferSize);
      if (AmountRead > 0){
         // We have a message
         if (BufferSize < AmountRead + ContentSize){
            // we are done - truncate
            COL_TRC("End of current buffer");
            SDBTtruncate(pBuffer, pIn);
            return;
         } else {
            pIn += AmountRead + ContentSize;
            BufferSize -= AmountRead + ContentSize;
            COL_TRC("Found a message");
            *pCount +=1;
         }
      } else {
         COL_TRC("Not enough data to read envelope");
         SDBTtruncate(pBuffer, pIn);
         return;
      }

   }while (true);
}