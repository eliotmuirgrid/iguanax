// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNpkcs7Padd
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Friday 16 June 2023 - 08:35AM
// ---------------------------------------------------------------------------

#include <TUN/TUNpkcs7Pad.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void TUNpkcs7Pad(COLstring* pString){
   COL_FUNCTION(TUNpkcs7Pad);
   COL_DUMP("In:", pString->c_str(), pString->size());
   int PaddingNeeded = 16 - pString->size() % 16;
   if (PaddingNeeded == 0) {
      PaddingNeeded = 16;
      COL_TRC("Perfectly divisible by 16 so we add sixteen 0x10 bytes");
   }
   COL_VAR(PaddingNeeded); 
   int NewSize = pString->size() + PaddingNeeded;
   if (pString->capacity() <= NewSize){
      pString->setCapacity(NewSize);
   }
   char* pStart = pString->get_buffer() + pString->size();
   char* pEnd   = pStart + PaddingNeeded;
   for (; pStart != pEnd; pStart++){
      *pStart = PaddingNeeded;
   }
   pString->setSize(NewSize);
   COL_DUMP("Out:", pString->c_str(), pString->size());
}

void TUNpkcs7Unpad(COLstring* pString){
   COL_FUNCTION(TUNpkcs7Unpad);
   COL_DUMP("In:", pString->c_str(), pString->size());
   int Padding = pString->c_str()[pString->size()-1];
   pString->setSize(pString->size()-Padding);
   COL_DUMP("Out:", pString->c_str(), pString->size());
}