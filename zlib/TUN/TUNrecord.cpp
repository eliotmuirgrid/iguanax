// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNrecord
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Wednesday 14 June 2023 - 11:44AM
// ---------------------------------------------------------------------------

#include <TUN/TUNrecord.h>
#include <TUN/TUNnumber.h>
#include <TUN/TUNrecordType.h>
#include <TUN/TUNrequest.h>

#include <SCK/SCKconnectionWrite.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void TUNrecordWriteString(COLstring* pOut, COLuint32 Type, const char* pData, COLuint32 SizeOfData){
   COL_FUNCTION(TUNrecordWriteString);
   char LengthBuffer[6];
   int SizeOfLength = TUNencodeNumber(SizeOfData, LengthBuffer);
   char TypeBuffer[6];
   int SizeOfType = TUNencodeNumber(Type, TypeBuffer);

   int RequiredSize = pOut->size() + SizeOfLength + SizeOfData + SizeOfType;
   COL_VAR4(SizeOfData, SizeOfLength, SizeOfType, RequiredSize);
   if (pOut->capacity() <= RequiredSize){
      COL_TRC("Increasing capacity to " << RequiredSize);
      pOut->setCapacity(RequiredSize);
   }
   unsigned char* pStart = (unsigned char*)pOut->get_buffer() + pOut->size();
   memcpy(pStart,LengthBuffer, SizeOfLength);
   pStart +=SizeOfLength;
   memcpy(pStart,TypeBuffer, SizeOfType);
   pStart +=SizeOfType;
   memcpy(pStart,pData, SizeOfData);
   pOut->setSize(RequiredSize);
   COL_DUMP("OutBuffer:", pOut->c_str(), pOut->size());
}

void TUNrecordWriteString(COLstring* pOut, COLuint32 Type, const COLstring& Data){
   TUNrecordWriteString(pOut, Type, Data.c_str(), Data.size());
}

void TUNrecordWrite(TUNrequestItem* pRequest, COLuint32 Type, const char* pData, COLuint32 SizeOfData){
   COL_FUNCTION(TUNrecordWrite);
   TUNrecordOutTrace((TUNrecordType)Type, pRequest->ConnectionId);
   TUNrecordWriteString(pRequest->pOut, Type, pData, SizeOfData);
}

void TUNrecordWrite(TUNrequestItem* pOut, COLuint32 Type, const COLstring& Data){
   TUNrecordWrite(pOut, Type, Data.c_str(), Data.size());
}

void TUNrecordConnectionWrite(SCKloop* pLoop, COLuint64 ConnectionId, COLuint32 Type, const COLstring& Data){
   COL_FUNCTION(TUNrecordConnectionWrite);
   TUNrecordOutTrace((TUNrecordType)Type, ConnectionId);
   COLstring Record;
   TUNrecordWriteString(&Record, Type, Data);
   SCKconnectionWrite(pLoop, ConnectionId, Record);
}

void TUNtruncate(COLstring* pBuffer, int ChunkToRemove){
   COL_FUNCTION(TUNtruncate);
   // if (pBuffer->size() == ChunkToRemove){
   //    COL_TRC("Clearing record buffer.");
   //    pBuffer->clear();
   //    return;
   // }
   // We actually want memmove here and should not assume memcpy behaves the same as memmove
   // In fact, the Clang address sanitizer complains about memcpy-param-overlap here
   // memcpy(pBuffer->get_buffer(), pBuffer->get_buffer() + ChunkToRemove, pBuffer->size() - ChunkToRemove);
   // pBuffer->setSize(pBuffer->size() - ChunkToRemove);
   // This function is equivalent to COLstring::shift()
   pBuffer->shift(ChunkToRemove);
}

int TUNrecordRead(const unsigned char* pBuffer, int SizeOfBuffer, COLuint32* pType, COLstring* pRecord){
   COL_FUNCTION(TUNrecordRead);
   COLuint32 SizeOfData = 0;
   int SizeOfSize = TUNdecodeNumber(&SizeOfData, pBuffer);
   if (SizeOfSize > SizeOfBuffer){
      COL_TRC("We don't have enough data to even get the size of the record.");
      return 0;
   }
   if (SizeOfData > 0xFFFFFF){
      // TODO - we should fail completely - this is too big of record.
      return -1;
   }
   int SizeOfType = TUNdecodeNumber(pType, pBuffer+SizeOfSize);
   int RequiredSize = SizeOfData + SizeOfSize + SizeOfType;
   if (SizeOfBuffer < RequiredSize){
      COL_TRC("We need " << RequiredSize << " but only have " << SizeOfBuffer);
      return 0;
   }
   //*pType = *(pBuffer + SizeOfSize);
   pRecord->setCapacity(SizeOfData);
   pRecord->setSize(SizeOfData);
   memcpy(pRecord->get_buffer(), pBuffer + SizeOfSize + SizeOfType, SizeOfData);
   return RequiredSize;
}