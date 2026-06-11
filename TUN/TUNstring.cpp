// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNstring
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Wednesday 14 June 2023 - 03:59PM
// ---------------------------------------------------------------------------

#include <TUN/TUNstring.h>
#include <TUN/TUNnumber.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

TUNcursor::TUNcursor(const COLstring& String) { 
   pBuffer=(const unsigned char*)String.get_buffer();
   Size = String.size();
}
   
void TUNstringWrite(COLstring* pOutBuffer, const COLstring& String){
   COL_FUNCTION(TUNstringWrite);
   int RequiredLength = String.size() + 5 + pOutBuffer->size()+1;
   if (RequiredLength >= pOutBuffer->capacity()){
      pOutBuffer->setCapacity(RequiredLength);
   }
   int SizeOfSize = TUNencodeNumber(String.size(), pOutBuffer->get_buffer() + pOutBuffer->size());
   memcpy(pOutBuffer->get_buffer() + pOutBuffer->size() + SizeOfSize, String.c_str(), String.size());
   pOutBuffer->setSize(pOutBuffer->size()+ SizeOfSize+ String.size());
}

bool TUNstringRead(TUNcursor* pCursor, COLstring* pString){
   COL_FUNCTION(TUNstringRead);
   COLuint32 SizeOfData = 0;
   int SizeOfSize = TUNdecodeNumber(&SizeOfData, pCursor->pBuffer);
   if (SizeOfSize > pCursor->Size){
      COL_TRC("We don't have enough data to even get the size of the record.");
      return false;
   }
   if (SizeOfData > 0xFFFFF){
      // TODO - we should fail completely - this is too big of a string.
      return false;
   }
   int RequiredSize = SizeOfData + SizeOfSize;
   if (pCursor->Size < RequiredSize){
      COL_TRC("We need " << RequiredSize << " but only have " << pCursor->Size);
      return false;
   }
   pString->setCapacity(SizeOfData);
   pString->setSize(SizeOfData);
   memcpy(pString->get_buffer(), pCursor->pBuffer + SizeOfSize, SizeOfData);
   pCursor->pBuffer += RequiredSize;
   pCursor->Size -= RequiredSize;
   return true;
}


bool TUNnumberRead(TUNcursor* pCursor, COLuint32* pNumber){
   COL_FUNCTION(TUNnumberRead);
   int SizeOfNumber = TUNdecodeNumber(pNumber, pCursor->pBuffer);
   if (SizeOfNumber > pCursor->Size){
      COL_TRC("We don't have enough data to even get the size of the record.");
      return false;
   }
   pCursor->pBuffer += SizeOfNumber;
   pCursor->Size    -= SizeOfNumber;
   return true;
}