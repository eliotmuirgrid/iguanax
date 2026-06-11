// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBmagicMarker
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Friday 24 February 2023 - 10:31AM
// ---------------------------------------------------------------------------

#include <SDB/SDBmagicMarker.h>
#include <SDB/SDBnumber.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

bool operator==(const SDBmagicMarker& lhs, const SDBmagicMarker& rhs) {
   return lhs.Type == rhs.Type;
}

COLostream& operator<<(COLostream& Stream, const SDBmagicMarker& Obj){
    Stream << "MAJIK Marker";
    return Stream;
}

int SDBmagicMarkerSize(const SDBmagicMarker* pMarker){
   COL_FUNCTION(SDBmagicMarkerSize);
   return SDBnumberSize(SDB_MARKER) + 4;
}

int SDBwriteMagicMarker(const SDBmagicMarker* pMarker, char* pBuffer, int BufferSize){
   COL_FUNCTION(SDBwriteMagicMarker);
   if (BufferSize < SDBmagicMarkerSize(pMarker)) {
      COL_ERR("Not enough data in buffer for a magic marker");
      return -1;
   }
   char* pOut = pBuffer;
   pOut += SDBwriteNumber(pMarker->Type, pOut);
   memcpy(pOut, "ABCD", 4);
   pOut += 4;
   return pOut - pBuffer;
}

int SDBreadMagicMarker(SDBmagicMarker* pMarker, const char* pBuffer, int BufferSize){
   COL_FUNCTION(SDBreadMagicMarker);
   if (BufferSize < SDBmagicMarkerSize(pMarker)) {
      COL_ERR("Buffer size not large enough for magic marker");
      return -1;
   }
   const char* pIn = pBuffer;
   pIn += SDBreadNumber(&pMarker->Type, pIn, BufferSize);
   if (pMarker->Type != SDB_MARKER){
      COL_ERR("Not a majik marker type.");
      return -2;
   }
   if (strncmp(pIn, "ABCD", 4) != 0){
      COL_ERR("Majik marker should have ABCD");
      return -3;
   }
   pIn += 4;
   return pIn - pBuffer;
}

int SDBmagicMarkerSize(const SDBcontent* pMarker) {
   return SDBmagicMarkerSize((const SDBmagicMarker*)pMarker);
}
int SDBwriteMagicMarker(const SDBcontent* pMarker, char* pBuffer, int BufferSize) {
   return SDBwriteMagicMarker((const SDBmagicMarker*)pMarker, pBuffer, BufferSize);
}
int SDBreadMagicMarker(SDBcontent* pMarker, const char* pBuffer, int BufferSize) {
   return SDBreadMagicMarker((SDBmagicMarker*)pMarker, pBuffer, BufferSize);
}

COLostream& SDBprintMagicMarker(const SDBcontent* pMarker, COLostream& Stream) {
   return Stream << "Magic Marker";
}

int SDBreadNextMagicMarkerOffset(const char* pBuffer, int BufferSize){
   COL_FUNCTION(SDBreadNextMagicMarkerOffset);
   if (BufferSize < SDBmagicMarkerSize((SDBmagicMarker*)NULL)) {
      COL_ERR("Not enough data in buffer for magic marker");
      return -1;
   }
   // search for the next marker
   const char* pIn = pBuffer;
   while (strncmp(pIn++, "ABCD", 4)) {
      if (pIn == pBuffer + BufferSize) {
         COL_ERR("Failed to find a magic marker");
         return -1;
      }
   }
   // pIn now points at 'B' in "ABCD". Adjust pIn
   pIn -= SDBnumberSize(SDB_MARKER) + 1;
   return pIn - pBuffer;
}