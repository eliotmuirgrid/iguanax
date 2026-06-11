// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBstring
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Thursday 23 February 2023 - 01:19PM
// ---------------------------------------------------------------------------

#include <SDB/SDBstring.h>
#include <SDB/SDBnumber.h>

#include <COL/COLerror.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

int SDBwriteString(const COLstring& In, char* pBuffer){
   COL_FUNCTION(SDBwriteString);
   int Written = SDBwriteNumber(In.size(), pBuffer);
   memcpy(pBuffer+Written, In.c_str(), In.size());
   Written += In.size();
   return Written;
}

int SDBreadString(COLstring* pValue, const char* pBuffer, int BufSize) {
   COL_FUNCTION(SDBreadString);
   COLuint64 Size = 0;
   int Amount = SDBreadNumber(&Size, pBuffer, BufSize);
   if (Amount < 0) { return Amount; }  // read number errors
   if (BufSize < Amount + Size) { return -1; }  // need more data for string
   COL_VAR(Size);
   if (Size >= 1073741824) { COLcerr << "WARNING: Trying to read a 1 GiB string." << newline; }
   pValue->setCapacity(Size);
   pValue->setSize(Size);
   memcpy(pValue->get_buffer(), pBuffer + Amount, Size);
   return Amount + Size;
}

int SDBstringSize(const COLstring& String) {
   return SDBnumberSize(String.size()) + String.size();
}
